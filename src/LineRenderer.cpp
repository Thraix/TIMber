#include "LineRenderer.h"

#include <internal/GreetGL.h>

using namespace Greet;

LineRenderer* LineRenderer::renderer = nullptr;

LineRenderer::LineRenderer()
  : shader(Shader::FromFile("res/shaders/line.shader"))
{
  vao = VertexArray::Create();
  vbo = VertexBuffer::CreateDynamic(nullptr, 2 * sizeof(Vec3<float>));
  vbo->SetStructure({{0, BufferAttributeType::VEC4}});
  vbo->Disable();
  vao->Disable();
  uint indices[2] = {0, 1};
  ibo = Buffer::Create(2 * sizeof(uint), BufferType::INDEX, BufferDrawType::STATIC);
  ibo->Enable();
  ibo->UpdateData(indices);
  ibo->Disable();
}

void LineRenderer::DrawLine(const Camera& camera, const Vec3<float>& p1, const Vec3<float>& p2, const Vec4& color) const
{
  shader->Enable();
  shader->SetUniformMat4("projectionMatrix", camera.GetProjectionMatrix());
  shader->SetUniformMat4("viewMatrix", camera.GetViewMatrix());
  shader->SetUniform4f("lineColor", color);
  GLCall(glLineWidth(5.0f));

  vbo->Enable();
  Vec3<float>* buffer = (Vec3<float>*)vbo->MapBuffer();
  buffer[0] = p1;
  buffer[1] = p2;
  vbo->UnmapBuffer();
  vbo->Disable();

  vao->Enable();
  ibo->Enable();
  GLCall(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr));
  ibo->Disable();
  vao->Disable();

  shader->Disable();
}
