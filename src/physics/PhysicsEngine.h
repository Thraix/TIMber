#pragma once

#define RENDER_PHYSICS

#ifdef RENDER_PHYSICS 
#include <graphics/models/Material.h>
#include <graphics/models/Mesh.h>
#include <graphics/models/MeshFactory.h>
#include <graphics/cameras/Camera3D.h>
#endif

#include "PhysicsBody.h"

#include <vector>


class PhysicsEngine 
{
  private:
#ifdef RENDER_PHYSICS 
    Greet::Material material;
    Greet::Mesh* sphereMesh;
    Greet::Mesh* planeMesh;
#endif
    std::vector<PhysicsBody*> bodies;
  public:
    PhysicsEngine();
    virtual ~PhysicsEngine();
    void AddPhysicsBody(PhysicsBody* body);
    void Update(float timeElapsed);
    bool CheckCollision(PhysicsBody& first, PhysicsBody& second);
    void ResolveCollision(PhysicsBody& first, PhysicsBody& second);
#ifdef RENDER_PHYSICS
    void Render(const Greet::Ref<Greet::Camera3D>& camera) const;
#endif
};

