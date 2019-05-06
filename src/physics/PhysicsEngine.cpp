#include "PhysicsEngine.h"

#include <logging/Log.h>

using namespace Greet;

#ifdef RENDER_PHYSICS
PhysicsEngine::PhysicsEngine()
  : material{Greet::Shader::FromFile("res/shaders/3dshader.shader")}
{
  MeshData* data = MeshFactory::Sphere({}, 1.0f, 20, 20);
  sphereMesh = new Mesh(data);
  sphereMesh->SetEnableCulling(false);
  delete data;
  data = Greet::MeshFactory::Quad(0.0f,0.0f,0.0f,60.0f,60.0f);
  planeMesh = new Mesh(data);
  planeMesh->SetEnableCulling(false);
  delete data;
}
#else
PhysicsEngine::PhysicsEngine()
{

}
#endif

PhysicsEngine::~PhysicsEngine()
{
#ifdef RENDER_PHYSICS
  delete sphereMesh;
  delete planeMesh;
#endif
}

void PhysicsEngine::AddPhysicsBody(PhysicsBody* body)
{
  bodies.push_back(body);
}

void PhysicsEngine::Update(float timeElapsed)
{
  for(auto&& body : bodies)
  {
    body->UpdatePosition(timeElapsed);
  }

  for(auto it1 = bodies.begin();it1 != bodies.end();it1++)
  {
    for(auto it2 = it1+1; it2 != bodies.end(); it2++)
    {
      if(CheckCollision(**it1, **it2))
      {
        ResolveCollision(**it1, **it2);
      }
    }
  }
}

bool PhysicsEngine::CheckCollision(PhysicsBody& first, PhysicsBody& second)
{
  if((int)first.GetType() > (int)second.GetType())
    return CheckCollision(second, first);

  if(first.GetType() == CollisionType::Sphere)
  {
    PhysicsSphereBody& sphere1 = (PhysicsSphereBody&)first;
    if(second.GetType() == CollisionType::Sphere)
    {
      PhysicsSphereBody& sphere2 = (PhysicsSphereBody&)second;
      float radius = sphere1.GetRadius() + sphere2.GetRadius();
      return (sphere1.GetPosition() - sphere2.GetPosition()).LengthSQ() < radius * radius; 
    }
    else if(second.GetType() == CollisionType::Plane)
    {
      PhysicsPlaneBody& plane2 = (PhysicsPlaneBody&)second;
      return Vec::Dot(plane2.GetNormal(), sphere1.GetPosition()) - Vec::Dot(plane2.GetNormal(), plane2.GetPosition()) < sphere1.GetRadius();
    }
  }
  Log::Error("Unimplemented collision types, ", (int)first.GetType(), ", ",(int)second.GetType());
  return false;
}

void PhysicsEngine::ResolveCollision(PhysicsBody& first, PhysicsBody& second)
{
  if((int)first.GetType() > (int)second.GetType())
    return ResolveCollision(second, first);
  if(first.GetType() == CollisionType::Sphere)
  {
    PhysicsSphereBody& sphere1 = (PhysicsSphereBody&)first;
    if(second.GetType() == CollisionType::Sphere)
    {
      PhysicsSphereBody& sphere2 = (PhysicsSphereBody&)second;
      // How fast sphere1 moves relative to sphere2 standing still.
      Vec3<float> relativeVelocity = sphere1.GetVelocity() - sphere2.GetVelocity();

      // vector from sphere1 to sphere2. 
      Vec3<float> relativePosition = sphere2.GetPosition() - sphere1.GetPosition();

      Vec3<float> velocityTowardsCenters = relativePosition * Vec::Dot(relativeVelocity, relativePosition) / (relativePosition.LengthSQ());

      float radius = sphere1.GetRadius() + sphere2.GetRadius();
      float distanceToResolve = radius * radius - (sphere1.GetPosition() - sphere2.GetPosition()).LengthSQ(); 
      float time = distanceToResolve / velocityTowardsCenters.Length();

      sphere1.SetPosition(sphere1.GetPosition() - sphere1.GetVelocity() * time);
      sphere2.SetPosition(sphere2.GetPosition() - sphere2.GetVelocity() * time);

      Vec3<float> pos1ToPos2 = sphere2.GetPosition() - sphere1.GetPosition();
      Vec3<float> vel1ToVel2 = sphere2.GetVelocity() - sphere1.GetVelocity();

      float totalMass = sphere1.GetMass() + sphere2.GetMass();
      sphere1.SetVelocity(sphere1.GetVelocity() + pos1ToPos2 * 2 * sphere2.GetMass() / totalMass * Vec::Dot(-vel1ToVel2, -pos1ToPos2) / pos1ToPos2.LengthSQ());

      sphere2.SetVelocity(sphere2.GetVelocity() - pos1ToPos2 * 2 * sphere1.GetMass() / totalMass * Vec::Dot(vel1ToVel2, pos1ToPos2) / pos1ToPos2.LengthSQ());

      return;
    }
    else if(second.GetType() == CollisionType::Plane)
    {
      PhysicsPlaneBody& plane2 = (PhysicsPlaneBody&)second;
      float planeD = Vec::Dot(plane2.GetNormal(), plane2.GetPosition());

      float t = (sphere1.GetRadius() - Vec::Dot(plane2.GetNormal(), sphere1.GetPosition()) + planeD); 

      if(sphere1.GetVelocity() != Vec3<float>{0})
      {
        t /= Vec::Dot(plane2.GetNormal(), sphere1.GetVelocity());
      }

      sphere1.SetPosition(sphere1.GetPosition() + sphere1.GetVelocity() * t);

      sphere1.SetVelocity(sphere1.GetVelocity() + plane2.GetNormal() * 2 * Vec::Dot(-sphere1.GetVelocity(), plane2.GetNormal()) / plane2.GetNormal().LengthSQ());
      //sphere1.SetVelocity({0});
      //sphere1.ResetForce();
      Log::Info(sphere1.GetPosition()) ;
      Log::Info(sphere1.GetRadius() - (Vec::Dot(plane2.GetNormal(), sphere1.GetPosition()) - Vec::Dot(plane2.GetNormal(), plane2.GetPosition())));
      return;
    }
  }

  //Log::Error("Unimplemented collision types");
}
#ifdef RENDER_PHYSICS
static int i = 0;
void PhysicsEngine::Render(const Camera& camera) const
{
  i++;
  material.Bind(&camera);
  material.GetShader().SetUniform4f("mat_color", Vec4{1.0f, 0.0f, 0.0f, 1.0f});
  for(auto&& body : bodies)
  {
    if(body->GetType() == CollisionType::Sphere)
    {
      material.GetShader().SetUniformMat4("transformationMatrix", Mat4::Translate(body->GetPosition()) * Mat4::Scale({((PhysicsSphereBody*)body)->GetRadius()}));
      sphereMesh->Bind();
      sphereMesh->Render();
      sphereMesh->Unbind();
    }
    else if(body->GetType() == CollisionType::Plane)
    {
      PhysicsPlaneBody* plane = (PhysicsPlaneBody*)body;
      material.GetShader().SetUniformMat4("transformationMatrix", Mat4::Translate(body->GetPosition()) * Mat4::RotateRX(atan2(plane->GetNormal().z, plane->GetNormal().y)) * Mat4::RotateRZ(-atan2(plane->GetNormal().x, plane->GetNormal().y)));
      planeMesh->Bind();
      planeMesh->Render();
      planeMesh->Unbind();
    }
  }
  material.Unbind();
}
#endif
