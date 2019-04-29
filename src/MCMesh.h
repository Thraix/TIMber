#pragma once

#include "MCPointData.h"

#include <internal/GreetTypes.h>
#include <graphics/buffers/Buffer.h>
#include <graphics/buffers/VertexArray.h>
#include <math/Vec3.h>
#include <vector>
#include <map>
#include <queue>

struct Fragmentation
{
  uint position;
  uint size;
  Fragmentation(uint position, uint size)
    : position{position}, size{size}
  {}
};

struct Face
{
  uint v1;
  uint v2;
  uint v3;

  Face(uint v1,uint v2,uint v3)
    : v1{v1}, v2{v2}, v3{v3} 
  {}
};

class MCMesh 
{
  private:
    // Rendering data
    Greet::Buffer vbo;
    Greet::VertexArray vao;
    Greet::Buffer ibo;

    // Voxel data
    std::vector<MCPointData> voxelData;
    uint width, height, length;

    std::vector<Greet::Vec3<float>> vertices;
    std::vector<Face> faces; 
    // Keep track of unique vertices, with the pair <position, count>
    std::map<Greet::Vec3<float>, std::pair<uint, uint>> uniqueVertices;
    std::map<uint, std::vector<uint>> voxelFaces;

    std::queue<Fragmentation> fragmentVertices;
    std::queue<Fragmentation> fragmentFaces;
  public:
    MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length); 

    void UpdateRenderData();
    void UpdateData(const std::vector<MCPointData>& data, int xOffset, int yOffset, int zOffset, uint w, uint h, uint l);

    void Bind();
    void Render();
    void Unbind();

    const std::vector<Face>& GetFaces() const { return faces; } 
    const std::vector<Greet::Vec3<float>>& GetVertices() const { return vertices; } 
  private:
    uint AddFace(const Greet::Vec3<Greet::Vec3<float>>& vertices);
    void RemoveFace(uint face);
    uint AddVertex(const Greet::Vec3<float>& vertex);
};

