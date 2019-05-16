#pragma once

#include "Voxels.h"

#include <internal/GreetTypes.h>

struct MCPointData
{
  const Voxel* voxel = &Voxel::grass;
  float magnitude = 0.5f; // from -1 - 1, negative is inside the mesh, positive outside
  inline bool Inhabited() const { return magnitude < 0.0f; }
};

