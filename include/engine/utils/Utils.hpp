#pragma once
#include "SDL3/SDL_mouse.h"
#include "engine/Camera.hpp"
#include "engine/Transform.hpp"

namespace Engine::Utils {
inline Transform defaultTransform() {
  Transform t;
  t.active = true;
  t.position = {0.0f, 0.0f, 0.0f};
  t.yaw = -90.0f;
  t.pitch = 0.0f;
  t.roll = 0.0f;
  t.scale = {1.0f, 1.0f, 1.0f};
  return t;
}
inline Camera defaultCamera(float fov = 60.0f) {
  Camera cam;
  cam.up = {0.0f, 1.0f, 0.0f};
  cam.front = {0.0f, 0.0f, -1.0f};
  cam.right = {1.0f, 0.0f, 0.0f};
  cam.fov = fov;
  cam.view = glm::mat4(1.0f);
  cam.projection = glm::mat4(1.0f);
  return cam;
}
inline entt::entity cameraWithControllerEntity(entt::registry& registry) {
  entt::entity entity = registry.create();
  Engine::Camera camComponent = Engine::Utils::defaultCamera(60.0f);
  Engine::Transform camTransform = Engine::Utils::defaultTransform();
  registry.emplace<Engine::Camera>(entity, camComponent);
  registry.emplace<Engine::Transform>(entity, camTransform);
  registry.emplace<Engine::CameraController>(entity, 0.25f, 0.5f);
  return entity;
}

}

