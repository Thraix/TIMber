#pragma once

#include <graphics/renderers/Renderer3D.h>

class LineRenderer
{
  private:
    static LineRenderer* renderer;
    Greet::Ref<Greet::Shader> shader;
    Greet::VertexArray vao;
    Greet::Buffer vbo;
    Greet::Buffer ibo;

  private:
    LineRenderer();
  public:
    virtual ~LineRenderer(){}
    void DrawLine(const Greet::Camera& camera, const Greet::Vec3<float>& p1, const Greet::Vec3<float>& p2, const Greet::Vec4& color) const;

    static const LineRenderer& GetInstance() { return *renderer; }
    static void CreateInstance() { renderer = new LineRenderer(); }
    static void DestroyInstance() { delete renderer; }
};
