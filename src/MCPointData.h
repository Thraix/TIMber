#pragma once

#include "Voxels.h"

#include <internal/GreetTypes.h>

struct MCPointData
{
  const Voxel* voxel = &Voxel::grass;
  float magnitude = -0.5f; // from 0 - 1 
};

