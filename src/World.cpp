#include "World.h"

using namespace Greet;

World::World(uint width, uint length)
  : player{this, {width * Chunk::CHUNK_WIDTH / 2.0f, 0.0f, length * Chunk::CHUNK_HEIGHT/ 2.0f}},
  skybox{Greet::TextureManager::Get3D("skybox")},
  width{width}, length{length}, terrainMaterial{Greet::Shader::FromFile("res/shaders/terrain.shader")}
{
  chunks = new Chunk[width * length];
  for(int z = 0;z < length; z++)
  {
    for(int x = 0;x < width; x++)
    {
      chunks[x+z*width].Initialize(x,z);
    } 
  }
  player.OnWorldInit();
}

void World::Render() const 
{
  skybox.Render(player.GetCamera());

  terrainMaterial.Bind(&player.GetCamera());
  for(int z = 0;z < length; z++)
  {
    for(int x = 0;x < width; x++)
    {
      terrainMaterial.GetShader().SetUniformMat4("transformationMatrix", Greet::Mat4::Translate(x * Chunk::CHUNK_WIDTH, 0, z * Chunk::CHUNK_HEIGHT));
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
  terrainMaterial.Unbind();
  player.Render();
}

void World::Update(float timeElapsed) 
{
  player.Update(timeElapsed);
}

void World::OnEvent(Greet::Event& event) 
{
  player.OnEvent(event);
}

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
