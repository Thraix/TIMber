#pragma once

#include "Chunk.h"
#include "Player.h"

#include <internal/GreetTypes.h>
#include <graphics/layers/Scene.h>
#include <graphics/models/EntityModel.h>
#include <graphics/models/TPCamera.h>
#include <graphics/Window.h>
#include <graphics/Skybox.h>
#include <graphics/textures/TextureManager.h>

class World : public Greet::Scene
{
  Greet::Skybox skybox;
  Player player;

  Greet::Material terrainMaterial;
  const Greet::Texture2D& noiseTexture;
  uint width;
  uint length;

  Chunk* chunks;

  public:

    // Initialize the world with a given size
    World(uint width, uint length);

    void Render() const override;
    void Update(float timeElapsed) override;
    void OnEvent(Greet::Event& event) override;
};
