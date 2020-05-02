#include "MCMesh.h"

#include <algorithm>

#include <internal/GreetGL.h>

#include "MCClassification.h"

using namespace Greet;

MCMesh::MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length)
  : voxelData{data}, width{width}, height{height}, length{length}
{
  vao = VertexArray::Create();
  vbo_position = VertexBuffer::CreateDynamic(nullptr, 0);
  vbo_color = VertexBuffer::CreateDynamic(nullptr, 0);
  ibo = Buffer::Create(0, BufferType::INDEX, BufferDrawType::DYNAMIC);
  // Go through all the cubes
  Vec3<size_t> voxel = {0, 0, 0};
  for(uint z = 0;z<length-1;z++)
  {
    voxel.z = z;
    for(uint y = 0;y<height-1;y++)
    {
      voxel.y = y;
      for(uint x = 0;x<width-1;x++)
      {
        voxel.x = x;
        std::vector<Greet::Vec3<size_t>> faces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);
        std::vector<std::pair<size_t, Vec3<size_t>>> listFaces;
        for(auto&& face : faces)
        {
          size_t v1 = AddVertex(face[0], voxel);
          size_t v2 = AddVertex(face[1], voxel);
          size_t v3 = AddVertex(face[2], voxel);
          uint pos = AddFace({v1, v2, v3}, voxel);
          listFaces.push_back(std::make_pair(pos, face));
        }
        voxelFaces.emplace(x + (y + z * (height - 1)) * (width - 1), listFaces);
      }
    }
  }
  vao->Enable();
  vbo_position->Enable();
  vbo_position->UpdateData(vertices.data(), vertices.size() * sizeof(Vec3<float>));
  vbo_position->SetStructure({{0, BufferAttributeType::VEC3}});
  vao->AddVertexBuffer(vbo_position);
  vbo_position->Disable();

  vbo_color->Enable();
  vbo_color->UpdateData(colors.data(), colors.size() * sizeof(Vec4));
  vbo_color->SetStructure({{1, BufferAttributeType::VEC4}});
  vao->AddVertexBuffer(vbo_color);
  vbo_color->Disable();

  ibo->Enable();
  ibo->UpdateData(faces.data(),faces.size() * 3 * sizeof(uint));
  ibo->Disable();
  vao->Disable();
}

void MCMesh::UpdateData(const std::vector<MCPointData>& data, int xOffset, int yOffset, int zOffset, uint w, uint h, uint l)
{
  memcpy(voxelData.data(), data.data(), voxelData.size() * sizeof(voxelData[0]));
  // Go through all the cubes
  uint minX = std::max(xOffset-1, 0);
  uint minY = std::max(yOffset-1, 0);
  uint minZ = std::max(zOffset-1, 0);
  uint maxX = std::min(xOffset+w+1, width-1);
  uint maxY = std::min(yOffset+h+1, height-1);
  uint maxZ = std::min(zOffset+l+1, length-1);
  Vec3<size_t> voxel = {0, 0, 0};
  for(uint z = minZ;z<maxZ;z++)
  {
    voxel.z = z;
    for(uint y = minY;y<maxY;y++)
    {
      voxel.y = y;
      for(uint x = minX;x<maxX;x++)
      {
        voxel.x = x;
        std::vector<Greet::Vec3<size_t>> newFaces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);

        uint index = x + (y + z * (height - 1)) * (width - 1);
        uint indexVoxel = x + (y + z * height) * width;
        auto&& it = voxelFaces.find(index);
        std::vector<std::pair<size_t, Vec3<size_t>>> listFaces;
        if(it != voxelFaces.end())
        {
          listFaces = it->second;
        }
        ushort oldEdges = GetVoxelEdges(listFaces);
        ushort newEdges = GetVoxelEdges(newFaces);

        std::vector<size_t> edgeVertex(12);
        // Go through all the edges (a cube has 12 edges)
        for(int i = 0;i<12;i++)
        {
          // If we had an edge but it is now gone
          if((oldEdges & 0x1) && !(newEdges & 0x1))
          {
            RemoveVertex(i, voxel);
          }
          // If we didn't have an edge but now we do
          else if(!(oldEdges & 0x1) && (newEdges & 0x1))
          {
            edgeVertex[i] = AddVertex(i, voxel);
          }
          else if(oldEdges & 0x1)
          {
            edgeVertex[i] = UpdateVertex(i, voxel);
          }

          oldEdges >>= 1;
          newEdges >>= 1;
        }

        for(int i = 0;i < newFaces.size();i++)
        {
          if(i < listFaces.size())
          {
            faces[listFaces[i].first].v1 = edgeVertex[newFaces[i][0]];
            faces[listFaces[i].first].v2 = edgeVertex[newFaces[i][1]];
            faces[listFaces[i].first].v3 = edgeVertex[newFaces[i][2]];
            listFaces[i].second = newFaces[i];
          }
          else
          {
            uint pos = AddFace({edgeVertex[newFaces[i][0]],edgeVertex[newFaces[i][1]],edgeVertex[newFaces[i][2]]}, voxel);
            listFaces.push_back(std::make_pair(pos, newFaces[i]));
          }
        }
        while(listFaces.size() > newFaces.size())
        {
          RemoveFace(listFaces[listFaces.size() - 1].first);
          listFaces.pop_back();
        }
        if(it != voxelFaces.end())
          it->second = listFaces;
        else
          voxelFaces.emplace(index, listFaces);
      }
    }
  }

  UpdateRenderData();
}

