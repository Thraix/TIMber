#include "Player.h"

#include <Greet.h>
#include "World.h"

using namespace Greet;

PlayerCamera Player::CreateCamera()
{
  float distance = 0.0f;
  return PlayerCamera(Greet::Mat4::ProjectionMatrix(Window::GetAspect(), 90.0, 0.01, 1000.0), Greet::Vec3<float>(), 0.0f, 0.0f);
}

Player::Player(World* world, const Greet::Vec3<float>& position)
  : invScene{inventory}, camera{CreateCamera()}, position{position}, world{world}
{
  RenderEngine::Add2DScene(&invScene, "inventory");
}

void Player::OnWorldInit()
{
  //position.y = world->GetHeight(position);
  Window::GrabMouse(true);
}

void Player::Render() const
{
  return;
  playerModel.model->BindShader(nullptr, &camera);
  playerModel.model->PreRender();
  playerModel.model->Render(nullptr, &camera);
  playerModel.model->PostRender();
  playerModel.model->UnbindShader(nullptr, &camera);
}
void Player::Update(float timeElapsed)
{
  static float t = 0.0f;
  // Temporary pause
  if(!playerControl.mouseGrab)
    return;
  //Move(timeElapsed);
  //Gravity(timeElapsed);
  //CheckGroundCollision(timeElapsed)
  playerMovement.velocity = {0.0f, 0.0f, 0.0f};
  if(playerControl.left) playerMovement.velocity.x = -1;
  if(playerControl.right) playerMovement.velocity.x = 1;
  if(playerControl.up) playerMovement.velocity.y = 1;
  if(playerControl.down) playerMovement.velocity.y = -1;
  if(playerControl.forward) playerMovement.velocity.z = -1;
  if(playerControl.backward) playerMovement.velocity.z = 1;
  if(playerControl.mouseDown)
  {
    t += timeElapsed;
    if(t>= 1/60.0f)
    {
      if(playerControl.mouseButton == GLFW_MOUSE_BUTTON_1)
      {
        std::map<size_t, float> removed = world->RemoveVoxels();
        for(auto it = removed.begin(); it != removed.end(); it++)
        {
          inventory.AddMaterials(it->first, it->second);
        }
      }
      if(playerControl.mouseButton == GLFW_MOUSE_BUTTON_2)
      {
        float used = world->PlaceVoxels(inventory.GetCurrentMaterial(), inventory.GetCurrentMaterialAmount());
        inventory.RemoveMaterials(inventory.GetCurrentMaterial().id, used);
      }
      t -= 1/60.0f;
    } 
  }

  Vec2 movement{playerMovement.velocity.x, playerMovement.velocity.z};
  movement.Rotate(camera.GetYaw());
  playerMovement.velocity = {movement.x, playerMovement.velocity.y, movement.y};
  position += playerMovement.velocity * timeElapsed * 10;

  playerModel.model->SetPosition(position);
  playerModel.model->Update(timeElapsed);

  camera.SetPosition(position*1);
  camera.Update(timeElapsed);
}

void Player::Move(float timeElapsed)
{
  static int i = 0;
  i++;
  // Calculate acceleration 
  Vec2 acc = {0,0};
  float accel = 50.0f;
  if(playerControl.forward) acc.y -= 1;
  if(playerControl.backward) acc.y += 1;
  if(playerControl.left) acc.x -= 1;
  if(playerControl.right) acc.x += 1;
  if(acc.LengthSQ() > 0.0f)
  {
    acc.Rotate(camera.GetYaw()).Normalize();
    acc *= accel;
  }
  Vec3<float> acceleration = Vec3(acc.x, 0.0f, acc.y);
  playerMovement.velocity.x *= 0.999; // Friction
  playerMovement.velocity.z *= 0.999; // Friction

  playerMovement.velocity += acceleration * timeElapsed;

  if(playerMovement.velocity.Length() > playerMovement.maxVelocity)
  {
    playerMovement.velocity = playerMovement.velocity.Normalize() * playerMovement.maxVelocity;
  }

  position += playerMovement.velocity * timeElapsed;
}

void Player::Gravity(float timeElapsed)
{
  float y = 0.0f;
  //float y = world->GetHeight(position);
  float grav = 0.0f;
  if(position.y > y)
    grav = -10.0f;
  else
  {
    position.y = y;
    playerMovement.velocity.y = 0.0f;
  }

  playerMovement.velocity.y += grav * timeElapsed;
  position.y += playerMovement.velocity.y * timeElapsed;

  if(position.y < y + 0.1f)
    playerMovement.onGround = true;
  else
    playerMovement.onGround = false;
}

void Player::OnEvent(Event& event)
{
  if(EVENT_IS_TYPE(event,EventType::KEY_PRESS))
  {
    KeyPressEvent& e = (KeyPressEvent&)event;

    if(e.GetButton() == GLFW_KEY_W)
      playerControl.forward = true;
    if(e.GetButton() == GLFW_KEY_S)
      playerControl.backward = true;
    if(e.GetButton() == GLFW_KEY_A)
      playerControl.left = true;
    if(e.GetButton() == GLFW_KEY_D)
      playerControl.right = true;
    if(playerMovement.onGround && e.GetButton() == GLFW_KEY_SPACE)
    {
      playerMovement.velocity.y += 5.0f;
    }
    if(e.GetButton() == GLFW_KEY_SPACE)
      playerControl.up = true;
    if(e.GetButton() == GLFW_KEY_LEFT_SHIFT)
      playerControl.down = true;
    if(e.GetButton() == GLFW_KEY_ESCAPE)
    {
      playerControl.mouseGrab = !playerControl.mouseGrab;
      Window::GrabMouse(playerControl.mouseGrab);
    }
  }
  else if(EVENT_IS_TYPE(event,EventType::KEY_RELEASE))
  {
    KeyReleaseEvent& e = (KeyReleaseEvent&)event;

    if(e.GetButton() == GLFW_KEY_W)
      playerControl.forward = false;
    if(e.GetButton() == GLFW_KEY_S)
      playerControl.backward = false;
    if(e.GetButton() == GLFW_KEY_A)
      playerControl.left = false;
    if(e.GetButton() == GLFW_KEY_D)
      playerControl.right = false;
    if(e.GetButton() == GLFW_KEY_SPACE)
      playerControl.up = false;
    if(e.GetButton() == GLFW_KEY_LEFT_SHIFT)
      playerControl.down = false;
  }
  else if(EVENT_IS_TYPE(event, EventType::MOUSE_MOVE))
  {
    camera.OnEvent(event);
  }
  else if(EVENT_IS_TYPE(event, EventType::MOUSE_PRESS))
  {
    MousePressEvent& e = (MousePressEvent&)event;
    playerControl.mouseDown = true;
    playerControl.mouseButton = e.GetButton();
  }
  else if(EVENT_IS_TYPE(event, EventType::MOUSE_RELEASE))
  {
    MouseReleaseEvent& e = (MouseReleaseEvent&)event;
    playerControl.mouseDown = false;
    playerControl.mouseButton = e.GetButton();
  }
  else if(EVENT_IS_TYPE(event, EventType::MOUSE_SCROLL))
  {
    MouseScrollEvent& e = (MouseScrollEvent&)event;
    inventory.ChangeMaterial(-e.GetScrollVertical());
  }
}

const PlayerCamera& Player::GetCamera() const
{
  return camera;
}
