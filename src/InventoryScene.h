#pragma once

#include <graphics/layers/Layer.h>
#include <graphics/renderers/BatchRenderer.h>
#include <graphics/shaders/ShaderFactory.h>
#include <graphics/fonts/FontManager.h>

#include "Inventory.h"

class InventoryScene : public Greet::Layer
{
  private:
    const Inventory& inventory;

  public:
    InventoryScene(const Inventory& inventory)
      : Layer{new Greet::BatchRenderer(), Greet::ShaderFactory::DefaultShader(), Greet::Mat3::Orthographic(0,Greet::Window::GetWidth(), 0, Greet::Window::GetHeight())}, inventory{inventory}
    {

    }

    void Render() const override
    {
      Greet::BatchRenderer* renderer = (Greet::BatchRenderer*)m_renderer; 
      size_t size = inventory.GetInventorySize();
      for(int i = 0;i<size;i++)
      {
        const Voxel& voxel = inventory.GetMaterial(i);
        float amount = inventory.GetMaterialAmount(i);
        std::string astrix = inventory.GetCurrentPos() == i ? "* " : "";
        renderer->SubmitString(astrix + voxel.name + ": " + std::to_string((int)amount), {0.0f, 20.0f + 30.0f * i}, Greet::FontManager::Get("roboto", 20), 0xffffffff);
      }
    }
};

