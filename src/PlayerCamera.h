#pragma once

#include <graphics/cameras/Camera.h>

class PlayerCamera : public Greet::Camera
{
  private:
    struct Movement
    {
      bool forward = false;
      bool backward = false;
      bool up = false;
      bool down = false;
      bool right = false;
      bool left = false;
    };

    Greet::Vec3<float> position;
    Greet::Vec3<float> rotation;
    Greet::Mat4 viewMatrix;
    Greet::Vec3<float> velocity;
    Movement movement;
  public:
    PlayerCamera(float fov, float near, float far, const Greet::Vec3<float>& position, float pitch, float yaw)
      : Camera{fov, near, far}, position{position}, rotation{pitch, yaw, 0},
      viewMatrix{Greet::Mat4::ViewMatrix(position, rotation)} {}

    virtual const Greet::Mat4& GetViewMatrix() const { return viewMatrix; }

    virtual ~PlayerCamera();

    void Update(float timeElapsed);
    void OnEvent(Greet::Event& event);

    void SetPitch(float pitch) { rotation.x = pitch; }
    void SetYaw(float yaw) { rotation.y = yaw; }
    void SetPosition(const Greet::Vec3<float>& position) { this->position = position; }

    float GetPitch() const { return rotation.x; }
    float GetYaw() const { return rotation.y; }
    const Greet::Vec3<float>& GetPosition() const { return position; }
};

