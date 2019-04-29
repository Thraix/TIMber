#pragma once

#include <graphics/models/Mesh.h>
#include <graphics/models/TPCamera.h>
#include <utils/Noise.h>
#include "MCMesh.h"
#include "RayCast.h"

class Chunk
{
  friend class World;
  private:
    static uint CHUNK_WIDTH;
    static uint CHUNK_HEIGHT;
    static uint CHUNK_LENGTH;

    MCMesh* mesh;
    std::vector<MCPointData> voxelData;
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

    void SphereOperation(const Greet::Vec3<float>& point, float radius, std::function<void(MCPointData&, int, int, int)> func);
    void PlaceVoxels(const Greet::Vec3<float>& point, float radius);
    void RemoveVoxels(const Greet::Vec3<float>& point, float radius);
};
