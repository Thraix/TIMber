#include "Chunk.h"

#include "RayCast.h"

#include <graphics/models/MeshFactory.h> 
#include <graphics/models/TPCamera.h> 

using namespace Greet;

uint Chunk::CHUNK_WIDTH = 32;
uint Chunk::CHUNK_HEIGHT = 32;
uint Chunk::CHUNK_LENGTH = 32;

Chunk::Chunk()
{}

void Chunk::Initialize(uint posX, uint posZ)
{
  this->posX = posX;
  this->posZ = posZ;
  heightMap = Noise::GenNoise(CHUNK_WIDTH+1, CHUNK_LENGTH+1,4,128, 128,0.75f, posX * CHUNK_WIDTH, posZ * CHUNK_LENGTH);
  biome = Noise::GenNoise(CHUNK_WIDTH+1, CHUNK_LENGTH+1,4,128, 128,0.5f, (posX+14) * CHUNK_WIDTH, (posZ+12) * CHUNK_LENGTH);
  std::vector<float> caves = Noise::GenNoise(CHUNK_WIDTH+1,CHUNK_HEIGHT+1, CHUNK_LENGTH+1,4,32,32, 32,0.75f, posX * CHUNK_WIDTH, 0, posZ * CHUNK_LENGTH);

  std::vector<float> minerals = Noise::GenNoise(CHUNK_WIDTH+1,CHUNK_HEIGHT+1, CHUNK_LENGTH+1,2,2,2,2,0.95f, posX * CHUNK_WIDTH, CHUNK_HEIGHT, posZ * CHUNK_LENGTH);

  {
    MeshData* data = MeshFactory::LowPolyGrid((CHUNK_WIDTH)/2.0,0,(CHUNK_LENGTH)/2.0,CHUNK_WIDTH, CHUNK_LENGTH,CHUNK_WIDTH, CHUNK_LENGTH,heightMap, CHUNK_HEIGHT+1);
    originalMesh= new Mesh(data);
    delete data;
  }

  voxelData = std::vector<MCPointData>((CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1) * (CHUNK_LENGTH+1));
  float min = 1.0f;
  float max = 0.0f;
  for(int z = 0;z<CHUNK_LENGTH+1;z++)
  {
    for(int y = 0;y<CHUNK_HEIGHT+1;y++)
    {
      for(int x = 0;x<CHUNK_WIDTH+1;x++)
      {
        int index = x + (y + z * (CHUNK_WIDTH+1)) * (CHUNK_HEIGHT+1);
        int indexHeight = x + z * (CHUNK_WIDTH+1);
        voxelData[index].inhabited = heightMap[indexHeight] * (CHUNK_HEIGHT+1) > y && (caves[index] < 0.48 || caves[index] > 0.52);
        voxelData[index].voxel = Voxel::stone;

        // Temporary
        if(minerals[index] < min) min = minerals[index];
        if(minerals[index] > max) max = minerals[index];
        voxelData[index].magnitude = (minerals[index] - 0.20f) / 0.80f;
        Math::Clamp(&(voxelData[index].magnitude), 0.01f, 0.99f);
        if(minerals[index] > 0.60)
          voxelData[index].voxel = Voxel::copper;
        if(minerals[index] < 0.40)
          voxelData[index].voxel = Voxel::tin;
        if(heightMap[indexHeight] * (CHUNK_HEIGHT+1) - 4 < y)
        {
          voxelData[index].voxel = Voxel::grass;
        }
      }
    }
  }
  Log::Info(min, ":", max);
  AddTree(4, heightMap[4 + 4 * (CHUNK_WIDTH+1)] * (CHUNK_HEIGHT+1),4);
  for(int z = 0;z<CHUNK_LENGTH+1;z++)
  {
    for(int x = 0;x<CHUNK_WIDTH+1;x++)
    { 
      int indexHeight = x + z * (CHUNK_WIDTH+1);
      if(biome[indexHeight] > 0.55)
        for(int y = CHUNK_HEIGHT;y>=0;y--)
        {
          int index = x + y * (CHUNK_WIDTH+1) + z * (CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1);
          if(voxelData[index].inhabited)
          {
            voxelData[index].voxel = Voxel::snow;
            break;
          }
        }
    }
  }
  mesh = new MCMesh(voxelData, CHUNK_WIDTH+1, CHUNK_HEIGHT+1, CHUNK_LENGTH+1);
}

Chunk::~Chunk()
{
  delete mesh;
}

