#include "TerrainCursor.h"

#include <graphics/models/MeshFactory.h>
#include <graphics/textures/TextureManager.h>

using namespace Greet;

TerrainCursor::TerrainCursor()
  : material{Greet::Shader::FromFile("res/shaders/basic.shader"), TextureManager::Get2D("terrainCursor")}
{
  MeshData* data = MeshFactory::Quad(0,0,0,1,1);

  mesh = new Mesh(data);
  delete data;
}

TerrainCursor::~TerrainCursor()
{
  delete mesh;
}

void TerrainCursor::Render(const Camera& camera, const IntersectionData& intersection) const
{
  Mat4 screenToWorld = ~(camera.GetProjectionMatrix() * camera.GetViewMatrix());

  Vec4 nearRes = screenToWorld * Vec3<float>(0.0f, 0.0f, -1.0);
  Vec4 farRes = screenToWorld * Vec3<float>(0.0f, 0.0f, 1.0);

  // Normalize the w
  Vec3<float> near = Vec3<float>(nearRes) / nearRes.w;
  Vec3<float> far = Vec3<float>(farRes) / farRes.w;

  material.Bind(&camera);
  Vec3<float> normal = (intersection.v2 - intersection.v1).Cross(intersection.v3 - intersection.v1).Normalize();
  Vec3<float> center = intersection.intersectionPoint;
  material.GetShader().SetUniformMat4("transformationMatrix", Mat4::AlignAxis(center+normal*0.01f, normal, far - near));


  mesh->Bind();
  mesh->Render();
  mesh->Unbind();

  material.Unbind();

}
