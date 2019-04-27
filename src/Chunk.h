#pragma once

#include <graphics/models/Mesh.h>
#include <graphics/models/TPCamera.h>
#include <utils/Noise.h>
#include "MarchingCubes.h"

class Chunk
{
  friend class World;
  private:
    static uint CHUNK_WIDTH;
    static uint CHUNK_HEIGHT;
    static uint CHUNK_LENGTH;

    MarchingCubes* mesh;
    MCPointData* voxelData;
    Greet::Mesh* originalMesh;
    float* heightMap;
    uint posX;
    uint posZ;
  private:
    Chunk();
    void Initialize(uint posX, uint posY);
  public:
    virtual ~Chunk();
    Greet::Mat4 GetTransformationMatrix() const { return Greet::Mat4::Translate({posX * CHUNK_WIDTH, 0 , posZ * CHUNK_LENGTH});}
    IntersectionData RayCastChunk(const Greet::TPCamera& camera);

    void PlaceVoxels(const Greet::Vec3<float>& point, float radius);
};