void MCMesh::Bind() const
{
  vao->Enable();
  vbo_position->Enable();
  ibo->Enable();
  GLCall(glEnable(GL_CULL_FACE));
  GLCall(glFrontFace(GL_CCW));
}
void MCMesh::Render() const
{
  GLCall(glDrawElements(GL_TRIANGLES, faces.size() * 3 * sizeof(uint), GL_UNSIGNED_INT,0));
}
void MCMesh::Unbind() const
{
  ibo->Disable();
  vbo_position->Disable();
  vao->Disable();
}

void MCMesh::UpdateRenderData()
{
  UpdateBuffer(vbo_position, vertices.data(), vertices.size() * sizeof(Vec3<float>));
  UpdateBuffer(vbo_color, colors.data(), colors.size() * sizeof(Vec4));
  UpdateBuffer(ibo, faces.data(), faces.size() * 3 * sizeof(uint));
}

void MCMesh::UpdateBuffer(const Ref<Buffer>& buffer, void* data, size_t size)
{
  if(size == 0 && buffer->GetDataSize() == 0)
    return;
  buffer->Enable();
  if(buffer->GetDataSize() < size)
  {
    buffer->UpdateData(data, size);
  }
  else
  {
    void* mapping = buffer->MapBuffer();
    memcpy(mapping, data, size);
    buffer->UnmapBuffer();
  }
  buffer->Disable();
}

uint MCMesh::AddFace(const Vec3<size_t>& vertices, const Greet::Vec3<size_t>& voxel)
{
  uint pos = faces.size();
  Face face(vertices[0],vertices[1],vertices[2]);
  if(!fragmentFaces.empty())
  {
    Fragmentation& fragment = fragmentFaces.front();
    pos = fragment.position;
    faces[pos] = face;
    fragment.position++;
    fragment.size--;
    if(fragment.size == 0)
      fragmentFaces.pop();
  }
  else
    faces.push_back(face);
  return pos;
}

const Vec4& MCMesh::GetColor(size_t edge, const Vec3<size_t>& voxel)
{
  Vec3<float> vMin = MCClassification::vertices[MCClassification::edges[edge].first];
  Vec3<float> vMax = MCClassification::vertices[MCClassification::edges[edge].second];
  uint indexMin = (vMin.x + voxel.x) + ((vMin.y + voxel.y) + (vMin.z + voxel.z) * height) * width;
  if(voxelData[indexMin].Inhabited())
  {
    return voxelData[indexMin].voxel->color;
  }
  else
  {
    uint indexMax = (vMax.x + voxel.x) + ((vMax.y + voxel.y) + (vMax.z + voxel.z) * height) * width;
    return voxelData[indexMax].voxel->color;
  }
}

void MCMesh::RemoveFace(uint face)
{
  fragmentFaces.push(Fragmentation{face, 1});

  // Invalidate all the vertices 
  faces[face].v1 = 0;
  faces[face].v2 = 0;
  faces[face].v3 = 0;
}

