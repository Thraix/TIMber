#include "LineRenderer.h"

#include <internal/GreetGL.h>

using namespace Greet;

LineRenderer* LineRenderer::renderer = nullptr;

LineRenderer::LineRenderer()
  : shader(Shader::FromFile("res/shaders/line.shader"))
{
  vao = VertexArray::Create();
  vbo = VertexBuffer::CreateDynamic(nullptr, 2 * sizeof(Vec3<float>));
  vbo->SetStructure({{0, BufferAttributeType::VEC3}});
  vao->AddVertexBuffer(vbo);
  vbo->Disable();
  uint indices[2] = {0, 1};
  ibo = Buffer::Create(2 * sizeof(uint), BufferType::INDEX, BufferDrawType::STATIC);
  ibo->Enable();
  ibo->UpdateData(indices);
  ibo->Disable();
  vao->SetIndexBuffer(ibo);
  vao->Disable();
}

void LineRenderer::DrawLine(const Ref<Camera3D>& camera, const Vec3<float>& p1, const Vec3<float>& p2, const Color& color) const
{
  shader->Enable();
  camera->SetShaderUniforms(shader);
  shader->SetUniformColor4("lineColor", color);
  GLCall(glLineWidth(5.0f));

  vbo->Enable();
  Vec3<float>* buffer = (Vec3<float>*)vbo->MapBuffer();
  buffer[0] = p1;
  buffer[1] = p2;
  vbo->UnmapBuffer();
  vbo->Disable();

  vao->Enable();
  GLCall(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr));
  vao->Disable();

  shader->Disable();
}
