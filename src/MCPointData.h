#pragma once

#include "Voxels.h"

#include <internal/GreetTypes.h>

struct MCPointData
{
  Voxel voxel = Voxel::grass;
  float magnitude = -0.5f; // from 0 - 1 
};

