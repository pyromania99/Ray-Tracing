#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction
{
  // Position of interaction
  Vector3f p;
  // Normal of the surface at interaction
  Vector3f n;
  // The uv co-ordinates at the intersection point
  Vector2f uv;
  // The viewing direction in local shading frame
  Vector3f wi;
  // Distance of intersection point from origin of the ray
  float t = 1e30f;
  // Used for light intersection, holds the radiance emitted by the emitter.
  Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
  // BSDF at the shading point
  BSDF* bsdf;
  // Vectors defining the orthonormal basis
  Vector3f a, b, c;

  bool didIntersect = false;

  Vector3f toLocal(Vector3f w)
  {
    double x = w.x * c.x + w.y * c.y + w.z * c.z;
    double y = w.x * b.x + w.y * b.y + w.z * b.z;
    double z = w.x * a.x + w.y * a.y + w.z * a.z;

    return Vector3f(x, y, z);
  }

  Vector3f toWorld(Vector3f w)
  {
    double x = w.x * c.x + w.y * b.x + w.z * a.x;
    double y = w.x * c.y + w.y * b.y + w.z * a.y;
    double z = w.x * c.z + w.y * b.z + w.z * a.z;

    return Vector3f(x, y, z);
  }
};

//     Vector3f min_axis;
// if (z_.x <= z_.y && z_.x <= z_.z)
//   min_axis = z_.x * Vector3f(1, 0, 0);
// else if (z_.y <= z_.x && z_.y <= z_.z)
//   min_axis = z_.y * Vector3f(0, 1, 0);
// else
//   min_axis = z_.z * Vector3f(0, 0, 1);
