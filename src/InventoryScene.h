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
    Greet::Renderable2D renderable;
    Greet::Renderable2D selected;

  public:
    InventoryScene(const Inventory& inventory)
      : Layer{new Greet::BatchRenderer(), Greet::ShaderFactory::DefaultShader(), Greet::Mat3::Orthographic(0,Greet::Window::GetWidth(), 0, Greet::Window::GetHeight())}, inventory{inventory}, renderable{{10.0f,0},{64,64},0xffffffff, {Greet::TextureManager::Get2D("material")}}, selected{{10.0f,0},{64,64},0xffffffff, {Greet::TextureManager::Get2D("selected_material")}} 
    {

    }

    void Render() const override
    {
      Greet::Renderable2D renderableCpy = renderable;
      Greet::Renderable2D selectedCpy = selected;
      Greet::BatchRenderer* renderer = (Greet::BatchRenderer*)m_renderer; 
      size_t size = inventory.GetInventorySize();
      float height = (size * 70.0f) - 6.0f;
      for(int i = 0;i<size;i++)
      {
        const Voxel& voxel = inventory.GetMaterial(i);
        float amount = inventory.GetMaterialAmount(i);

        std::string amountStr = std::to_string((int)amount);
        float yPos = (Greet::Window::GetHeight() - height) * 0.5f + 70.0f * i;
        renderableCpy.m_color = Greet::ColorUtils::Vec4ToColorHex(voxel.color);
        renderableCpy.m_position.y = yPos;
        renderer->Submit(renderableCpy);

        Greet::Font* font = Greet::FontManager::Get("roboto", 20);
        if(inventory.GetCurrentPos() == i)
        {

          selectedCpy.m_position = renderableCpy.m_position;
          renderer->Submit(selectedCpy);
        }
        renderer->SubmitString(amountStr, {10.0f + 32 - font->GetWidthOfText(amountStr)* 0.5f, yPos + 60.0f}, font, 0xffffffff);
      }
    }
};

