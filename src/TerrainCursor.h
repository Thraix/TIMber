#pragma once

#include "RayCast.h"
#include <graphics/models/EntityModel.h>

class TerrainCursor 
{
  private:
    Greet::Material material;
    Greet::Mesh* mesh;
    float time = 0.0f;
  public:
    TerrainCursor();
    virtual ~TerrainCursor();
    void Render(const Greet::Camera& camera, const IntersectionData& intersection) const;
    void Update(float timeElapsed);
};

