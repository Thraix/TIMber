#pragma once

#include <math/Vec3.h>

struct IntersectionData
{
  bool hasIntersection = false;
  Greet::Vec3<float> v1;
  Greet::Vec3<float> v2;
  Greet::Vec3<float> v3;
  Greet::Vec3<float> intersectionPoint;
  float distanceFromNear;
};

class RayCast 
{
  public:
    RayCast() = delete;
    virtual ~RayCast();

    static IntersectionData Cube(const Greet::Vec3<float>& near, const Greet::Vec3<float>& far,const Greet::Vec3<float>& boxMin, const Greet::Vec3<float>& boxMax);
    static bool CheckCube(const Greet::Vec3<float>& near, const Greet::Vec3<float>& far,const Greet::Vec3<float>& boxMin, const Greet::Vec3<float>& boxMax);
};

