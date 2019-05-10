#pragma once

#include <graphics/layers/Scene.h>
#include <graphics/models/TPCamera.h>
#include <event/MouseEvent.h>
#include <event/KeyEvent.h>

#include "Cube.h"

class MarchingDemo : public Greet::Scene
{
  private:
    Greet::TPCamera camera;
    Cube cube1;
    Cube cube2;
    bool fill = false;
  public:
    MarchingDemo()
      : camera{Greet::Mat4::ProjectionMatrix(Greet::Window::GetAspect(), 90, 0.01f, 1000.0f)},
      cube1{camera, 2},
      cube2{camera, 4}
    {

    }

    virtual ~MarchingDemo()
    {

    }

    void Render() const
    {
      if(fill)
        cube1.Render(camera);
      else
        cube2.Render(camera);
    }

    void Update(float timeElapsed)
    {
      camera.Update(timeElapsed);
      cube1.Update(timeElapsed);
      cube2.Update(timeElapsed);
    }

    void OnEvent(Greet::Event& event)
    {
      if(EVENT_IS_TYPE(event, Greet::EventType::KEY_PRESS))
      {
        Greet::KeyPressEvent& e = (Greet::KeyPressEvent&)event;
        if(e.GetButton() == GLFW_KEY_E)
        {
          fill = !fill;
        }
      }
      if(EVENT_IS_TYPE(event, Greet::EventType::MOUSE_PRESS))
      {
        Greet::MousePressEvent& e = (Greet::MousePressEvent&)event;
        if(e.GetButton() == GLFW_MOUSE_BUTTON_3)
          camera.OnEvent(event);
      }
      else
      {
        camera.OnEvent(event);
      }
      cube1.OnEvent(event);
      cube2.OnEvent(event);
    }

    void WindowResize(Greet::WindowResizeEvent& event) 
    {
      camera.SetProjectionMatrix(Greet::Mat4::ProjectionMatrix(90, Greet::Window::GetAspect(), 0.01f, 1000.0f));
    }
};

