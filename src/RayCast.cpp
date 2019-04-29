#include "RayCast.h"

using namespace Greet;

bool RayCast::CheckCube(const Vec3<float>& near, const Vec3<float>& far,const Vec3<float>& boxMin, const Vec3<float>& boxMax)
{
  // Check if we are inside the cube
  if(boxMin.x >= near.x && boxMax.x <= near.x && boxMin.y >= near.y && boxMax.y <= near.y && boxMin.z >= near.z && boxMax.z <= near.z)
    return true;

  Vec3<float> direction = far - near;
  if(direction.x != 0)
  {
    if(direction.x < 0)
    {
      float t = (boxMax.x - near.x) / direction.x;
      Vec3<float> intersection = near + direction * t;
      if(intersection.y >= boxMin.y && intersection.y <= boxMax.y && 
          intersection.z >= boxMin.z && intersection.z <= boxMax.z)
        return true;
    }
    else
    {
      float t = (boxMin.x - near.x) / direction.x;
      Vec3<float> intersection = near + direction * t;
      if(intersection.y >= boxMin.y && intersection.y <= boxMax.y && 
          intersection.z >= boxMin.z && intersection.z <= boxMax.z)
        return true;
    }
  } 

  if(direction.y != 0)
  {
    if(direction.y < 0)
    {
      float t = (boxMax.y - near.y) / direction.y;
      Vec3<float> intersection = near + direction * t;
      if(intersection.x >= boxMin.x && intersection.x <= boxMax.x && 
          intersection.z >= boxMin.z && intersection.z <= boxMax.z)
        return true;
    }
    else
    {
      float t = (boxMin.y - near.y) / direction.y;
      Vec3<float> intersection = near + direction * t;
      if(intersection.x >= boxMin.x && intersection.x <= boxMax.x && 
          intersection.z >= boxMin.z && intersection.z <= boxMax.z)
        return true;
    }
  } 

  if(direction.z != 0)
  {
    if(direction.z < 0)
    {
      float t = (boxMax.z - near.z) / direction.z;
      Vec3<float> intersection = near + direction * t;
      if(intersection.x >= boxMin.x && intersection.x <= boxMax.x && 
          intersection.y >= boxMin.y && intersection.y <= boxMax.y)
        return true;
    }
    else
    {
      float t = (boxMin.z - near.z) / direction.z;
      Vec3<float> intersection = near + direction * t;
      if(intersection.x >= boxMin.x && intersection.x <= boxMax.x && 
          intersection.y >= boxMin.y && intersection.y <= boxMax.y)
        return true;
    }
  } 
  return false;
}

IntersectionData RayCast::MCMeshCast(const Vec3<float>& near, const Vec3<float>& far, const MCMesh& mesh)
{
  // This is a vector of pairs with faces and their intersection distance from the near point.
  IntersectionData intersection;
  for(auto&& face : mesh.GetFaces())
  {
    Vec3<float> v1 = mesh.GetVertices()[face.v1];
    Vec3<float> v2 = mesh.GetVertices()[face.v2];
    Vec3<float> v3 = mesh.GetVertices()[face.v3];
    Vec3<float> cross = (v2 - v1).Cross(v3 - v1);
    // Since the mesh can have invalid faces we need to check that the cross product is valid.
    if(cross == Vec3{0.0f})
      continue;
    // Check so that the normals are facing opposite directions.
    // This is so that we are not raytracing triangles that are facing
    // away from us.
    if(cross.Dot(far - near) > 0)
      continue;
    if(NegativeVolumeTetrahedron(near, v1, v2, v3) != NegativeVolumeTetrahedron(far,v1,v2,v3))
    {
      bool s1 = NegativeVolumeTetrahedron(near, far, v1, v2);
      bool s2 = NegativeVolumeTetrahedron(near, far, v2, v3);
      bool s3 = NegativeVolumeTetrahedron(near, far, v3, v1);
      if(s1 == s2 && s2 == s3)
      {
        // We have an intersection

        Vec3<Vec3<float>> triangleFace = {v1,v2,v3};
        Vec3<float> intersectionPoint = LineIntersectPlane(near, far, triangleFace);
        float distanceFromNear = (intersectionPoint - near).Length();
        if(!intersection.hasIntersection || distanceFromNear < intersection.distanceFromNear)
        {
          intersection = {
            .hasIntersection = true,
            .v1 = v1,
            .v2 = v2,
            .v3 = v3,
            .intersectionPoint = intersectionPoint,
            .distanceFromNear = distanceFromNear
          };
        }
      }
    }
  }
  return intersection;
}

bool RayCast::NegativeVolumeTetrahedron(const Vec3<float>& v1,const Vec3<float>& v2,const Vec3<float>& v3,const Vec3<float>& v4)
{
  return std::signbit(SignedVolumeTetrahedron(v1,v2,v3,v4));
}

float RayCast::SignedVolumeTetrahedron(const Vec3<float>& v1,const Vec3<float>& v2,const Vec3<float>& v3,const Vec3<float>& v4)
{
  return (v2-v1).Cross(v3 - v1).Dot(v4-v1) * (1.0f/6.0f);
}

Vec3<float> RayCast::LineIntersectPlane(const Vec3<float>& near, const Vec3<float>& far, const Vec3<Vec3<float>>& plane)
{
  Vec3<float> normal = (plane[1] - plane[0]).Cross(plane[2] - plane[0]);
  float d = normal.Dot(plane[0]);
  float t = (d - normal.Dot(near)) / normal.Dot(far-near);
  return near + (far - near) * t;
}
