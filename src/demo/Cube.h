#pragma once

#include <internal/GreetGL.h>
#include <input/InputDefines.h>
#include <graphics/fonts/FontManager.h>
#include <graphics/models/Mesh.h>
#include <graphics/models/Material.h>
#include <graphics/models/MeshFactory.h>
#include <event/MouseEvent.h>
#include <event/KeyEvent.h>
#include <graphics/renderers/BatchRenderer.h>

#include "../MCPointData.h"
#include "../MCMesh.h"
#include "../LineRenderer.h"

class Cube
{
  private:
    bool showUi = true;
    size_t size;
    Greet::Mesh* mesh;
    Greet::Material material;
    Greet::Ref<Greet::Shader> shader2d;
    std::vector<MCPointData> data;
    MCMesh mcMesh;
    std::vector<Greet::Vec3<float>> vertices;
    Greet::Ref<Greet::Camera3D> camera;
    Greet::BatchRenderer* renderer;
    std::vector<Greet::Renderable2D> renderables;


  public:
    Cube(const Greet::Ref<Greet::Camera3D>& camera, size_t size)
      : size{size}, material{Greet::Shader::FromFile("res/shaders/demo.shader")},
      shader2d{Greet::Shader::FromFile("res/shaders/2dshader.shader")},
      data{(size*size*size)},
      mcMesh(data, size, size, size), camera{camera}
    {
      mesh = new Greet::Mesh(Greet::MeshFactory::Plane());
      renderer = new Greet::BatchRenderer(shader2d);

      vertices = {
        {-0.5f,-0.5f,-0.5f},
        { 0.5f,-0.5f,-0.5f},
        {-0.5f, 0.5f,-0.5f},
        { 0.5f, 0.5f,-0.5f},
        {-0.5f,-0.5f, 0.5f},
        { 0.5f,-0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},
        { 0.5f, 0.5f, 0.5f}
      };
      for(int i = 0;i<8;i++)
      {
        renderables.push_back(Greet::Renderable2D{{-10.0f},{20.0f},0xff000000, Greet::Sprite(Greet::TextureManager::LoadTexture2D("res/textures/select.meta"))});
      }
    }

    virtual ~Cube()
    {
      delete mesh;
    }

    void Update(float timeElapsed)
    {
      for(int i = 0;i<vertices.size();i++)
      {
        renderables[i].m_position = ~Greet::Mat3::OrthographicViewport() * Greet::Vec2(GetCubeScreenPos(i)) - 10.0f;
      }
    }

    void Render(const Greet::Ref<Greet::Camera3D>& camera) const
    {
      if(showUi)
      {
        Greet::Color color{1.0f,1.0f,1.0f,1.0f};
        LineRenderer::GetInstance().DrawLine(camera, vertices[0],vertices[1], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[2],vertices[3], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[4],vertices[5], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[6],vertices[7], color);

        LineRenderer::GetInstance().DrawLine(camera, vertices[0],vertices[2], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[1],vertices[3], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[4],vertices[6], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[5],vertices[7], color);

        LineRenderer::GetInstance().DrawLine(camera, vertices[0],vertices[4], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[1],vertices[5], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[2],vertices[6], color);
        LineRenderer::GetInstance().DrawLine(camera, vertices[3],vertices[7], color);
      }

      material.Bind();
      camera->SetShaderUniforms(material.GetShader());
      material.GetShader()->SetUniform4f("uMaterialColor", {1.0f,1.0f,1.0f,1.0f});
      material.GetShader()->SetUniformMat4("uTransformationMatrix", Greet::Mat4::Translate({-0.5f - (size - 2) * 0.5f}));
      mcMesh.Bind();
      GLCall(glDisable(GL_CULL_FACE));
      mcMesh.Render();
      mcMesh.Unbind();
      material.Unbind();

      if(showUi)
      {
        shader2d->Enable();
        shader2d->SetUniformMat3("pr_matrix", Greet::Mat3::OrthographicViewport());
        renderer->Begin();
        int i = 0;
        for(auto&& renderable : renderables)
        {
          renderer->Draw(renderable);
          renderer->DrawText(std::to_string(i), renderable.m_position+Greet::Vec2{0.0f,-10.0f}, Greet::FontManager::Get("roboto", 30),0xff000000);
          i++;
        }
        renderer->End();
        renderer->Flush();
        shader2d->Disable();
      }
    }

    void SetDataPoint(int index, float value)
    {
      int z = index / (4) + (size - 2) / 2;
      int y = (index % 4) / 2 + (size - 2) / 2;
      int x = (index % 2) + (size - 2) / 2;

      MCPointData& point = data[x + (y + z * size) * size];

      point.magnitude = point.magnitude / fabs(point.magnitude) * value;
      mcMesh.UpdateData(data, x,y,z,1,1,1);
    }

    void OnEvent(Greet::Event& event)
    {
      if(EVENT_IS_TYPE(event, Greet::EventType::MOUSE_PRESS))
      {
        Greet::MousePressEvent& e = (Greet::MousePressEvent&)event;
        if(e.GetButton() == GLFW_MOUSE_BUTTON_1)
        {
          std::pair<size_t, float> point;
          bool pressed = false;

          Greet::Mat3 invProjectionMatrix = ~Greet::Mat3::OrthographicViewport();
          for(int i = 0;i<vertices.size();i++)
          {
            Greet::Vec3<float> projection = invProjectionMatrix * GetCubeScreenPos(i);
            Greet::Vec2 mousePos = invProjectionMatrix * e.GetPosition() - 10.0f;
            if((mousePos - renderables[i].m_position).LengthSQ() < 100)
            {
              if(!pressed || projection.z < point.second)
                point = {i, projection.z};
              pressed = true;
            }
          }
          if(pressed)
          {
            int x = ceil(vertices[point.first].x) + (size - 2) / 2;
            int y = ceil(vertices[point.first].y) + (size - 2) / 2;
            int z = ceil(vertices[point.first].z) + (size - 2) / 2;
            int index = x + (y + z * size) * size;
            data[index].magnitude *= -1;
            if(data[index].Inhabited())
              renderables[point.first].m_color = 0xff00ff00;
            else
              renderables[point.first].m_color = 0xff000000;
            mcMesh.UpdateData(data, x,y,z,1,1,1);
          }
        }
      }
      else if(EVENT_IS_TYPE(event, Greet::EventType::KEY_PRESS))
      {
        Greet::KeyPressEvent& e = (Greet::KeyPressEvent&)event;
        if(e.GetButton() == GREET_KEY_F)
          showUi = !showUi;
      }
    }

    Greet::Vec3<float> GetCubeScreenPos(size_t index)
    {
      return camera->GetWorldToScreenCoordinate(vertices[index]);
    }
};

