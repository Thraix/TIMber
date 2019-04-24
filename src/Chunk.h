#pragma once

#include <graphics/models/Mesh.h>
#include <utils/Noise.h>
#include "MarchingCubes.h"

class Chunk
{
  friend class World;
  private:
    static uint CHUNK_WIDTH;
    static uint CHUNK_HEIGHT;

    MarchingCubes* mesh;
    Greet::Mesh* originalMesh;
    float* heightMap;
    uint posX;
    uint posY;
  private:
    Chunk();
    void Initialize(uint posX, uint posY);

    void RecalcHeight(float* heightMap);
    void CalcGridVertexOffset(Greet::MeshData* data);
    void RecalcGrid(Greet::MeshData* data);

  public:
    virtual ~Chunk();
    // Values between 0-CHUNK_SIZE
    float GetHeight(const Greet::Vec3<float>& chunkPosition);
    Greet::Mat4 GetTransformationMatrix() const { return Greet::Mat4::Translate({posX * CHUNK_WIDTH, 0 , posY * CHUNK_HEIGHT});}
};
