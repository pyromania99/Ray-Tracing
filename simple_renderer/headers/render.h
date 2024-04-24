#pragma once

#include "scene.h"

struct Integrator
{
  Integrator(Scene& scene);

  long long render(int spp, int sampling_method);

  Scene scene;
  Texture outputImage;
};