#include "PlayerCamera.h"

#include <event/MouseEvent.h>
#include <event/KeyEvent.h>
#include <graphics/Window.h>

using namespace Greet;

PlayerCamera::~PlayerCamera()
{

}

void PlayerCamera::Update(float timeElapsed)
{
  Vec2 vel = Vec2(0);
  float yVel = 0.0f;
  float speed = 10.0f;
  if(movement.forward) vel.y -= 1.0f;
  if(movement.left) vel.x -= 1.0f;
  if(movement.backward) vel.y += 1.0f;
  if(movement.right) vel.x += 1.0f;
  if(movement.up) yVel += speed;
  if(movement.down) yVel -= speed;
  if(vel.LengthSQ() > 0.0f)
  {
    vel.Normalize().Rotate(rotation.y);
    vel *= speed;
  }
  velocity.x = vel.x;
  velocity.y = yVel;
  velocity.z = vel.y;
  position += velocity * timeElapsed;
  viewMatrix = Mat4::ViewMatrix(position, rotation);
}

void PlayerCamera::OnEvent(Event& event)
{
  Camera::OnEvent(event);
  if(EVENT_IS_TYPE(event, EventType::MOUSE_MOVE))
  {
    float sens = 40.0f;
    MouseMoveEvent& e = (MouseMoveEvent&)event;
    Vec2 delta = e.GetDeltaPosition() * Vec2(Window::GetAspect(), 1);
    rotation.x -= delta.y * sens;
    rotation.y += delta.x * sens;
  }
  else if(EVENT_IS_TYPE(event, EventType::KEY_PRESS))
  {
    KeyPressEvent& e = (KeyPressEvent&)event;
    if(e.GetButton() == GLFW_KEY_W) movement.forward = true;
    else if(e.GetButton() == GLFW_KEY_A) movement.left = true;
    else if(e.GetButton() == GLFW_KEY_S) movement.backward = true;
    else if(e.GetButton() == GLFW_KEY_D) movement.right = true;
    else if(e.GetButton() == GLFW_KEY_SPACE) movement.up = true;
    else if(e.GetButton() == GLFW_KEY_LEFT_SHIFT) movement.down = true;
  }
  else if(EVENT_IS_TYPE(event, EventType::KEY_RELEASE))
  {
    KeyReleaseEvent& e = (KeyReleaseEvent&)event;
    if(e.GetButton() == GLFW_KEY_W) movement.forward = false;
    else if(e.GetButton() == GLFW_KEY_A) movement.left = false;
    else if(e.GetButton() == GLFW_KEY_S) movement.backward = false;
    else if(e.GetButton() == GLFW_KEY_D) movement.right = false;
    else if(e.GetButton() == GLFW_KEY_SPACE) movement.up = false;
    else if(e.GetButton() == GLFW_KEY_LEFT_SHIFT) movement.down = false;
  }
}
