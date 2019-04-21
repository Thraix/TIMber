#include "Chunk.h"

#include <graphics/models/MeshFactory.h> 

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
  RecalcHeight(heightMap);
  MeshData* data = MeshFactory::LowPolyGrid((CHUNK_WIDTH)/2.0,0,(CHUNK_HEIGHT)/2.0,CHUNK_WIDTH, CHUNK_HEIGHT,CHUNK_WIDTH, CHUNK_HEIGHT,heightMap, 1.0f);
  RecalcGrid(data);
  mesh = new Mesh(data);
  delete data;
}

Chunk::~Chunk()
{
  delete mesh;
}

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

void Chunk::RecalcHeight(float* heightMap)
{
  for(int i = 0;i<(CHUNK_WIDTH+1) * (CHUNK_HEIGHT+1);i++)
  {
    if (heightMap[i] < 0.45)
    {
      //y = 0.45f + Noise::PRNG(vertex->x, vertex->z)*0.01f;// + 0.03f*(rand() / (float)RAND_MAX - 0.5f);
    }
    else if (heightMap[i] < 0.48)
    {

    }
    else if (heightMap[i] < 0.58)
    {

    }
    else if (heightMap[i] < 0.65)
    {
      //heightMap[i] = (pow(heightMap[i] - 0.58, 0.6) + 0.58);
    }
    else
    {
      //heightMap[i] = (pow(heightMap[i] - 0.58, 0.6) + 0.58);
    }
    heightMap[i] *= 20;
  }
}

void RecalcColors(const Vec3<float>& v1, const Vec3<float>& v2, const Vec3<float>& v3, uint* color)
{
  float y = (v1.y + v2.y + v3.y) / 3.0f / 20.0f;
  if (y < 0.45+0.01f)
  {
    uint blue = (uint)(pow(1, 4.0f) * 255);
    blue = blue > 255 ? 255 : blue;
    *color = 0xff000000 | ((blue / 2) << 16) | ((uint)(blue * 0.9) << 8) | blue;
    *color = 0xffffd399;
  }
  else if (y < 0.48)
  {
    *color = 0xffffd399;
  }
  else// if (y < 0.58)
  {
    *color = 0xff5cff64;
  }
  /*
     else if (y < pow(0.07, 0.6) + 0.58)
     {
   *color = 0xffB5B0A8;
   }
   else
   {
   *color = 0xffDCF2F2;
   }
   */
  *color = 0xff5cff64;
}

void Chunk::CalcGridVertexOffset(MeshData* data)
{
  Vec3<float>* vertices = data->GetVertices();
  uint vertexCount = data->GetVertexCount();
  uint indexCount = data->GetIndexCount();
  uint* indices = data->GetIndices();

  byte* offsets = new byte[4 * vertexCount];
  for (int i = 0;i < indexCount;i+=3)
  {
    Vec3<float> v1 = vertices[indices[i+1]] - vertices[indices[i]];
    Vec3<float> v2 = vertices[indices[i+2]] - vertices[indices[i]];
    offsets[indices[i]*4] = round(v1.x);
    offsets[indices[i]*4 + 1] = round(v1.z);
    offsets[indices[i]*4 + 2] = round(v2.x);
    offsets[indices[i]*4 + 3] = round(v2.z);
  }
  data->AddAttribute(new AttributeData<byte>(AttributeDefaults(4, 4, 4 * sizeof(byte), GL_BYTE,GL_FALSE), offsets));
}

void Chunk::RecalcGrid(MeshData* data)
{
  uint* colors = new uint[data->GetVertexCount()];
  Vec3<float>* vertices = data->GetVertices();
  uint indexCount = data->GetIndexCount();
  uint* indices = data->GetIndices();
  Vec3<float>* normals = (Vec3<float>*)data->GetAttribute(ATTRIBUTE_NORMAL)->data;
  for (int i = 0;i < indexCount;i += 3)
  {
    normals[indices[i]] = MeshFactory::CalculateNormal(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    RecalcColors(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]], &colors[indices[i]]);
  }
  data->AddAttribute(new AttributeData<uint>(ATTRIBUTE_COLOR, colors));
}
