#include "PhysicsBody.h"

using namespace Greet;

PhysicsBody::PhysicsBody(CollisionType type, const Greet::Vec3<float>& position, float mass)
  : type{type}, position{position}, mass{mass}
{

}

void PhysicsBody::ApplyForce(const Vec3<float>& force)
{
  netForce += force;
}

void PhysicsBody::ApplyForceAsAcceleration(const Greet::Vec3<float>& acceleration)
{
  netForce += acceleration * mass;

}

void PhysicsBody::UpdatePosition(float timeElapsed)
{
  if(mass <= 0.0f)
    return;
  Vec3<float> acceleration = netForce / mass;
  velocity += acceleration * timeElapsed;
  position += velocity * timeElapsed;
}

void PhysicsBody::CheckCollision(PhysicsBody* other)
{
}

void PhysicsBody::ResolveCollision(PhysicsBody* other)
{
}

void PhysicsBody::ResetForce()
{
  netForce = 0;
}
