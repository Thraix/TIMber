#include "Chunk.h"

#include "RayCast.h"

#include <graphics/models/MeshFactory.h> 
#include <graphics/models/TPCamera.h> 

using namespace Greet;

uint Chunk::CHUNK_WIDTH = 32;
uint Chunk::CHUNK_HEIGHT = 32;

Chunk::Chunk()
{}

void Chunk::Initialize(uint posX, uint posY)
{
  this->posX = posX;
  this->posY = posY;
  heightMap = Noise::GenNoise(CHUNK_WIDTH+1, CHUNK_HEIGHT+1,4,32, 32,0.75f, posX * CHUNK_WIDTH, posY * CHUNK_HEIGHT);
  float* caves = Noise::GenNoise(CHUNK_WIDTH+1,20, CHUNK_HEIGHT+1,4,32,32, 32,0.75f, posX * CHUNK_WIDTH, 0, posY * CHUNK_HEIGHT);
  {
    MeshData* data = MeshFactory::LowPolyGrid((CHUNK_WIDTH)/2.0,0,(CHUNK_HEIGHT)/2.0,CHUNK_WIDTH, CHUNK_HEIGHT,CHUNK_WIDTH, CHUNK_HEIGHT,heightMap, 20.0f);
    originalMesh= new Mesh(data);
    delete data;
  }

  MCPointData data[(CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1) * 20];
  for(int z = 0;z<CHUNK_HEIGHT+1;z++)
  {
    for(int y = 0;y<20;y++)
    {
      for(int x = 0;x<CHUNK_WIDTH+1;x++)
      {
        int index = x + y * (CHUNK_WIDTH+1) + z * (CHUNK_WIDTH+1) * 20;
        data[index].inhabited = heightMap[x + z * (CHUNK_WIDTH+1)] * 20 > y && !(caves[index] < 0.45 || caves[index] > 0.55);
      }
    }
  }
  mesh = new MarchingCubes(data, CHUNK_WIDTH+1, 20, CHUNK_HEIGHT+1);
}

Chunk::~Chunk()
{
  delete mesh;
}

#if 0
float Chunk::GetHeight(const Vec3<float>& chunkPosition)
{
  int x = floorf(chunkPosition.x);
  int z = floorf(chunkPosition.z);

  // Decimal part
  float xDec = chunkPosition.x - x;
  float zDec = chunkPosition.z - z;
  assert(!(x < 0 || z < 0 || x >= CHUNK_WIDTH || z >= CHUNK_HEIGHT) && "Chunk::GetHeight out of bounds");
  Vec3<float> normal;
  float d;
  if(zDec > xDec)
  {
    float h1 = heightMap[x + z * (CHUNK_WIDTH+1)]; // (0,h1,0)
    float h2 = heightMap[x + (z+1) * (CHUNK_WIDTH+1)]; // (0,h2,1)
    float h3 = heightMap[x+1 + (z+1) * (CHUNK_WIDTH+1)]; // (1,h3,1)
    float v1 = h2 - h1;
    float v2 = h3 - h1;

    // Cross product without x, z coordinates
    normal = {v1 - v2, 1, -v1};
    // normal dot (0, h1, 0)
    d = h1;
  }
  else
  {
    float h1 = heightMap[x + z * (CHUNK_WIDTH+1)]; // (0,h1,0)
    float h2 = heightMap[x+1 + (z+1) * (CHUNK_WIDTH+1)]; // (1,h2,1)
    float h3 = heightMap[x+1 + z * (CHUNK_WIDTH+1)]; // (1,h3,0)
    float v1 = h2 - h1;
    float v2 = h3 - h1;
    normal = {-v2, 1, v2 - v1};
    d = h1;
  }
  return d - normal.x * xDec - normal.z * zDec;
}
#endif

IntersectionData Chunk::RayCastChunk(const TPCamera& camera)
{
  Mat4 screenToModel = ~(camera.GetProjectionMatrix() * camera.GetViewMatrix() * GetTransformationMatrix());

  Vec4 nearRes = screenToModel * Vec3<float>(0.0f, 0.0f, -1.0);
  Vec4 farRes = screenToModel * Vec3<float>(0.0f, 0.0f, 1.0);

  // Normalize the w
  Vec3<float> near = Vec3<float>(nearRes) / nearRes.w;
  Vec3<float> far = Vec3<float>(farRes) / farRes.w;

  if(RayCast::CheckCube(near,far, {0,0,0}, {CHUNK_WIDTH+1, 20, CHUNK_HEIGHT+1}))
  {
    static int i = 0;
    i++;
    IntersectionData data = mesh->GetHalfEdgeMesh()->RayCast(near, far);
    data.v1 = Vec3<float>(GetTransformationMatrix() * data.v1);
    data.v2 = Vec3<float>(GetTransformationMatrix() * data.v2);
    data.v3 = Vec3<float>(GetTransformationMatrix() * data.v3);
    return data;
  }
  return IntersectionData();
}