uint MCMesh::AddVertex(size_t edge, const Greet::Vec3<size_t>& voxel)
{
  uint edgeIndex = GetEdgeIndex(edge, voxel);
  auto&& it = uniqueVertices.find(edgeIndex);

  if(it != uniqueVertices.end())
  {
    colors[it->second] = GetColor(edge, voxel);
    return it->second;
  }

  uint pos = vertices.size();
  if(!fragmentVertices.empty())
  {
    Fragmentation& fragment = fragmentVertices.front();
    pos = fragment.position;
    vertices[pos] = GetVertex(edge, voxel);
    colors[pos] = GetColor(edge, voxel);
    fragment.position++;
    fragment.size--;
    if(fragment.size == 0)
      fragmentVertices.pop();
  }
  else
  {
    vertices.push_back(GetVertex(edge, voxel));
    colors.push_back(GetColor(edge, voxel));
  }
  uniqueVertices.emplace(edgeIndex, pos);
  return pos;
}

void MCMesh::RemoveVertex(size_t edge, const Vec3<size_t>& voxel)
{
  auto&& it = uniqueVertices.find(GetEdgeIndex(edge, voxel));
  if(it != uniqueVertices.end())
  {
    size_t index = it->second;
    fragmentVertices.push({index, 1});
    uniqueVertices.erase(it);
  }
}

uint MCMesh::UpdateVertex(size_t edge, const Vec3<size_t>& voxel)
{
  auto&& it = uniqueVertices.find(GetEdgeIndex(edge, voxel));
  if(it != uniqueVertices.end())
  {
    size_t index = it->second;
    vertices[index] = GetVertex(edge, voxel);
    colors[index] = GetColor(edge, voxel);
    return index;
  }
  Log::Error("Tried updating non existing vertex!");
  return 0;
}

uint MCMesh::GetEdgeIndex(size_t mcEdgeIndex, const Vec3<size_t>& voxel)
{
  const Vec3<float>& v1 = MCClassification::vertices[MCClassification::edges[mcEdgeIndex].first];
  const Vec3<float>& v2 = MCClassification::vertices[MCClassification::edges[mcEdgeIndex].second];
  if(v1.x != v2.x)
  {
    // Edge along voxel.x avoxel.xis
    int edgeX = voxel.x;
    int edgeY = voxel.y + (int)v1.y;
    int edgeZ = voxel.z + (int)v1.z;
    return edgeX + (edgeY + edgeZ * height) * width;
  }
  else if(v1.y != v2.y)
  {
    // Edge along voxel.y avoxel.xis
    int edgeX = voxel.x + (int)v1.x;
    int edgeY = voxel.y;
    int edgeZ = voxel.z + (int)v1.z;
    return edgeX + (edgeY + edgeZ * height) * width + width * height * length;
  }
  else //if(v1.voxel.z != v2.voxel.z)
  {
    // Edge along voxel.z avoxel.xis
    int edgeX = voxel.x + (int)v1.x;
    int edgeY = voxel.y + (int)v1.y;
    int edgeZ = voxel.z;
    return edgeX + (edgeY + edgeZ * height) * width + 2 * width * height * length;
  }
}

ushort MCMesh::GetVoxelEdges(std::vector<Vec3<size_t>> faces)
{
  ushort edges = 0;
  for(auto&& face : faces)
  {
    edges |= (1 << face[0]);
    edges |= (1 << face[1]);
    edges |= (1 << face[2]);
  }
  return edges;
}

ushort MCMesh::GetVoxelEdges(std::vector<std::pair<size_t, Vec3<size_t>>> faces)
{
  ushort edges = 0;
  for(auto&& face : faces)
  {
    edges |= (1 << face.second[0]);
    edges |= (1 << face.second[1]);
    edges |= (1 << face.second[2]);
  }
  return edges;
}

Vec3<float> MCMesh::GetVertex(size_t mcEdgeIndex, const Vec3<size_t>& voxel)
{
  return MCClassification::GetEdgeMidPoint(mcEdgeIndex, voxelData, voxel.x, voxel.y, voxel.z, width, height, length) + Vec3<float>(voxel.x, voxel.y, voxel.z);
}

