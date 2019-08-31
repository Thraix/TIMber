#include "TerrainCursor.h"

#include <graphics/models/MeshFactory.h>
#include <graphics/textures/TextureManager.h>

using namespace Greet;

TerrainCursor::TerrainCursor()
  : material{Greet::Shader::FromFile("res/shaders/cursor.shader")}//, TextureManager::Get2D("terrainCursor")}
{
  //MeshData data = MeshFactory::Quad(0,0,0,1,1);
  mesh = new Mesh(MeshFactory::Sphere({0.0f},1.0f, 20, 20));
}

TerrainCursor::~TerrainCursor()
{
  delete mesh;
}

void TerrainCursor::Render(const Camera& camera, const IntersectionData& intersection) const
{
  Mat4 screenToWorld = ~(camera.GetProjectionMatrix() * camera.GetViewMatrix());

  Vec3<float> nearRes = screenToWorld * Vec3<float>(0.0f, 0.0f, -1.0);
  Vec3<float> farRes = screenToWorld * Vec3<float>(0.0f, 0.0f, 1.0);

  material.Bind(&camera);
  Vec3<float> normal = (intersection.v2 - intersection.v1).Cross(intersection.v3 - intersection.v1).Normalize();
  Vec3<float> center = intersection.intersectionPoint;
  //material.GetShader().SetUniformMat4("transformationMatrix", Mat4::AlignAxis(center, near-far, {0.0f, 1.0f, 0.0f}) * Mat4::Scale({2.0f}));
  material.GetShader().SetUniformMat4("transformationMatrix", Mat4::Translate(center) * Mat4::Scale({0.5f}));
  material.GetShader().SetUniform4f("mat_color", {1.0f, 1.0f, 1.0f, 0.3f});
  material.GetShader().SetUniform1f("time", time);

  mesh->Bind();
  mesh->Render();
  mesh->Unbind();

  material.Unbind();

}

void TerrainCursor::Update(float timeElapsed)
{
  time += timeElapsed;
  if(time > M_PI)
    time -= M_PI;
}
