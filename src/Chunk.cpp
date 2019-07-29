#include "Chunk.h"

#include "RayCast.h"

#include <graphics/models/MeshFactory.h> 
#include <graphics/models/TPCamera.h> 
#include <algorithm>

using namespace Greet;

uint Chunk::CHUNK_WIDTH = 32;
uint Chunk::CHUNK_HEIGHT = 32;
uint Chunk::CHUNK_LENGTH = 32;

Chunk::Chunk()
  : mesh{nullptr}
{}

void Chunk::Initialize(uint posX, uint posZ)
{
  this->posX = posX;
  this->posZ = posZ;
  heightMap = Noise::GenNoise(CHUNK_WIDTH+1, CHUNK_LENGTH+1,4,32, 32,0.75f, posX * CHUNK_WIDTH, posZ * CHUNK_LENGTH);
  biome = Noise::GenNoise(CHUNK_WIDTH+1, CHUNK_LENGTH+1,4,128, 128,0.5f, (posX+14) * CHUNK_WIDTH, (posZ+12) * CHUNK_LENGTH);
  std::vector<float> caves = Noise::GenNoise(CHUNK_WIDTH+1,CHUNK_HEIGHT+3, CHUNK_LENGTH+1,4,32,32, 32,0.75f, posX * CHUNK_WIDTH, 0, posZ * CHUNK_LENGTH);

  std::vector<float> minerals = Noise::GenNoise(CHUNK_WIDTH+1,CHUNK_HEIGHT+3, CHUNK_LENGTH+1,3,8,8,8,0.75f, posX * CHUNK_WIDTH, CHUNK_HEIGHT, posZ * CHUNK_LENGTH);

  {
    MeshData data = MeshFactory::LowPolyGrid((CHUNK_WIDTH)/2.0,0,(CHUNK_LENGTH)/2.0,CHUNK_WIDTH, CHUNK_LENGTH,CHUNK_WIDTH, CHUNK_LENGTH,heightMap, CHUNK_HEIGHT+1);
    originalMesh= new Mesh(data);
  }
  voxelData = std::vector<MCPointData>((CHUNK_WIDTH+1) * (CHUNK_HEIGHT+3) * (CHUNK_LENGTH+1));
#if 0
  voxelData[10 + (10 + 10 * (CHUNK_WIDTH+1)) * (CHUNK_HEIGHT+1)].inhabited = true;
  voxelData[11 + (10 + 10 * (CHUNK_WIDTH+1)) * (CHUNK_HEIGHT+1)].inhabited = true;
  mesh = new MCMesh(voxelData, CHUNK_WIDTH+1, CHUNK_HEIGHT+1, CHUNK_LENGTH+1);
  return;
#endif
  float min = 1.0f;
  float max = 0.0f;
  for(int z = 0;z<CHUNK_LENGTH+1;z++)
  {
    for(int y = 0;y<CHUNK_HEIGHT+1;y++)
    {
      for(int x = 0;x<CHUNK_WIDTH+1;x++)
      {
        int index = GetVoxelIndex(x,y,z);
        int indexHeight = x + z * (CHUNK_WIDTH+1);
        float height = heightMap[indexHeight] * (CHUNK_HEIGHT+1);
        MCPointData& data = GetVoxelData(x,y,z);

        data.magnitude = (y - height);
        // D = 1 - cave
        // cave = 1 -D
        Math::Clamp(&data.magnitude, -1.0f, 1.0f);
#if 0
        float p = 2;
        float cave = (caves[index] - 0.48f) * 50.0f;
        Math::Clamp(&cave, -1.0f, 1.0f);
        data.magnitude = 1.0f - powf(powf(1 - data.magnitude, -p) + powf((1+cave), -p), -1.0f/p);
#else
        data.magnitude = std::max(data.magnitude, -(caves[index] - 0.48f) * 30.0f);
#endif
        Math::Clamp(&data.magnitude, -1.0f, 1.0f);
        data.voxel = &Voxel::stone;

        // Temporary
        if(minerals[index] < min) min = minerals[index];
        if(minerals[index] > max) max = minerals[index];
        //data.magnitude = (minerals[index] - 0.20f) / 0.80f;
        if(minerals[index] > 0.60)
          data.voxel = &Voxel::royalium;
        if(minerals[index] < 0.40)
          data.voxel = &Voxel::malachite;
        if(heightMap[indexHeight] * (CHUNK_HEIGHT+1) - 4 < y)
        {
          data.voxel = &Voxel::grass;
        }
      }
    }
  }
  for(int i = 0;i<20;i++)
  {
    int x = (rand() % (CHUNK_WIDTH - 6)) + 3;
    int z = (rand() % (CHUNK_LENGTH- 6)) + 3;
    int y = heightMap[x + z * (CHUNK_WIDTH+1)] * (CHUNK_HEIGHT+1);
   // if(voxelData[GetVoxelIndex(x,y,z)].magnitude >= 0.0f)
   //   AddTree(x, y, z);
  }
  for(int z = 0;z<CHUNK_LENGTH+1;z++)
  {
    for(int x = 0;x<CHUNK_WIDTH+1;x++)
    { 
      int indexHeight = x + z * (CHUNK_WIDTH+1);
      if(biome[indexHeight] > 0.55)
        for(int y = CHUNK_HEIGHT;y>=0;y--)
        {
          int index = GetVoxelIndex(x,y,z);
          if(voxelData[index].Inhabited())
          {
            voxelData[index].voxel = &Voxel::snow;
            break;
          }
        }
    }
  }
  mesh = new MCMesh(voxelData, CHUNK_WIDTH+1, CHUNK_HEIGHT+3, CHUNK_LENGTH+1);
  chunkChange.dirty = false;
}

