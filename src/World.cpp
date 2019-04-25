#include "World.h"

#include "LineRenderer.h"

using namespace Greet;

World::World(uint width, uint length)
  : player{this, {0,0,0}},//width * Chunk::CHUNK_WIDTH / 2.0f, 0.0f, length * Chunk::CHUNK_HEIGHT/ 2.0f}},
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
      Greet::Mesh* mesh = chunks[x + z * width].mesh->GetMesh();
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
  int chunkMinZ = floor((pos.z- player.GetReach()) / (double)Chunk::CHUNK_HEIGHT);
  int chunkMaxX = ceil((pos.x + player.GetReach()) / (double)Chunk::CHUNK_WIDTH);
  int chunkMaxZ = ceil((pos.z + player.GetReach()) / (double)Chunk::CHUNK_HEIGHT);
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
}

void World::OnEvent(Greet::Event& event) 
{
  player.OnEvent(event);
}

#if 0
float World::GetHeight(const Vec3<float>& position)
{
  int chunkX = floor(position.x / (double)Chunk::CHUNK_WIDTH);
  int chunkZ = floor(position.z / (double)Chunk::CHUNK_HEIGHT);
  if(chunkX < 0 || chunkZ < 0)
    return position.y;
  if(chunkX >= width || chunkZ >= length)
    return position.y;

  return chunks[chunkX + chunkZ * width].GetHeight(position - Vec3<float>(chunkX * Chunk::CHUNK_WIDTH, 0.0f, chunkZ * Chunk::CHUNK_HEIGHT));
}
#endif
