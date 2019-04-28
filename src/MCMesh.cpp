#include "MCMesh.h"

#include "MCClassification.h"

using namespace Greet;

MCMesh::MCMesh(const std::vector<MCPointData>& data, uint width, uint height, uint length)
  : vao{}, 
  vbo{0, BufferType::ARRAY, BufferDrawType::DYNAMIC}, 
  ibo{0, BufferType::INDEX, BufferDrawType::DYNAMIC}
{
  // Go through all the cubes
  for(uint z = 0;z<length-1;z++)
  {
    for(uint y = 0;y<height-1;y++)
    {
      for(uint x = 0;x<width-1;x++)
      {
        std::vector<Greet::Vec3<Greet::Vec3<float>>> faces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);
        for(auto&& face : faces)
        {
          AddFace(face);
        }
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
    vbo.UpdateData(vertices.data(), vertices.size());
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
    ibo.UpdateData(faces.data(), faces.size());
  }
  else
  {
    uint* indices = (uint*)ibo.MapBuffer();
    memcpy(indices, faces.data(), faces.size() * 3 * sizeof(uint));
    ibo.UnmapBuffer();
  }
  ibo.Disable();
}

void MCMesh::AddFace(const Vec3<Vec3<float>>& verts)
{
  uint v1 = AddVertex(verts[0]);
  uint v2 = AddVertex(verts[1]);
  uint v3 = AddVertex(verts[2]);

  Face face{v1,v2,v3};
  faces.push_back(face);
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
  vertices.push_back(vertex);
  uniqueVertices.emplace(vertex, std::make_pair(pos, 1));
  return pos;
}

