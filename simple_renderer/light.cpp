#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
  switch (type)
  {
    case LightType::POINT_LIGHT:
      this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
      break;
    case LightType::DIRECTIONAL_LIGHT:
      this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
      break;
    case LightType::AREA_LIGHT:
      // TODO: Implement this
      this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
      this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
      this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
      this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
      break;

    default:
      std::cout << "WARNING: Invalid light type detected";
      break;
  }

  this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
  this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction* si)
{
  LightSample ls;
  memset(&ls, 0, sizeof(ls));

  Vector3f radiance;
  switch (type)
  {
    case LightType::POINT_LIGHT:
      ls.wo = (position - si->p);
      ls.d = ls.wo.Length();
      ls.wo = Normalize(ls.wo);
      radiance = (1.f / (ls.d * ls.d)) * this->radiance;
      break;
    case LightType::DIRECTIONAL_LIGHT:
      ls.wo = Normalize(direction);
      ls.d = 1e10;
      radiance = this->radiance;
      break;
    case LightType::AREA_LIGHT:
      ls.wo = (center + (2 * next_float() - 1) * vx + (2 * next_float() - 1) * vy - si->p);
      ls.d = ls.wo.Length();
      ls.wo = Normalize(ls.wo);
      ls.wo = Normalize(Dot(normal, -ls.wo) * ls.wo);
      radiance = Dot(normal, -ls.wo) * this->radiance * 4 * vx.Length() * vy.Length() * (1.f / (ls.d * ls.d));
      break;
  }
  return { radiance, ls };
}

Interaction Light::intersectLight(Ray* ray)
{
  Interaction si;
  memset(&si, 0, sizeof(si));

  if (type == LightType::AREA_LIGHT)
  {
    float dDotN = Dot(ray->d, this->normal);
    if (dDotN != 0.f)
    {
      float t = -Dot((ray->o - this->center), this->normal) / dDotN;
      Vector3f p = ray->o + ray->d * t;
      if (std::abs(Dot((p - this->center), Normalize(this->vx))) / this->vx.Length() < 1 &&
          std::abs(Dot((p - this->center), Normalize(this->vy))) / this->vy.Length() < 1)
      {
        if (t >= 0.f)
        {
          si.didIntersect = true;
          si.t = t;
          si.n = this->normal;
          si.emissiveColor = Dot(normal, -ray->d) * this->radiance * 4 * vx.Length() * vy.Length() * (1.f / (t * t));
        }
      }
    }
  }

  return si;
}
Interaction Light::CamIntersect(Ray* ray)
{
  Interaction si;
  memset(&si, 0, sizeof(si));

  if (type == LightType::AREA_LIGHT)
  {
    float dDotN = Dot(ray->d, this->normal);
    if (dDotN != 0.f)
    {
      float t = -Dot((ray->o - this->center), this->normal) / dDotN;
      Vector3f p = ray->o + ray->d * t;
      if (std::abs(Dot((p - this->center), Normalize(this->vx))) / this->vx.Length() < 1 &&
          std::abs(Dot((p - this->center), Normalize(this->vy))) / this->vy.Length() < 1)
      {
        if (t >= 0.f)
        {
          si.didIntersect = true;
          si.t = t;
          si.n = this->normal;
          si.emissiveColor = this->radiance;
        }
      }
    }
  }

  return si;
}

Interaction Light::intersectDirnLight(Ray* ray)
{
  Interaction si;
  memset(&si, 0, sizeof(si));

  if (type == LightType::AREA_LIGHT)
  {
    float dDotN = Dot(ray->d, this->normal);
    if (dDotN != 0.f)
    {
      float t = -Dot((ray->o - this->center), this->normal) / dDotN;
      Vector3f p = ray->o + ray->d * t;
      if (std::abs(Dot((p - this->center), Normalize(this->vx))) / this->vx.Length() < 1 &&
          std::abs(Dot((p - this->center), Normalize(this->vy))) / this->vy.Length() < 1)
      {
        if (t >= 0.f && Dot(normal, -ray->d) > 0)
        {
          si.didIntersect = true;
          si.t = t;
          si.n = this->normal;
          si.emissiveColor = this->radiance;
        }
      }
    }
  }

  return si;
}