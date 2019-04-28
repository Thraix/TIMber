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
    // Location, vbo
    Greet::Buffer vbo;

    Greet::VertexArray vao;
    Greet::Buffer ibo;

    std::vector<Greet::Vec3<float>> vertices;
    std::vector<Face> faces; 
    // Keep track of unique vertices, with the pair <position, count>
    std::map<Greet::Vec3<float>, std::pair<uint, uint>> uniqueVertices;
    std::queue<Fragmentation> fragmentVertices;
  public:
    MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length); 

    void UpdateRenderData();

    void Bind();
    void Render();
    void Unbind();

    const std::vector<Face>& GetFaces() const { return faces; } 
    const std::vector<Greet::Vec3<float>>& GetVertices() const { return vertices; } 
  private:
    void AddFace(const Greet::Vec3<Greet::Vec3<float>>& vertices);
    uint AddVertex(const Greet::Vec3<float>& vertex);
};

