#pragma once

#include <functional>

#include <graphics/layers/Scene.h>
#include <graphics/GlobalSceneManager.h>
#include <graphics/cameras/TPCamera.h>
#include <event/MouseEvent.h>
#include <event/KeyEvent.h>
#include <graphics/gui/GUIScene.h>
#include <graphics/gui/ComponentFactory.h>
#include <graphics/gui/Frame.h>
#include <graphics/gui/Viewport.h>
#include <graphics/gui/Slider.h>
#include <graphics/gui/Button.h>

#include "Cube.h"

class MarchingDemo : public Greet::Scene
{
  private:
    Greet::TPCamera camera;
    Cube cube1;
    Cube cube2;
    bool fill = false;
    Greet::GUIScene* guiScene;
  public:
    MarchingDemo()
      : camera{90, 0.01f, 1000.0f},
      cube1{camera, 2},
      cube2{camera, 4}
    {
      guiScene = new Greet::GUIScene(new Greet::GUIRenderer(), Greet::Shader::FromFile("res/shaders/gui.shader"));
      guiScene->AddFrame(Greet::FrameFactory::GetFrame("res/guis/demo.xml"));
      Greet::GlobalSceneManager::GetSceneManager().Add2DScene(guiScene, "gui");
      Greet::Frame* frame = guiScene->GetFrame("TopComponent");
      Greet::Viewport* viewport = frame->GetComponentByName<Greet::Viewport>("demoViewport");
      using namespace std::placeholders;
      frame->GetComponentByName<Greet::Slider>("Slider0")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 0));
      frame->GetComponentByName<Greet::Slider>("Slider1")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 1));
      frame->GetComponentByName<Greet::Slider>("Slider2")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 2));
      frame->GetComponentByName<Greet::Slider>("Slider3")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 3));
      frame->GetComponentByName<Greet::Slider>("Slider4")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 4));
      frame->GetComponentByName<Greet::Slider>("Slider5")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 5));
      frame->GetComponentByName<Greet::Slider>("Slider6")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 6));
      frame->GetComponentByName<Greet::Slider>("Slider7")
        ->SetOnValueChangeCallback(std::bind(&MarchingDemo::OnSliderChange, std::ref(*this), _1, _2, _3, 7));
      frame->GetComponentByName<Greet::Button>("button")
        ->SetOnClickCallback(std::bind(&MarchingDemo::OnReset, std::ref(*this), _1));
      viewport->GetSceneManager().Add3DScene(this, "demo");
    }

    virtual ~MarchingDemo()
    {
      delete guiScene;
    }

    void OnReset(Greet::Component*)
    {
      Greet::Frame* frame = guiScene->GetFrame("TopComponent");
      for(int i = 0;i<8;i++)
      {
        cube1.SetDataPoint(i, 0.5f);
        cube2.SetDataPoint(i, 0.5f);
        frame->GetComponentByName<Greet::Slider>("Slider"+std::to_string(i))->SetValue(0.5f);
      }
    }

    void OnSliderChange(Greet::Component* component, float oldValue, float newValue, int index)
    {
      cube1.SetDataPoint(index, newValue);
      cube2.SetDataPoint(index, newValue);
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
      camera.SetProjectionMatrix(Greet::Mat4::ProjectionMatrix(90, event.GetWidth() / (float)event.GetHeight(), 0.01f, 1000.0f));
    }
};

