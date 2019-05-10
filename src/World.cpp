#include "World.h"

#include "LineRenderer.h"

using namespace Greet;

World::World(uint width, uint length)
  : player{this, {width * Chunk::CHUNK_WIDTH / 2.0f, Chunk::CHUNK_HEIGHT, length * Chunk::CHUNK_LENGTH/ 2.0f}},
  skybox{Greet::TextureManager::Get3D("skybox")},
  width{width}, length{length}, terrainMaterial{Greet::Shader::FromFile("res/shaders/terrain.shader")}, noiseTexture{TextureManager::Get2D("noiseMap")}
{
  chunks = new Chunk[width * length];
  for(int z = 0;z < length; z++)
  {
    for(int x = 0;x < width; x++)
    {
      chunks[x+z*width].Initialize(x,z);
      Log::Info("Initialized ", x+z*width+1, "/", width*length, " chunks");
    } 
  }
  player.OnWorldInit();
}

void World::Render() const 
{
  skybox.Render(player.GetCamera());
  if(chunkIntersection.hasIntersection)
  {
    LineRenderer::GetInstance().DrawLine(player.GetCamera(), chunkIntersection.v1, chunkIntersection.v2, Vec4(1,1,1,1));
    LineRenderer::GetInstance().DrawLine(player.GetCamera(), chunkIntersection.v2, chunkIntersection.v3, Vec4(1,1,1,1));
    LineRenderer::GetInstance().DrawLine(player.GetCamera(), chunkIntersection.v3, chunkIntersection.v1, Vec4(1,1,1,1));
    cursor.Render(player.GetCamera(), chunkIntersection);
  }

  terrainMaterial.Bind(&player.GetCamera());
  noiseTexture.Enable();
  for(int z = 0;z < length; z++)
  {
    for(int x = 0;x < width; x++)
    {
      terrainMaterial.GetShader().SetUniformMat4("transformationMatrix", chunks[x + z * width].GetTransformationMatrix());
      MCMesh* mesh = chunks[x + z * width].mesh;
      mesh->Bind();
      mesh->Render();
      mesh->Unbind();
#if 0
      Greet::Mesh* mesh2 = chunks[x + z * width].originalMesh;
      mesh2->Bind();
      mesh2->Render();
      mesh2->Unbind();
#endif
    }
  }
  noiseTexture.Disable();
  terrainMaterial.Unbind();
  player.Render();
}

void World::Update(float timeElapsed) 
{
  player.Update(timeElapsed);
  Vec3<float> pos = player.GetPosition();
  int chunkMinX = floor((pos.x - player.GetReach()) / (double)Chunk::CHUNK_WIDTH);
  int chunkMinZ = floor((pos.z- player.GetReach()) / (double)Chunk::CHUNK_LENGTH);
  int chunkMaxX = ceil((pos.x + player.GetReach()) / (double)Chunk::CHUNK_WIDTH);
  int chunkMaxZ = ceil((pos.z + player.GetReach()) / (double)Chunk::CHUNK_LENGTH);
  chunkIntersection.hasIntersection = false;
  for(int z = chunkMinZ; z < chunkMaxZ; z++)
  {
    if(z < 0 || z >= length)
      continue;
    for(int x = chunkMinX; x < chunkMaxX; x++)
    {
      if(x < 0 || x >= width)
        continue;
      IntersectionData data = chunks[x + z * width].RayCastChunk(player.GetCamera());
      if(data.hasIntersection)
      {
        if(!chunkIntersection.hasIntersection || data.distanceFromNear < chunkIntersection.distanceFromNear)
          chunkIntersection = data;
      } 
    }
  }
  for(int z = 0;z<length;z++)
  {
    for(int x = 0;x<width;x++)
    {
      chunks[x + z * width].Update(timeElapsed);

    }
  }
}

void World::OnEvent(Greet::Event& event) 
{
  player.OnEvent(event);
}

