#pragma once

#include "MCPointData.h"

#include <common/Types.h>
#include <graphics/buffers/Buffer.h>
#include <graphics/buffers/VertexArray.h>
#include <math/Vec3.h>
#include <vector>
#include <map>
#include <queue>

struct Fragmentation
{
  size_t position;
  size_t size;
  Fragmentation(size_t position, size_t size)
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
    Greet::Ref<Greet::Buffer> vbo_position;
    Greet::Ref<Greet::Buffer> vbo_color;
    Greet::Ref<Greet::VertexArray> vao;
    Greet::Ref<Greet::Buffer> ibo;

    // Voxel data
    std::vector<MCPointData> voxelData;
    uint width, height, length;

    // Render data
    std::vector<Greet::Vec3<float>> vertices;
    std::vector<Greet::Vec4> colors;
    std::vector<Face> faces; 

    // Keep track of unique vertices with their correspondig vertex index
    std::map<uint, uint> uniqueVertices;
    std::map<uint, std::vector<std::pair<size_t, Greet::Vec3<size_t>>>> voxelFaces;

    std::queue<Fragmentation> fragmentVertices;
    std::queue<Fragmentation> fragmentFaces;
  public:
    MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length); 

    void UpdateRenderData();
    void UpdateData(const std::vector<MCPointData>& data, int xOffset, int yOffset, int zOffset, uint w, uint h, uint l);
    void UpdateBuffer(const Greet::Ref<Greet::Buffer>& buffer, void* data, size_t size);

    void Bind() const;
    void Render() const;
    void Unbind() const;

    const std::vector<Face>& GetFaces() const { return faces; } 
    const std::vector<Greet::Vec3<float>>& GetVertices() const { return vertices; } 
  private:
    ushort GetVoxelEdges(std::vector<Greet::Vec3<size_t>> faces);
    ushort GetVoxelEdges(std::vector<std::pair<size_t, Greet::Vec3<size_t>>> faces);
    const Greet::Vec4& GetColor(size_t edge, const Greet::Vec3<size_t>& voxel);
    uint AddFace(const Greet::Vec3<size_t>& edges, const Greet::Vec3<size_t>& voxel);
    void RemoveFace(uint face);
    uint AddVertex(size_t edge, const Greet::Vec3<size_t>& voxel);
    uint UpdateVertex(size_t edge, const Greet::Vec3<size_t>& voxel);
    void RemoveVertex(size_t edge, const Greet::Vec3<size_t>& voxel);

    uint GetEdgeIndex(size_t mcEdgeIndex, const Greet::Vec3<size_t>& voxel);
    Greet::Vec3<float> GetVertex(size_t mcEdgeIndex, const Greet::Vec3<size_t>& voxel);
};