Chunk::~Chunk()
{
  delete mesh;
}

void Chunk::AddTree(uint x, uint y, uint z)
{
#if 0 
  for(int i = y; i < y + 5; i++)
  {
    uint index = GetVoxelIndex(x,i,z);
    voxelData[index].magnitude = 0.5f;
    voxelData[index].voxel = &Voxel::wood;
  }
  // Vim doesn't like this formatting...
  SphereOperation(Vec3<float>(x + posX * CHUNK_WIDTH,y + 6, z + posZ * CHUNK_LENGTH), 3, [&] (MCPointData& data, int voxelX, int voxelY, int voxelZ, float distanceSQ, bool inside) 
      {
      if(inside)
      {
      data.voxel = &Voxel::leaves;
      }
      data.magnitude = std::max(3 - sqrtf(distanceSQ), data.magnitude);
      Math::Clamp(&data.magnitude, -1.0f, 1.0f);
      });

#endif
}

IntersectionData Chunk::RayCastChunk(const Camera& camera)
{
  Mat4 screenToModel = ~(camera.GetProjectionMatrix() * camera.GetViewMatrix() * GetTransformationMatrix());

  Vec4 nearRes = screenToModel * Vec3<float>(0.0f, 0.0f, -1.0);
  Vec4 farRes = screenToModel * Vec3<float>(0.0f, 0.0f, 1.0);

  // Normalize the w
  Vec3<float> near = Vec3<float>(nearRes) / nearRes.w;
  Vec3<float> far = Vec3<float>(farRes) / farRes.w;

  if(RayCast::CheckCube(near,far, {0,0,0}, {CHUNK_WIDTH+1, CHUNK_HEIGHT+1, CHUNK_LENGTH+1}))
  {
    static int i = 0;
    i++;
    IntersectionData data = RayCast::MCMeshCast(near, far, *mesh);
    data.v1 = Vec3<float>(GetTransformationMatrix() * data.v1);
    data.v2 = Vec3<float>(GetTransformationMatrix() * data.v2);
    data.v3 = Vec3<float>(GetTransformationMatrix() * data.v3);
    data.intersectionPoint = Vec3<float>(GetTransformationMatrix() * data.intersectionPoint);
    return data;
  }
  return IntersectionData();
}

void Chunk::UpdateMesh()
{
  if(chunkChange.dirty && mesh)
  {
    mesh->UpdateData(voxelData, chunkChange.minX, chunkChange.minY, chunkChange.minZ, chunkChange.maxX - chunkChange.minX + 1,chunkChange.maxY - chunkChange.minY + 1, chunkChange.maxZ - chunkChange.minZ + 1);
    chunkChange.dirty = false;
  } 
}

void Chunk::UpdateVoxel(int x, int y, int z, const MCPointData& data)
{
  assert(IsInside(x,y,z) && "UpdateVoxel out of bounds");

  if(chunkChange.dirty)
  {
    if(x < chunkChange.minX) chunkChange.minX = x;
    if(y < chunkChange.minY) chunkChange.minY = y;
    if(z < chunkChange.minZ) chunkChange.minZ = z;
    if(x > chunkChange.maxX) chunkChange.maxX = x;
    if(y > chunkChange.maxY) chunkChange.maxY = y;
    if(z > chunkChange.maxZ) chunkChange.maxZ = z;
  }
  else
  {
    chunkChange.minX = x;
    chunkChange.minY = y;
    chunkChange.minZ = z;
    chunkChange.maxX = x;
    chunkChange.maxY = y;
    chunkChange.maxZ = z;
    chunkChange.dirty = true;
  }

  voxelData[GetVoxelIndex(x,y,z)] = data;
}

bool Chunk::IsInside(int x, int y, int z)
{
  return x >= 0 && x < CHUNK_WIDTH + 1 && y >= 0 && y < CHUNK_HEIGHT + 1 && z >= 0 && z < CHUNK_LENGTH+ 1;
}

int Chunk::GetVoxelIndex(int x, int y, int z)
{
  assert(IsInside(x,y,z) && "GetVoxelIndex out of bounds");
  return x + (y + 1 + z * (CHUNK_HEIGHT+3)) * (CHUNK_WIDTH+1);
}

MCPointData& Chunk::GetVoxelData(int x, int y, int z)
{
  assert(IsInside(x,y,z) && "GetVoxelData out of bounds");
  return voxelData[x + (y + 1 + z * (CHUNK_HEIGHT+3)) * (CHUNK_WIDTH+1)];
}

const MCPointData& Chunk::GetVoxelDataConst(int x, int y, int z) const
{
  return voxelData[x + (y + 1 + z * (CHUNK_HEIGHT+3)) * (CHUNK_WIDTH+1)];
}

void Chunk::Update(float timeElapsed)
{
  return;
  static float timer = 0;
  timer += timeElapsed;
  if(timer > 1 / 20.0f)
  {
    bool dirty;
    for(int i = 0;i<5;i++)
    {
      int x = rand() % (CHUNK_WIDTH+1);
      int z = rand() % (CHUNK_LENGTH+1);
      int indexHeight = x + z * (CHUNK_WIDTH+1);
      if(biome[indexHeight] > 0.55)
      {
        for(int y = CHUNK_HEIGHT;y>=0;y--)
        {
          int index = x + y * (CHUNK_WIDTH+1) + z * (CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1);
          if(voxelData[index].Inhabited())
          {
            if(voxelData[index].voxel != &Voxel::snow)
            {
              voxelData[index].voxel = &Voxel::snow;
              mesh->UpdateData(voxelData, x,y,z, 1, 1, 1);
            }
            break;
          }

        }
      } 
    } 
  }
}
