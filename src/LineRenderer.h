#pragma once

#include <graphics/renderers/Renderer3D.h>

class LineRenderer
{
  private:
    static LineRenderer* renderer;
    Greet::Ref<Greet::Shader> shader;
    Greet::Ref<Greet::VertexArray> vao;
    Greet::Ref<Greet::VertexBuffer> vbo;
    Greet::Ref<Greet::Buffer> ibo;

  private:
    LineRenderer();
  public:
    virtual ~LineRenderer(){}
    void DrawLine(const Greet::Ref<Greet::Camera3D>& camera, const Greet::Vec3<float>& p1, const Greet::Vec3<float>& p2, const Greet::Color& color) const;

    static const LineRenderer& GetInstance() { return *renderer; }
    static void CreateInstance() { renderer = new LineRenderer(); }
    static void DestroyInstance() { delete renderer; }
};
