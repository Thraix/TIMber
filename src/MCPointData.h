#pragma once

#include "Voxels.h"

#include <internal/GreetTypes.h>

struct MCPointData
{
  bool inhabited;
  Voxel voxel = Voxel::grass;
  float magnitude; // from 0 - 1 
};

