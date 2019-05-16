#pragma once

#include "Voxels.h"

#include <internal/GreetTypes.h>

struct MCPointData
{
  const Voxel* voxel = &Voxel::grass;
  float magnitude = 0.0f; // from 0 - 1 
};

