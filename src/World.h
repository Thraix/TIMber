#pragma once

#include "Chunk.h"
#include "Player.h"
#include "TerrainCursor.h"

#include <internal/GreetTypes.h>
#include <graphics/layers/Scene.h>
#include <graphics/models/EntityModel.h>
#include <graphics/models/TPCamera.h>
#include <graphics/Window.h>
#include <graphics/Skybox.h>
#include <graphics/textures/TextureManager.h>

class World : public Greet::Scene
{
  Greet::Skybox skybox;
  Player player;

  Greet::Material terrainMaterial;
  const Greet::Texture2D& noiseTexture;
  uint width;
  uint length;
  Chunk* chunks;

  IntersectionData chunkIntersection;
  TerrainCursor cursor;

  std::set<Greet::Vec3<int>> dirtyChunks;

  public:

    World(uint width, uint length);

    void Render() const override;
    void Update(float timeElapsed) override;
    void OnEvent(Greet::Event& event) override;


    void PlaceVoxels();
    void RemoveVoxels();

    void PlacingFunction();

    void SphereOperation(const Greet::Vec3<float>& point, float radius, std::function<void(MCPointData, int, int,int, float distanceSQ, bool inside)> func);

  private:
    void UpdateChunks();
    void UpdateVoxel(Greet::Vec3<int> chunkPos, Greet::Vec3<int> chunkOffset, const MCPointData& data);
    inline MCPointData GetVoxelData(const Greet::Vec3<int>& chunk, const Greet::Vec3<int>& chunkPos);
    inline MCPointData GetVoxelData(int x, int y, int z);

    inline Greet::Vec3<int> GetChunkOffset(int x, int y, int z);
    inline Greet::Vec3<int> GetChunkPos(int x, int y, int z);

    inline Chunk& GetChunk(const Greet::Vec3<int>& chunk);
};
