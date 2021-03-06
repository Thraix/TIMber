#pragma once

#include <graphics/models/Mesh.h>
#include <utils/Noise.h>
#include "MCMesh.h"
#include "RayCast.h"
#include "PlayerCamera.h"

struct ChunkChange
{
  bool dirty = false;
  int minX, minY, minZ;
  int maxX, maxY, maxZ;
};

class Chunk
{
  public:
    friend class World;
  private:
    static uint CHUNK_WIDTH;
    static uint CHUNK_HEIGHT;
    static uint CHUNK_LENGTH;

    ChunkChange chunkChange;
    MCMesh* mesh;
    std::vector<MCPointData> voxelData;
    std::vector<float> heightMap;
    std::vector<float> biome;
    uint posX;
    uint posZ;
  private:
    Chunk();
    void Initialize(uint posX, uint posY);
    void AddTree(uint x, uint y, uint z);
  public:
    virtual ~Chunk();
    void Update(float timeElapsed);

    Greet::Mat4 GetTransformationMatrix() const { return Greet::Mat4::Translate({posX * CHUNK_WIDTH, 0 , posZ * CHUNK_LENGTH});}
    IntersectionData RayCastChunk(const Greet::Ref<Greet::Camera3D>& camera);


    void UpdateMesh();
    void UpdateVoxel(int x, int y, int z, const MCPointData& data);
    MCPointData& GetVoxelData(int x, int y, int z);
    const MCPointData& GetVoxelDataConst(int x, int y, int z) const;

    bool IsInside(int x, int y, int z);
    int GetVoxelIndex(int x, int y, int z);
};
