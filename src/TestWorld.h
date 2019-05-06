#pragma once

#include <graphics/layers/Scene.h>
#include <event/MouseEvent.h>
#include <event/KeyEvent.h>
#include <graphics/shaders/Shader.h>
#include <graphics/models/Material.h>
#include <graphics/textures/TextureManager.h>
#include <graphics/Skybox.h>
#include <graphics/models/MeshFactory.h>

#include "physics/PhysicsEngine.h"
#include "MCMesh.h"
#include "MCPointData.h"
#include "LineRenderer.h"
#include "PlayerCamera.h"

class TestWorld : public Greet::Scene
{
  private:
    size_t size = 11;
    std::vector<MCPointData> data;
    MCMesh* mesh;
    Greet::Skybox skybox;
    Greet::Material material;
    PlayerCamera camera;
    bool mouseGrab = true;
    PhysicsEngine engine;

    PhysicsPlaneBody plane;
    PhysicsPlaneBody plane2;
    PhysicsPlaneBody plane3;
    PhysicsPlaneBody plane4;
    PhysicsPlaneBody plane5;
    PhysicsSphereBody sphere;
    PhysicsSphereBody sphere2;
    PhysicsSphereBody sphere3;

  public:
    TestWorld()
      : skybox{Greet::TextureManager::Get3D("skybox")}, 
      material{Greet::Shader::FromFile("res/shaders/terrain.shader")},
      camera{Greet::Mat4::ProjectionMatrix(Greet::Window::GetAspect(), 90, 0.001f, 100.0f),{0.0f, 0.0f, 10.0f}, 0.0f,0.0f},
      plane{{0.0f, -0.0f, 0.0f}, {2.0f, 1.0f, 0.0f}}, 
      plane2{{0.0f, -10.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, 
      plane3{{25.0f, -10.0f, 0.0f}, {-2.0f, 1.0f, 0.0f}}, 
      plane4{{0.0f, 0.0f, 20.0f}, {0.0f, 0.0f,-1.0f}}, 
      plane5{{0.0f, 0.0f, -20.0f}, {0.0f, 0.0f,1.0f}}, 
      sphere{{1.0f, 10.0f, 0.0f}, 1.0f, 1.0f},
      sphere2{{0.0f, 15.0f, 0.0f},8.0f, 2.0f},
      sphere3{{0.2f, 19.0f, 0.0f},1.0f, 1.0f}
    {
      data = std::vector<MCPointData>(size * size * size);
      for(int z = 1;z<10;z++)
      {
        for(int x = 1;x<10;x++)
        {
          SetDataPoint({Voxel::grass, 0.5}, x,1,z);
        }
      }
      for(int z = 4;z<7;z++)
      {
        for(int x = 4;x<7;x++)
        {
          SetDataPoint({Voxel::grass, 0.2}, x,2,z);  
        }
      }
          SetDataPoint({Voxel::grass, 0.5}, 5,2,5);  
      mesh = new MCMesh(data, size, size, size);
      Greet::Window::GrabMouse(true);

      engine.AddPhysicsBody(&plane);
      engine.AddPhysicsBody(&plane2);
      engine.AddPhysicsBody(&plane3);
      engine.AddPhysicsBody(&plane4);
      engine.AddPhysicsBody(&plane5);
      engine.AddPhysicsBody(&sphere);
      engine.AddPhysicsBody(&sphere2);
      engine.AddPhysicsBody(&sphere3);
      PhysicsSphereBody** bodies = new PhysicsSphereBody*[100];
      for(int i = 0;i<100;i++)
      {
        bodies[i] = new PhysicsSphereBody({2 + (float)(i / 10) * 3, 0.0f, (float)(i % 10) * 3 - 10}, 1.0f, 1.0f);
        engine.AddPhysicsBody(bodies[i]);
        bodies[i]->ApplyForceAsAcceleration({0, -9.82,0});
      }


      // Apply gravity to sphere
      sphere.ApplyForceAsAcceleration({0, -9.82, 0});
      sphere2.ApplyForceAsAcceleration({0, -9.82, 0});
      sphere3.ApplyForceAsAcceleration({0, -9.82, 0});
    }

    virtual ~TestWorld()
    {
      delete mesh;
    }

    void Update(float timeElapsed) override
    {
      camera.Update(timeElapsed);
      //engine.Update(timeElapsed);
    }

    void Render() const override
    {
      skybox.Render(camera);
#if 1
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

#ifdef RENDER_PHYSICS
      //engine.Render(camera);
#endif
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

