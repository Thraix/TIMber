#pragma once

#include <graphics/layers/Scene.h>
#include <event/MouseEvent.h>
#include <event/KeyEvent.h>
#include <graphics/shaders/Shader.h>
#include <graphics/models/Material.h>
#include <graphics/textures/TextureManager.h>
#include <graphics/Skybox.h>

#include "MCPointData.h"
#include "LineRenderer.h"
#include "PlayerCamera.h"
#include "MCMesh.h"

class TestWorld : public Greet::Scene
{
  private:
    size_t size = 10;
    std::vector<MCPointData> data;
    MCMesh* mesh;
    Greet::Skybox skybox;
    Greet::Material material;
    PlayerCamera camera;
    bool mouseGrab = true;
  public:
    TestWorld()
      : skybox{Greet::TextureManager::Get3D("skybox")}, 
      material{Greet::Shader::FromFile("res/shaders/terrain.shader")},
      camera{Greet::Mat4::ProjectionMatrix(Greet::Window::GetAspect(), 90, 0.001f, 100.0f),{0.0f}, 0.0f,0.0f}
    {
      data = std::vector<MCPointData>(size * size * size);
      SetDataPoint({true}, 1,1,1);
      SetDataPoint({true}, 2,1,1);
      mesh = new MCMesh(data, size, size, size);
      Greet::Window::GrabMouse(true);
    }

    virtual ~TestWorld()
    {
      delete mesh;
    }

    void Update(float timeElapsed) override
    {
      camera.Update(timeElapsed);
    }

    void Render() const override
    {
      skybox.Render(camera);
#if 0
      for(int z = 0;z < size+1;z++)
      {
        for(int x = 0;x < size+1;x++)
        {
          LineRenderer::GetInstance().DrawLine(camera, Greet::Vec3<float>(x, 0, z), Greet::Vec3<float>(x, size, z), Greet::Vec4(1,1,1,1));
        }
      }
      for(int y = 0;y < size+1;y++)
      {
        for(int x = 0;x < size+1;x++)
        {
          LineRenderer::GetInstance().DrawLine(camera, Greet::Vec3<float>(x, y, 0), Greet::Vec3<float>(x, y, size), Greet::Vec4(1,1,1,1));
        }
      }
      for(int z = 0;z < size+1;z++)
      {
        for(int y = 0;y < size+1;y++)
        {
          LineRenderer::GetInstance().DrawLine(camera, Greet::Vec3<float>(0, y, z), Greet::Vec3<float>(size, y, z), Greet::Vec4(1,1,1,1));
        }
      }
#endif

      material.Bind(&camera);
      material.GetShader().SetUniformMat4("transformationMatrix", Greet::Mat4::Identity());
      GLCall(glDisable(GL_CULL_FACE));
      mesh->Bind();
      mesh->Render();
      mesh->Unbind();
      material.Unbind();
    }

    void OnEvent(Greet::Event& event) override
    {
      camera.OnEvent(event);
      if(EVENT_IS_TYPE(event, Greet::EventType::MOUSE_PRESS))
      {
        Greet::MousePressEvent& e = (Greet::MousePressEvent&)event;
        if(e.GetButton() == GLFW_MOUSE_BUTTON_1)
        {
          SetDataPoint({false}, 3, 1, 1);
          mesh->UpdateData(data, 3, 1, 1, 1, 1, 1);
        }
        else if(e.GetButton() == GLFW_MOUSE_BUTTON_2)
        {
          SetDataPoint({true}, 3, 1, 1);
          mesh->UpdateData(data, 3, 1, 1, 1, 1, 1);
        }
      }
      else if(EVENT_IS_TYPE(event, Greet::EventType::KEY_PRESS))
      {
        Greet::KeyPressEvent& e = (Greet::KeyPressEvent&)event;
        if(e.GetButton() == GLFW_KEY_ESCAPE)
        {
          mouseGrab = !mouseGrab;
          Greet::Window::GrabMouse(mouseGrab);

        }
      }
    }

    void SetDataPoint(const MCPointData& point, int x, int y, int z)
    {
      data[x + (y + z * size) * size] = point;
    }
};

