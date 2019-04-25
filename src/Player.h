#pragma once

#include <graphics/models/TPCamera.h>
#include <graphics/renderers/BatchRenderer3D.h>
#include "HalfEdgeMesh.h"

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
      : material{Greet::Shader::FromFile("res/shaders/3dshader.shader")}
    {
      Greet::MeshData* data = Greet::MeshFactory::Cube(0,0.2f,0,0.2f,0.4f,0.2f);
      HalfEdgeMesh* halfEdgeMesh = new HalfEdgeMesh{data};
      mesh = new Greet::Mesh(halfEdgeMesh->vertexArray.data(), halfEdgeMesh->vertexArray.size(), halfEdgeMesh->indices.data(), halfEdgeMesh->indices.size());
      model = new Greet::EntityModel(mesh, &material);
      delete data;
    }

    ~PlayerModel()
    {
      delete mesh;
      delete model;
    }
  };

  private:
    Greet::TPCamera camera;
    Greet::Vec3<float> position;
    PlayerMovement playerMovement;
    PlayerControl playerControl;
    PlayerModel playerModel;
    World* world;

  private:
    // Since this initialization takes up a lot of space the code looks cleaner with this
    static Greet::TPCamera CreateCamera();

  public:

    Player(World* world, const Greet::Vec3<float>& position);
    void OnWorldInit();
    void Render() const;
    void Update(float timeElapsed);
    void OnEvent(Greet::Event& event);

    const Greet::TPCamera& GetCamera() const;
    const Greet::Vec3<float>& GetPosition() const { return position; }
    const float GetReach() const { return 8; };

  private:
    void Move(float timeElapsed);
    void Gravity(float timeElapsed);
};
