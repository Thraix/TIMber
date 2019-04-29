#include "MCMesh.h"

#include <algorithm>

#include "MCClassification.h"

using namespace Greet;

MCMesh::MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length)
  : vao{}, 
  vbo{0, BufferType::ARRAY, BufferDrawType::DYNAMIC}, 
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
  vbo.Enable();

  vbo.UpdateData(vertices.data(), vertices.size() * sizeof(Vec3<float>));
  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));

  vbo.Disable();

  ibo.Enable();
  ibo.UpdateData(faces.data(),faces.size() * 3 * sizeof(uint));
  ibo.Disable();
  vao.Disable();
}

void MCMesh::UpdateData(const std::vector<MCPointData>& data, int xOffset, int yOffset, int zOffset, uint w, uint h, uint l)
{
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
  memcpy(voxelData.data(), data.data(), voxelData.size() * sizeof(voxelData[0]));

  UpdateRenderData();
}

void MCMesh::Bind()
{
  vao.Enable();
  vbo.Enable();
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
  vbo.Disable();
  vao.Disable();
}

void MCMesh::UpdateRenderData()
{
  vbo.Enable();
  if(vbo.GetDataSize() < vertices.size()*sizeof(Vec3<float>))
  {
    vbo.UpdateData(vertices.data(), vertices.size() * sizeof(Vec3<float>));
  }
  else
  {
    Vec3<float>* verts = (Vec3<float>*)vbo.MapBuffer();
    memcpy(verts, vertices.data(), vertices.size() * sizeof(Vec3<float>));
    vbo.UnmapBuffer();
  }
  vbo.Disable();

  ibo.Enable();
  if(ibo.GetDataSize() < faces.size() * 3 * sizeof(uint))
  {
    ibo.UpdateData(faces.data(), faces.size() * 3 * sizeof(uint));
  }
  else
  {
    uint* indices = (uint*)ibo.MapBuffer();
    memcpy(indices, faces.data(), faces.size() * 3 * sizeof(uint));
    ibo.UnmapBuffer();
  }
  ibo.Disable();
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
    vertices.push_back(vertex);
  uniqueVertices.emplace(vertex, std::make_pair(pos, 1));
  return pos;
}

