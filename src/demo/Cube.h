#pragma once

#include <graphics/fonts/FontManager.h>
#include <graphics/models/Mesh.h>
#include <graphics/models/Material.h>
#include <graphics/models/MeshFactory.h>
#include <event/MouseEvent.h>
#include <graphics/renderers/BatchRenderer.h>

#include "../MCPointData.h"
#include "../MCMesh.h"
#include "../LineRenderer.h"

class Cube 
{
  private:
    size_t size;
    Greet::Mesh* mesh;
    Greet::Material material;
    Greet::Shader shader2d;
    std::vector<MCPointData> data;
    MCMesh mcMesh;
    std::vector<Greet::Vec3<float>> vertices;
    Greet::Camera& camera;
    Greet::BatchRenderer* renderer;
    std::vector<Greet::Renderable2D> renderables;


  public:
    Cube(Greet::Camera& camera, size_t size)
      : size{size}, material{Greet::Shader::FromFile("res/shaders/terrain.shader")}, 
      shader2d{Greet::Shader::FromFile("res/shaders/2dshader.shader")},
      data{(size*size*size)},
      mcMesh(data, size, size, size), camera{camera}
    {
      Greet::MeshData* meshData = Greet::MeshFactory::Quad(0,0,0,1,1);
      mesh = new Greet::Mesh(meshData);
      delete meshData;
      renderer = new Greet::BatchRenderer();

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
        renderables.push_back(Greet::Renderable2D{{-10.0f},{20.0f},0xff000000, Greet::Sprite(Greet::TextureManager::Get2D("select"))});
      }
      shader2d.Enable();
      GLint texIDs[32];
      for (int i = 0; i < 32; i++)
      {
        texIDs[i] = i;
      }
      shader2d.Enable();
      shader2d.SetUniform1iv("textures", 32, texIDs);
      shader2d.Disable();
    }

    virtual ~Cube()
    {
      delete mesh;
    }

    void Update(float timeElapsed)
    {
      for(int i = 0;i<vertices.size();i++)
      {
        Greet::Vec3<float> projection = GetCubeScreenPos(i);
        renderables[i].m_position = {projection.x, projection.y};
        Greet::Window::TransformMousePosToScreen(renderables[i].m_position);
        renderables[i].m_position -= 10.0f;
      }
    }

    void Render(const Greet::Camera& camera) const
    {
      Greet::Vec4 color{1.0f,1.0f,1.0f,1.0f};
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

      material.Bind(&camera);
      material.GetShader().SetUniform4f("mat_color", {1.0f,1.0f,1.0f,1.0f});
      material.GetShader().SetUniformMat4("transformationMatrix", Greet::Mat4::Translate({-0.5f - (size - 2) * 0.5f}));
      mcMesh.Bind();
      GLCall(glDisable(GL_CULL_FACE));
      mcMesh.Render();
      mcMesh.Unbind();
      material.Unbind();

      shader2d.Enable();
      shader2d.SetUniformMat3("pr_matrix", Greet::Mat3::Orthographic(0, Greet::Window::GetWidth(), 0, Greet::Window::GetHeight()));
      renderer->Begin();
      int i = 0;
      for(auto&& renderable : renderables)
      {
        renderer->Submit(renderable);
        renderer->SubmitString(std::to_string(i), renderable.m_position, Greet::FontManager::Get("roboto", 40),0xff000000);
        i++;
      }
      renderer->End();
      renderer->Flush();
      shader2d.Disable();
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

          for(int i = 0;i<vertices.size();i++)
          {
            Greet::Vec3<float> projection = GetCubeScreenPos(i);
            Greet::Vec2 mousePos = e.GetPosition();
            Greet::Window::TransformMousePosToScreen((Greet::Vec2&)projection);
            Greet::Window::TransformMousePosToScreen(mousePos);
            if((mousePos - projection).LengthSQ() < 100)
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
            if(data[index].magnitude >= 0.0f)
              renderables[point.first].m_color = 0xff00ff00;
            else
              renderables[point.first].m_color = 0xff000000;
            mcMesh.UpdateData(data, x,y,z,1,1,1);
          }
        }
      }
    }

    Greet::Vec3<float> GetCubeScreenPos(size_t index)
    {
      Greet::Vec4 projection = camera.GetProjectionMatrix() * camera.GetViewMatrix() * vertices[index];
      Greet::Vec3<float> pos = Greet::Vec3<float>{projection} / projection.w;
      return pos;
    }
};