void World::SphereOperation(const Vec3<float>& point, float radius, std::function<void(MCPointData, int, int,int, float distanceSQ, bool inside)> func)
{
  int pX = (int)floor(point.x);
  int pY = (int)floor(point.y);
  int pZ = (int)floor(point.z);

  // Calculate the lower and upper bound for the for loops
  Vec3<int> min = Vec::Max(Vec3<int>{pX, pY, pZ} - radius - 1, {0});
  Vec3<int> max = Vec::Min(Vec3<int>{pX, pY, pZ} + (radius + 1)+1, {Chunk::CHUNK_WIDTH * width+1, Chunk::CHUNK_HEIGHT, Chunk::CHUNK_LENGTH * length+1});

  float radiusSQ = radius * radius;
  for(int z = min.z;z < max.z;z++)
  {
    for(int y = min.y;y < max.y;y++)
    {
      for(int x = min.x;x < max.x;x++)
      {
        float distanceSQ = (Vec3<float>{x,y,z} - point).LengthSQ();
        bool inside = distanceSQ <= radiusSQ;
        func(GetVoxelData(x,y,z), x, y, z, distanceSQ, inside);
      }
    }
  }
}


void World::PlaceVoxels()
{
  if(chunkIntersection.hasIntersection)
  {
    int radius = 4;
    SphereOperation(chunkIntersection.intersectionPoint, radius, [&] (MCPointData data, int x, int y, int z, float distanceSQ, bool inside)
        {
        Vec3<int> chunkOffset = GetChunkOffset(x,y,z);
        Vec3<int> chunkPos = GetChunkPos(x,y,z);

        if(
            (x > 0 && GetVoxelData(x-1,y,z).magnitude >= 0.0f) ||
            (y > 0 && GetVoxelData(x,y-1,z).magnitude >= 0.0f) ||
            (z > 0 && GetVoxelData(x,y,z-1).magnitude >= 0.0f) ||
            (x < Chunk::CHUNK_WIDTH*width && GetVoxelData(x+1,y,z).magnitude >= 0.0f) ||
            (y < Chunk::CHUNK_HEIGHT && GetVoxelData(x,y+1,z).magnitude >= 0.0f) ||
            (z < Chunk::CHUNK_LENGTH*length && GetVoxelData(x,y,z+1).magnitude >= 0.0f)
          )
        {
        float max = std::max(radius - sqrtf(distanceSQ), data.magnitude);
        if(data.magnitude < max)
        data.magnitude += (radius - sqrtf(distanceSQ)) * 0.1f;
        Math::Clamp(&data.magnitude, -1.0f, 1.0f);
        // UpdateVoxel(x,y,z, data);
        UpdateVoxel(chunkPos, chunkOffset, data);

        }
        });
    UpdateChunks();
  }
}


void World::RemoveVoxels()
{
  if(chunkIntersection.hasIntersection)
  {
    int radius = 4;
    SphereOperation(chunkIntersection.intersectionPoint, radius, [&] (MCPointData data, int x, int y, int z, float distanceSQ, bool inside)
        {
        Vec3<int> chunkOffset = GetChunkOffset(x,y,z);
        Vec3<int> chunkPos = GetChunkPos(x,y,z);

        //MCPointData& = GetChunk(chunkPos);

        if(
            (x > 0 && GetVoxelData(x-1,y,z).magnitude < 0.0f) ||
            (y > 0 && GetVoxelData(x,y-1,z).magnitude < 0.0f) ||
            (z > 0 && GetVoxelData(x,y,z-1).magnitude < 0.0f) ||
            (x < Chunk::CHUNK_WIDTH*width && GetVoxelData(x+1,y,z).magnitude < 0.0f) ||
            (y < Chunk::CHUNK_HEIGHT && GetVoxelData(x,y+1,z).magnitude < 0.0f) ||
            (z < Chunk::CHUNK_LENGTH*length && GetVoxelData(x,y,z+1).magnitude < 0.0f)
          )
        {
        float min = std::min(sqrtf(distanceSQ) - radius, data.magnitude);
        if(data.magnitude > min)
        data.magnitude += (sqrtf(distanceSQ) - radius) * 0.1f;
        Math::Clamp(&data.magnitude, -1.0f, 1.0f);
        // UpdateVoxel(x,y,z, data);
        UpdateVoxel(chunkPos, chunkOffset, data);

        }
        });
    UpdateChunks();
  }
}

