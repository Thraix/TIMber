#pragma once

#include <math/Vec3.h>

enum class CollisionType
{
  Sphere, Plane, Triangle, MCMesh, 
};

class PhysicsBody 
{
  private:

    CollisionType type;

    Greet::Vec3<float> netForce;
    Greet::Vec3<float> velocity;
    Greet::Vec3<float> position;
    float mass;

  public:
    PhysicsBody(CollisionType type, const Greet::Vec3<float>& position, float mass);
    void ApplyForce(const Greet::Vec3<float>& force);
    void ApplyForceAsAcceleration(const Greet::Vec3<float>& acceleration);
    void UpdatePosition(float timeElapsed);
    void CheckCollision(PhysicsBody* other);
    void ResolveCollision(PhysicsBody* other);
    void ResetForce();

    CollisionType GetType() const { return type; }
    const Greet::Vec3<float> GetPosition() const { return position; }
    const Greet::Vec3<float> GetVelocity() const { return velocity; }
    void SetPosition(const Greet::Vec3<float>& position) { this->position = position; }
    void SetVelocity(const Greet::Vec3<float>& velocity) { this->velocity = velocity; }
    float GetMass() const { return mass; }

};

class PhysicsSphereBody : public PhysicsBody
{
  private:
    float radius;
  public:
    PhysicsSphereBody(const Greet::Vec3<float>& position, float mass, float radius)
      : PhysicsBody{CollisionType::Sphere, position, mass}, radius{radius}
    {
    }

    float GetRadius() const { return radius; } 
};

class PhysicsPlaneBody : public PhysicsBody
{
  private:
    Greet::Vec3<float> normal;
  public:
    PhysicsPlaneBody(const Greet::Vec3<float>& position, const Greet::Vec3<float>& normal)
      : PhysicsBody{CollisionType::Plane, position, 0.0f}, normal{normal}
    {
      this->normal.Normalize();
    }

    const Greet::Vec3<float>& GetNormal() const { return normal; } 
};
