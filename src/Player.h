#pragma once

#include <graphics/renderers/BatchRenderer3D.h>
#include "PlayerCamera.h"
#include "Inventory.h"
#include "InventoryScene.h"

class World;

class Player
{
  struct PlayerControl final
  {
    bool forward = false;
    bool left = false;
    bool backward= false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool mouseGrab = true;
    bool mouseDown = false;
    int mouseButton = 0;
  };

  struct PlayerMovement final
  {
    float maxVelocity = 6.0f;
    float maxFallVelocity = 0.5f;
    bool onGround = true;

    Greet::Vec3<float> velocity = {0.0f,0.0f,0.0f};
  };

  struct PlayerModel final
  {
    Greet::Mesh* mesh;
    Greet::Material material;
    Greet::EntityModel* model;

    PlayerModel()
      : material{Greet::ShaderFactory::Shader3D()}
    {
      mesh = new Greet::Mesh(Greet::MeshFactory::Cube({0,0.2f,0},{0.2f,0.4f,0.2f}));
      model = new Greet::EntityModel(mesh, &material);
    }

    ~PlayerModel()
    {
      delete mesh;
      delete model;
    }
  };

  private:
    Inventory inventory;
    InventoryScene invScene;
    Greet::Ref<PlayerCamera> camera;
    Greet::Vec3<float> position;
    PlayerMovement playerMovement;
    PlayerControl playerControl;
    PlayerModel playerModel;
    World* world;

  private:
    // Since this initialization takes up a lot of space the code looks cleaner with this
    static Greet::Ref<PlayerCamera> CreateCamera();

  public:

    Player(World* world, const Greet::Vec3<float>& position);
    void OnWorldInit();
    void Render() const;
    void Update(float timeElapsed);
    void OnEvent(Greet::Event& event);

    const Greet::Ref<PlayerCamera>& GetCamera() const;
    const Greet::Vec3<float>& GetPosition() const { return position; }
    const float GetReach() const { return 20; };

  private:
    void Move(float timeElapsed);
    void Gravity(float timeElapsed);
};
