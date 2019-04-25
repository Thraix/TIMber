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

void HalfEdgeMesh::AddFace(const Vec3<Vec3<float>>& vertices)
{
  uint faceIndices[3];
  for(uint i = 0;i<3;i++)
  {
    AddVertex(vertices[i], faceIndices[i]);
  }

  uint edgeIndices[6];
  for(uint i = 0;i<3;i++)
  {
    AddEdge(faceIndices[i], faceIndices[(i+1)%3], edgeIndices[i*2], edgeIndices[i*2+1]);
  }

  for(uint i = 0;i<3;i++)
  {
    edges[edgeIndices[i*2]].next = edgeIndices[(i*2 + 2) % 6];
    edges[edgeIndices[i*2]].prev = edgeIndices[(i*2 + 4) % 6];
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

  HalfEdge edge1{e2, v1}, edge2{e1,v2};

  vertices[v1].edge = e1;
  vertices[v2].edge = e2;

  edges.push_back(edge1);
  edges.push_back(edge2);
  edgePairs.emplace(OrderedPair(v1,v2), e1);
}

void HalfEdgeMesh::CalculateNormals()
{
  for(auto&& vertex : vertices)
  {
    std::vector<size_t> nFaces = FindNeightbouringFaces(vertex.vert);
    Vec3<float> normal{};
    for(auto&& face : nFaces)
    {
      normal += faces[face].normal;
    }
    normal.Normalize();
    vertex.normal = normal;
  }
}

IntersectionData HalfEdgeMesh::RayCast(const Vec3<float>& near, const Vec3<float>& far)
{
  // This is a vector of pairs with faces and their intersection distance from the near point.
  IntersectionData intersection;
  for(auto&& face : faces)
  {
    Vec3<float> v1 = vertexArray[edges[face.halfEdge].vert];
    Vec3<float> v2 = vertexArray[edges[edges[face.halfEdge].next].vert];
    Vec3<float> v3 = vertexArray[edges[edges[edges[face.halfEdge].next].next].vert];
    if(NegativeVolume(near, v1, v2, v3) != NegativeVolume(far,v1,v2,v3))
    {
      bool s1 = NegativeVolume(near, far, v1, v2);
      bool s2 = NegativeVolume(near, far, v2, v3);
      bool s3 = NegativeVolume(near, far, v3, v1);
      if(s1 == s2 && s2 == s3)
      {
        // We have an intersection

        TriangleFace triangleFace = {v1,v2,v3};
        Vec3<float> intersectionPoint = LineIntersectPlane(near, far, triangleFace);
        float distanceFromNear = (intersectionPoint - near).Length();
        if(!intersection.hasIntersection || distanceFromNear < intersection.distanceFromNear)
        {
          intersection = {
            .hasIntersection = true,
            .v1 = v1,
            .v2 = v2,
            .v3 = v3,
            .intersectionPoint = intersectionPoint,
            .distanceFromNear = distanceFromNear
          };
        }
      }
    }
  }
  return intersection;
}

bool HalfEdgeMesh::NegativeVolume(const Vec3<float>& v1,const Vec3<float>& v2,const Vec3<float>& v3,const Vec3<float>& v4)
{
  return std::signbit(SignedVolume(v1,v2,v3,v4));
}

float HalfEdgeMesh::SignedVolume(const Vec3<float>& v1,const Vec3<float>& v2,const Vec3<float>& v3,const Vec3<float>& v4)
{
  return (v2-v1).Cross(v3 - v1).Dot(v4-v1) * (1.0f/6.0f);
}

Vec3<float> HalfEdgeMesh::LineIntersectPlane(const Vec3<float>& near, const Vec3<float>& far, const TriangleFace& plane)
{
  Vec3<float> normal = (plane[1] - plane[0]).Cross(plane[2] - plane[0]);
  float d = normal.Dot(plane[0]);
  float t = (d - normal.Dot(near)) / normal.Dot(far-near);
  return near + (far - near) * t;

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
