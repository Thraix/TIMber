#include "Player.h"

#include <Greet.h>
#include "World.h"

using namespace Greet;

TPCamera Player::CreateCamera()
{
  float distance = 2.0f;
  return TPCamera(Greet::Mat4::ProjectionMatrix(Window::GetAspect(), 90.0, 0.01, 1000.0), Greet::Vec3<float>(), distance, 0.4, 0, distance, distance,0.0,1.0);
}

Player::Player(World* world, const Greet::Vec3<float>& position)
  : camera{CreateCamera()}, position{position}, world{world}
{
  Window::GrabMouse(true);
}

void Player::OnWorldInit()
{
  position.y = world->GetHeight(position);
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

  Vec2 movement{playerMovement.velocity.x, playerMovement.velocity.z};
  movement.Rotate(camera.GetRotation() + 90);
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
    acc.Rotate(camera.GetRotation() + 90).Normalize();
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
  float y = world->GetHeight(position);
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
    MouseMoveEvent& e = (MouseMoveEvent&)event;
    camera.Rotate(e.GetDeltaPosition()*0.2);
  }
}

const TPCamera& Player::GetCamera() const
{
  return camera;
}
