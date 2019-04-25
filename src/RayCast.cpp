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