void Chunk::AddTree(uint x, uint y, uint z)
{
  for(int i = y; i < y + 5; i++)
  {
    uint index = x + i * (CHUNK_WIDTH+1) + z * (CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1);
    voxelData[index].inhabited = true;
    voxelData[index].voxel = Voxel::wood;
    //voxelData[index].magnitude = 0.5f;
  }
  // Vim doesn't like this formatting...
  SphereOperation(Vec3<float>(x + posX * CHUNK_WIDTH,y + 5, z + posZ * CHUNK_LENGTH), 3, [&] (MCPointData& data, int x, int y, int z) 
      {
      if(!data.inhabited)
      {
      data.inhabited = true;
      data.voxel = Voxel::leaves;
      }
      } );

}

void Chunk::PlaceVoxels(const Vec3<float>& point, float radius)
{
  bool dirty = false;
  uint maxX = 0;
  uint maxY = 0;
  uint maxZ = 0;
  uint minX = CHUNK_WIDTH;
  uint minY = CHUNK_HEIGHT;
  uint minZ = CHUNK_LENGTH;
  // Vim doesn't like this formatting...
  SphereOperation(point, radius, [&] (MCPointData& data, int x, int y, int z) 
      {
      if(!data.inhabited)
      {
      data.inhabited = true;
      dirty = true;
      if(x < minX) minX = x;
      if(y < minY) minY = y;
      if(z < minZ) minZ = z;
      if(x > maxX) maxX = x;
      if(y > maxY) maxY = y;
      if(z > maxZ) maxZ = z;
      }
      });

  if(dirty)
  {
    mesh->UpdateData(voxelData, minX, minY, minZ, maxX - minX + 1, maxY - minY + 1, maxZ - minZ + 1); 
  }
}

void Chunk::RemoveVoxels(const Vec3<float>& point, float radius)
{
  bool dirty = false;
  uint maxX = 0;
  uint maxY = 0;
  uint maxZ = 0;
  uint minX = CHUNK_WIDTH;
  uint minY = CHUNK_HEIGHT;
  uint minZ = CHUNK_LENGTH;
  // Vim doesn't like this formatting...
  SphereOperation(point, radius, [&] (MCPointData& data, int x, int y, int z) 
      {
      if(data.inhabited)
      {
      data.inhabited = false;
      dirty = true;
      if(x < minX) minX = x;
      if(y < minY) minY = y;
      if(z < minZ) minZ = z;
      if(x > maxX) maxX = x;
      if(y > maxY) maxY = y;
      if(z > maxZ) maxZ = z;
      }
      });

  if(dirty)
  {
    mesh->UpdateData(voxelData, minX, minY, minZ, maxX - minX + 1, maxY - minY + 1, maxZ - minZ + 1); 
  }
}

void Chunk::SphereOperation(const Vec3<float>& point, float radius, std::function<void(MCPointData&, int, int,int)> func)
{
  int pX = (int)floor(point.x) - posX * CHUNK_WIDTH;
  int pY = (int)floor(point.y);
  int pZ = (int)floor(point.z) - posZ * CHUNK_LENGTH;
  Vec3<float> p = point - Vec3<float>{posX * CHUNK_WIDTH, 0, posZ * CHUNK_LENGTH};

  bool dirty = false;
  for(int z = pZ - radius;z < pZ + radius + 1;z++)
  {
    if(z < 0 || z > CHUNK_LENGTH)
      continue;
    for(int y = pY - radius;y < pY + radius + 1;y++)
    {
      if(y < 0 || y > CHUNK_HEIGHT)
        continue;
      for(int x = pX - radius;x < pX + radius + 1;x++)
      {
        if(x < 0 || x > CHUNK_WIDTH)
          continue;
        if((Vec3<float>{x,y,z} - p).Length() < radius)
        {
          func(voxelData[x + (y + z * (CHUNK_HEIGHT+1)) * (CHUNK_WIDTH+1)], x, y, z);
        }
      }
    }
  }
}

IntersectionData Chunk::RayCastChunk(const TPCamera& camera)
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

void Chunk::Update(float timeElapsed)
{
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
          if(voxelData[index].inhabited)
          {
            if(voxelData[index].voxel != Voxel::snow)
            {
              voxelData[index].voxel = Voxel::snow;
              mesh->UpdateData(voxelData, x,y,z, 1, 1, 1);
            }
            break;
          }

        }
      } 
    } 
  }
}
