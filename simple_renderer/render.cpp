#include "render.h"

Integrator::Integrator(Scene& scene)
{
  this->scene = scene;
  this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int spp, int sampling_method)
{
  auto startTime = std::chrono::high_resolution_clock::now();
  for (int x = 0; x < this->scene.imageResolution.x; x++)
  {
    for (int y = 0; y < this->scene.imageResolution.y; y++)
    {
      Vector3f result(0, 0, 0);
      for (int i = 0; i <= spp; ++i)
      {
        Ray cameraRay = this->scene.camera.generateRay(x, y);
        Interaction si = this->scene.rayIntersect(cameraRay);
        Interaction siEmitter = this->scene.CamEmitterIntersect(cameraRay);

        // comment this if statement if you want subsection 3 only
        if (si.didIntersect && si.t < siEmitter.t)
        {
          Vector3f radiance;
          if (sampling_method != 2)
          {
            // for loop over directions
            // just do this through spp itself?
            float theta;
            float phi;
            if (sampling_method == 0)
            {
              theta = 2.0 * M_PI * next_float();  // Azimuthal angle
              phi = asin(next_float());           // Polar angle//revert at submisiion
            }
            else if (sampling_method == 1)
            {
              theta = 2.0 * M_PI * next_float();  // Azimuthal angle
              phi = acos(sqrt(next_float()));     // Polar angle
            }

            Vector3f sample_dirn = Vector3f(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));

            Ray shadowRay(si.p + 1e-3f * si.n, si.toWorld(sample_dirn));

            Interaction siShadow = this->scene.rayIntersect(shadowRay);
            Interaction siBlockEmitter = this->scene.dirnEmitterIntersect(shadowRay);

            if (siBlockEmitter.didIntersect && siShadow.t > siBlockEmitter.t)
            {
              if (sampling_method == 0)
              {
                result += 2 * M_PI * si.bsdf->eval(&si, sample_dirn) * siBlockEmitter.emissiveColor *
                          std::abs(Dot(si.n, si.toWorld(sample_dirn))) / spp;
              }
              else if (sampling_method == 1)
              {
                result += M_PI * si.bsdf->eval(&si, sample_dirn) * siBlockEmitter.emissiveColor / spp;
              }
            }
          }
          else
          {
            // for loop over lights
            for (Light& light : this->scene.lights)
            {
              LightSample ls;
              std::tie(radiance, ls) = light.sample(&si);

              Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
              Interaction siShadow = this->scene.rayIntersect(shadowRay);
              Interaction siBlockEmitter = this->scene.rayEmitterIntersect(shadowRay);
              // checking if light is blocked by a shadow or another emitter
              if (siShadow.t > ls.d && siBlockEmitter.t >= ls.d)
              {
                result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)) / spp;
              }
              // if blocked by emitter , use emitter radiance instead ..do i even have to, maybe i just skip
              else if (siBlockEmitter.didIntersect && siShadow.t > siBlockEmitter.t)
              {
                result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * siBlockEmitter.emissiveColor *
                          std::abs(Dot(si.n, ls.wo)) / spp;
              }
            }
          }
        }
        else if (siEmitter.didIntersect)
        {
          // std::cout << (siEmitter.t) << std::endl;
          result += siEmitter.emissiveColor / spp;
        }
      }
      this->outputImage.writePixelColor(result, x, y);
    }
  }
  auto finishTime = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char** argv)
{
  if (argc != 5)
  {
    std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
    return 1;
  }
  Scene scene(argv[1]);

  Integrator rayTracer(scene);
  int spp = atoi(argv[3]);
  int sampling_method = atoi(argv[4]);

  auto renderTime = rayTracer.render(spp, sampling_method);

  std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
  rayTracer.outputImage.save(argv[2]);

  return 0;
}
