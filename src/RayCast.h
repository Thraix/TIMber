#pragma once

#include <math/Vec3.h>

#include "MCMesh.h"

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


    static IntersectionData MCMeshCast(const Greet::Vec3<float>& near, const Greet::Vec3<float>& far, const MCMesh& mesh);
    static IntersectionData Cube(const Greet::Vec3<float>& near, const Greet::Vec3<float>& far,const Greet::Vec3<float>& boxMin, const Greet::Vec3<float>& boxMax);
    static bool CheckCube(const Greet::Vec3<float>& near, const Greet::Vec3<float>& far,const Greet::Vec3<float>& boxMin, const Greet::Vec3<float>& boxMax);

  private:

    // Helper functions to calculate a raycast for a mesh
    static bool NegativeVolumeTetrahedron(
        const Greet::Vec3<float>& v1,
        const Greet::Vec3<float>& v2,
        const Greet::Vec3<float>& v3,
        const Greet::Vec3<float>& v4);

    static float SignedVolumeTetrahedron(
        const Greet::Vec3<float>& v1,
        const Greet::Vec3<float>& v2,
        const Greet::Vec3<float>& v3,
        const Greet::Vec3<float>& v4);

    static Greet::Vec3<float> LineIntersectPlane(
        const Greet::Vec3<float>& near, 
        const Greet::Vec3<float>& far, 
        const Greet::Vec3<Greet::Vec3<float>>& plane);
};

