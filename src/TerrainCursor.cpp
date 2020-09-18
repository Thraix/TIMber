#include "TerrainCursor.h"

#include <graphics/models/MeshFactory.h>
#include <graphics/textures/TextureManager.h>

using namespace Greet;

TerrainCursor::TerrainCursor()
  : material{Greet::Shader::FromFile("res/shaders/cursor.shader")}//, TextureManager::Get2D("terrainCursor")}
{
  //MeshData data = MeshFactory::Quad(0,0,0,1,1);
  mesh = new Mesh(MeshFactory::Sphere(20, 20));
}

TerrainCursor::~TerrainCursor()
{
  delete mesh;
}

void TerrainCursor::Render(const Ref<Camera3D>& camera, const IntersectionData& intersection) const
{
  material.Bind();
  camera->SetShaderUniforms(material.GetShader());
  Vec3<float> normal = (intersection.v2 - intersection.v1).Cross(intersection.v3 - intersection.v1).Normalize();
  Vec3<float> center = intersection.intersectionPoint;
  //material.GetShader()->SetUniformMat4("transformationMatrix", Mat4::AlignAxis(center, near-far, {0.0f, 1.0f, 0.0f}) * Mat4::Scale({2.0f}));
  material.GetShader()->SetUniformMat4("uTransformationMatrix", Mat4::Translate(center) * Mat4::Scale({0.5f}));
  material.GetShader()->SetUniform4f("uMaterialColor", {1.0f, 1.0f, 1.0f, 0.3f});
  material.GetShader()->SetUniform1f("uTime", time);

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
