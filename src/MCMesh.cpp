#include "MCMesh.h"

#include <algorithm>

#include "MCClassification.h"

using namespace Greet;

MCMesh::MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length)
  : vao{}, 
  vbo_position{0, BufferType::ARRAY, BufferDrawType::DYNAMIC}, 
  vbo_color{0, BufferType::ARRAY, BufferDrawType::DYNAMIC}, 
  ibo{0, BufferType::INDEX, BufferDrawType::DYNAMIC},
  voxelData{data}, width{width}, height{height}, length{length}
{
  // Go through all the cubes
  for(uint z = 0;z<length-1;z++)
  {
    for(uint y = 0;y<height-1;y++)
    {
      for(uint x = 0;x<width-1;x++)
      {
        std::vector<Greet::Vec3<Greet::Vec3<float>>> faces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);
        std::vector<uint> listFaces;
        for(auto&& face : faces)
        {
          uint pos = AddFace(face);
          listFaces.push_back(pos);
        }
        voxelFaces.emplace(x + (y + z * (height - 1)) * (width - 1), listFaces);
      }
    }
  }
  vao.Enable();
  vbo_position.Enable();
  vbo_position.UpdateData(vertices.data(), vertices.size() * sizeof(Vec3<float>));
  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
  vbo_position.Disable();

  vbo_color.Enable();
  vbo_color.UpdateData(colors.data(), colors.size() * sizeof(Vec4));
  GLCall(glEnableVertexAttribArray(1));
  GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
  vbo_color.Disable();

  ibo.Enable();
  ibo.UpdateData(faces.data(),faces.size() * 3 * sizeof(uint));
  ibo.Disable();
  vao.Disable();
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
  for(uint z = minZ;z<maxZ;z++)
  {
    for(uint y = minY;y<maxY;y++)
    {
      for(uint x = minX;x<maxX;x++)
      {
        std::vector<Greet::Vec3<Greet::Vec3<float>>> newFaces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);

        uint index = x + (y + z * (height - 1)) * (width - 1);
        uint indexVoxel = x + (y + z * height) * width;
        auto&& it = voxelFaces.find(index);
        std::vector<uint> listFaces;
        if(it != voxelFaces.end())
        {
          listFaces = it->second;
        }

        for(int i = 0;i < newFaces.size();i++)
        {
          if(i < listFaces.size())
          {
            faces[listFaces[i]].v1 = AddVertex((newFaces[i])[0]);
            faces[listFaces[i]].v2 = AddVertex((newFaces[i])[1]);
            faces[listFaces[i]].v3 = AddVertex((newFaces[i])[2]);
          }
          else
          {
            uint pos = AddFace(newFaces[i]);
            listFaces.push_back(pos);
          }
        }
        while(listFaces.size() > newFaces.size())
        {
          RemoveFace(listFaces[listFaces.size() - 1]);
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

void MCMesh::Bind()
{
  vao.Enable();
  vbo_position.Enable();
  ibo.Enable();
  GLCall(glEnable(GL_CULL_FACE));
  GLCall(glFrontFace(GL_CCW));
}
void MCMesh::Render()
{
  GLCall(glDrawElements(GL_TRIANGLES, faces.size() * 3 * sizeof(uint), GL_UNSIGNED_INT,0));
}
void MCMesh::Unbind()
{
  ibo.Disable();
  vbo_position.Disable();
  vao.Disable();
}

void MCMesh::UpdateRenderData()
{
  UpdateBuffer(vbo_position, vertices.data(), vertices.size() * sizeof(Vec3<float>));
  UpdateBuffer(vbo_color, colors.data(), colors.size() * sizeof(Vec4));
  UpdateBuffer(ibo, faces.data(), faces.size() * 3 * sizeof(uint));
}

void MCMesh::UpdateBuffer(Buffer& buffer, void* data, size_t size)
{
  buffer.Enable();
  if(buffer.GetDataSize() < size)
  {
    buffer.UpdateData(data, size);
  }
  else
  {
    void* mapping = buffer.MapBuffer();
    memcpy(mapping, data, size);
    buffer.UnmapBuffer();
  }
  buffer.Disable();
}

uint MCMesh::AddFace(const Vec3<Vec3<float>>& verts)
{
  uint v1 = AddVertex(verts[0]);
  uint v2 = AddVertex(verts[1]);
  uint v3 = AddVertex(verts[2]);

  uint pos = faces.size();
  Face face{v1,v2,v3};
#if 0
  if(false && !fragmentFaces.empty())
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
#endif
    faces.push_back(face);
  return pos;
}

const Vec4& MCMesh::GetColor(const Vec3<float>& vertex)
{
  const Vec3<uint> vMin{floor(vertex.x), floor(vertex.y), floor(vertex.z)};
  const Vec3<uint> vMax{ceil(vertex.x), ceil(vertex.y), ceil(vertex.z)};
  uint indexMin = vMin.x + (vMin.y + vMin.z * height) * width;
  if(voxelData[indexMin].inhabited)
  {
    return voxelData[indexMin].voxel.color;
  }
  else
  {
    uint indexMax = vMax.x + (vMax.y + vMax.z * height) * width;
    return voxelData[indexMax].voxel.color;
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

uint MCMesh::AddVertex(const Vec3<float>& vertex) 
{
  auto&& it = uniqueVertices.find(vertex);

  if(it != uniqueVertices.end())
  {
    colors[it->second.first] = GetColor(vertex);
    it->second.second++;
    return it->second.first;
  }

  uint pos = vertices.size();
#if 0
  if(!fragmentVertices.empty())
  {
    Fragmentation& fragment = fragmentVertices.front();
    pos = fragment.position;
    vertices[pos] = vertex;
    fragment.position++;
    fragment.size--;
    if(fragment.size == 0)
      fragmentVertices.pop();
  }
  else
#endif
  {
    vertices.push_back(vertex);
    colors.push_back(GetColor(vertex));
  }
  uniqueVertices.emplace(vertex, std::make_pair(pos, 1));
  return pos;
}

