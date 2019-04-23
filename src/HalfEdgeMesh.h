#pragma once

#include <internal/GreetTypes.h>
#include <math/Vec3.h>
#include <graphics/models/MeshData.h>
#include <map>

class HalfEdgeMesh
{
  static uint UNINITIALIZED;
  struct Vertex
  {
    Vertex(uint vert):vert{vert}{}
    uint vert;
    Greet::Vec3<float> normal;

    uint edge = UNINITIALIZED;
  };

  struct HalfEdge
  {
    uint pair = UNINITIALIZED; 
    uint prev = UNINITIALIZED; 
    uint next = UNINITIALIZED; 
    uint vert = UNINITIALIZED;
    uint face = UNINITIALIZED;

    HalfEdge(uint pair, uint vert)
      : pair{pair}, vert{vert}
    {}
  };

  struct Face
  {
    uint halfEdge;
    Greet::Vec3<float> normal;
  };

  struct OrderedPair
  {
    uint first,second;
    OrderedPair(uint first, uint second) 
      : first{first < second ? first : second}, 
      second{first < second ? second : first} 
    {}

    bool operator<(const OrderedPair& rhs) const
    {
      if(first < rhs.first)
        return true;
      else if(first == rhs.first)
        return second < rhs.second;
      return false;
    }
  };

  public:
    std::vector<Vertex> vertices;
    std::vector<HalfEdge> edges;
    std::vector<Face> faces;

    std::map<OrderedPair, uint> edgePairs;
    std::map<Greet::Vec3<float>, uint> uniqueVertices;

    // Used for drawing the mesh
    std::vector<Greet::Vec3<float>> vertexArray;
    std::vector<uint> indices;

  public:

    HalfEdgeMesh();
    HalfEdgeMesh(Greet::MeshData* data);

    void AddFace(const Greet::Vec3<Greet::Vec3<float>>& vertices);
    void AddVertex(const Greet::Vec3<float>& v, uint& index);
    void AddEdge(uint v1, uint v2, uint& e1, uint& e2);

    void CalculateNormals();

    std::vector<size_t> FindNeightbouringFaces(size_t vertex);
};