void World::UpdateChunks()
{
  for(auto&& dirtyChunk : dirtyChunks)
  {
    GetChunk(dirtyChunk).UpdateMesh();
  }
  dirtyChunks.clear();
}

void World::UpdateVoxel(Vec3<int> chunkPos, Vec3<int> chunkOffset, const MCPointData& data)
{
  if(chunkPos.x == width)
  {
    chunkOffset.x = Chunk::CHUNK_WIDTH;
    chunkPos.x--;
  }

  if(chunkPos.z == length)
  {
    chunkOffset.z = Chunk::CHUNK_LENGTH;
    chunkPos.z--;
  }
  GetChunk(chunkPos).UpdateVoxel(chunkOffset.x,chunkOffset.y,chunkOffset.z,data);
  dirtyChunks.emplace(chunkPos);
  if(chunkOffset.x == 0 && chunkPos.x != 0)
  {
    GetChunk({chunkPos.x-1, chunkPos.y, chunkPos.z}).UpdateVoxel(Chunk::CHUNK_WIDTH,chunkOffset.y,chunkOffset.z,data);
    dirtyChunks.emplace(Vec3<int>{chunkPos.x-1, chunkPos.y, chunkPos.z});
  }
  if(chunkOffset.z == 0 && chunkPos.z != 0)
  {
    GetChunk({chunkPos.x, chunkPos.y, chunkPos.z-1}).UpdateVoxel(chunkOffset.x,chunkOffset.y,Chunk::CHUNK_LENGTH,data);
    dirtyChunks.emplace(Vec3<int>{chunkPos.x, chunkPos.y, chunkPos.z-1});
  }
  if(chunkOffset.x == 0 && chunkPos.x != 0 && chunkOffset.z == 0 && chunkPos.z != 0)
  {
    GetChunk({chunkPos.x-1, chunkPos.y, chunkPos.z-1}).UpdateVoxel(Chunk::CHUNK_WIDTH,chunkOffset.y,Chunk::CHUNK_LENGTH,data);
    dirtyChunks.emplace(Vec3<int>{chunkPos.x-1, chunkPos.y, chunkPos.z-1});
  }
}

MCPointData World::GetVoxelData(const Vec3<int>& chunk, const Vec3<int>& chunkPos)
{
  return chunks[chunk.x + chunk.z * width].GetVoxelData(chunkPos.x, chunkPos.y, chunkPos.z);
}

MCPointData World::GetVoxelData(int x, int y, int z)
{
  Vec3<int> chunkOffset = GetChunkOffset(x,y,z);
  Vec3<int> chunkPos = GetChunkPos(x,y,z);
  if(chunkPos.x == width)
  {
    chunkOffset.x = Chunk::CHUNK_WIDTH;
    chunkPos.x--;
  }

  if(chunkPos.z == length)
  {
    chunkOffset.z = Chunk::CHUNK_LENGTH;
    chunkPos.z--;
  }
  return chunks[chunkPos.x + chunkPos.z * width].GetVoxelData(chunkOffset.x, chunkOffset.y, chunkOffset.z);
}

Vec3<int> World::GetChunkOffset(int x, int y, int z)
{
  return {
    x % Chunk::CHUNK_WIDTH,
      y % Chunk::CHUNK_HEIGHT,
      z % Chunk::CHUNK_LENGTH
  };
}

Vec3<int> World::GetChunkPos(int x, int y, int z)
{
  return {
    x/Chunk::CHUNK_WIDTH,
      y/Chunk::CHUNK_HEIGHT,
      z/Chunk::CHUNK_LENGTH
  };
}

Chunk& World::GetChunk(const Vec3<int>& chunk)
{
  return chunks[chunk.x + chunk.z * width];
}
