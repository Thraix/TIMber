#include "LineRenderer.h"

using namespace Greet;

LineRenderer* LineRenderer::renderer = nullptr;

LineRenderer::LineRenderer()
  : shader(Shader::FromFile("res/shaders/line.shader")), vao(), vbo(2 * sizeof(Vec3<float>), BufferType::ARRAY, BufferDrawType::DYNAMIC), ibo(2 * sizeof(uint), BufferType::INDEX, BufferDrawType::STATIC)
{
  vao.Enable();
  vbo.Enable();
  vbo.UpdateData(nullptr);
  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3<float>), 0));
  vbo.Disable();
  vao.Disable();
  uint indices[2] = {0, 1};
  ibo.Enable();
  ibo.UpdateData(indices);
  ibo.Disable();
}

void LineRenderer::DrawLine(const Camera& camera, const Vec3<float>& p1, const Vec3<float>& p2, const Vec4& color) const
{
  shader.Enable();
  shader.SetUniformMat4("projectionMatrix", camera.GetProjectionMatrix());
  shader.SetUniformMat4("viewMatrix", camera.GetViewMatrix());
  shader.SetUniform4f("lineColor", color);

  vbo.Enable();
  Vec3<float>* buffer = (Vec3<float>*)vbo.MapBuffer();
  buffer[0] = p1;
  buffer[1] = p2;
  vbo.UnmapBuffer();
  vbo.Disable();

  vao.Enable();
  ibo.Enable();
  GLCall(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr));
  ibo.Disable();
  vao.Disable();

  shader.Disable();
}
