#pragma once

#include "RayCast.h"
#include <graphics/models/EntityModel.h>

class TerrainCursor 
{
  private:
    Greet::Material material;
    Greet::Mesh* mesh;
  public:
    TerrainCursor();
    virtual ~TerrainCursor();
    void Render(const Greet::Camera& camera, const IntersectionData& intersection) const;
};

