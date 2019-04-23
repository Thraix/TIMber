#include "HalfEdgeMesh.h"

#include <map>

using namespace Greet;

uint HalfEdgeMesh::UNINITIALIZED = -1;

HalfEdgeMesh::HalfEdgeMesh()
{}

HalfEdgeMesh::HalfEdgeMesh(MeshData* data)
{
  for(uint i = 0;i<data->GetIndexCount();i+=3)
  {
    AddFace({data->GetVertices()[data->GetIndices()[i]],data->GetVertices()[data->GetIndices()[i+1]],data->GetVertices()[data->GetIndices()[i+2]]});
  }
}

void HalfEdgeMesh::AddVertex(const Vec3<float>& v, uint& index)
{
  auto it = uniqueVertices.find(v);
  if(it != uniqueVertices.end())
  {
    index = it->second;
    indices.push_back(index);
    return;
  }

  index = uniqueVertices.size();
  uniqueVertices.emplace(v, index);
  vertexArray.push_back(v);
  vertices.push_back({(uint)index});
  indices.push_back(index);
}

void HalfEdgeMesh::AddFace(const Greet::Vec3<Greet::Vec3<float>>& vertices)
{
  uint faceIndices[3];
  for(uint i = 0;i<3;i++)
  {
    AddVertex(vertices[i], faceIndices[i]);
  }

  uint edgeIndices[6];
  for(uint j = 0;j<3;j++)
  {
    AddEdge(faceIndices[j], faceIndices[(j+1)%3], edgeIndices[j*2], edgeIndices[j*2+1]);
  }

  for(uint j = 0;j<3;j++)
  {
    edges[edgeIndices[j*2]].next = edgeIndices[(j*2+2) % 6];
    edges[edgeIndices[j*2+1]].prev = edgeIndices[((j+2)*2) % 6];
  }

  uint faceIndex{(uint)(faces.size())};

  Face face{edgeIndices[0]};
  face.normal = (vertices[1] - vertices[0]).Cross(vertices[2] - vertices[0]).Normalize();
  faces.push_back(face);
  edges[edgeIndices[0]].face = faceIndex;
  edges[edgeIndices[2]].face = faceIndex;
  edges[edgeIndices[4]].face = faceIndex;
}

void HalfEdgeMesh::AddEdge(uint v1, uint v2, uint& e1, uint& e2)
{
  auto it = edgePairs.find(OrderedPair(v1,v2));
  if(it != edgePairs.end())
  {
    if(v1 == edges[it->second].vert)
    {
      e1 = it->second;
      e2 = edges[it->second].pair;
    }
    else
    {
      e1 = edges[it->second].pair;
      e2 = it->second;
    }
    return;
  }
  e1 = edges.size();
  e2 = e1 + 1;

  HalfEdge edge1{e1, v1}, edge2{e2,v2};

  vertices[v1].edge = e1;
  vertices[v1].edge = e2;

  edges.push_back(edge1);
  edges.push_back(edge2);
  edgePairs.emplace(OrderedPair(v1,v2), e1);
}

void HalfEdgeMesh::CalculateNormals()
{
  for(auto&& vertex : vertices)
  {
    std::vector<size_t> nFaces = FindNeightbouringFaces(vertex.vert);
    Greet::Vec3<float> normal{};
    for(auto&& face : nFaces)
    {
      normal += faces[face].normal;
    }
    normal.Normalize();
    vertex.normal = normal;
  }
}

std::vector<size_t> HalfEdgeMesh::FindNeightbouringFaces(size_t vertex)
{
  std::vector<size_t> faces;

  HalfEdge currentHalfEdge = edges[edges[vertices[vertex].edge].prev];
  const HalfEdge& firstHalfEdge = currentHalfEdge;

  do
  {
    faces.push_back(currentHalfEdge.face);
    currentHalfEdge = edges[edges[currentHalfEdge.pair].next];
  }
  while(currentHalfEdge.vert != firstHalfEdge.vert);
  return faces;

}
