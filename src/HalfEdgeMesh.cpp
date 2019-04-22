#include "HalfEdgeMesh.h"

#include <map>

using namespace Greet;

uint HalfEdgeMesh::UNINITIALIZED = -1;

HalfEdgeMesh::HalfEdgeMesh(MeshData* data)
{
  std::map<uint, uint> indexToHalfEdge;
  uint indexCount = 0;

  for(uint i = 0;i<data->GetIndexCount();i+=3)
  {
    AddFace(data->GetVertices()[data->GetIndices()[i]],data->GetVertices()[data->GetIndices()[i+1]],data->GetVertices()[data->GetIndices()[i+2]]);
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

void HalfEdgeMesh::AddFace(const Vec3<float>& v1,const Vec3<float>& v2,const Vec3<float>& v3)
{
  uint faceIndices[3];
  AddVertex(v1, faceIndices[0]);
  AddVertex(v2, faceIndices[1]);
  AddVertex(v3, faceIndices[2]);

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
