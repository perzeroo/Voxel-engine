#include "engine/Camera.hpp"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "engine/Transform.hpp"
#include "engine/Input.hpp"
#include <iostream>

namespace Engine::CameraSystem{

glm::mat4 update(entt::registry &registry, float aspectRatio) {
  auto view = registry.view<Camera, const Transform>();
  glm::mat4 viewProjection = glm::mat4(1.0f);
  for (auto entity : view) {
    auto &transform = view.get<const Transform>(entity);
    if (!transform.active) continue;

    auto &camera = view.get<Camera>(entity);

    float rollRad = glm::radians(transform.roll);
    
    camera.front = transform.front();

    camera.right = transform.right();

    if (transform.roll != 0.0) {
      glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), rollRad, camera.front);
      camera.up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(glm::cross(camera.right, camera.front), 0.0f))); 
    } else {
      camera.up = transform.up();
    }

    glm::vec3 target = transform.position + camera.front;

    camera.view = glm::lookAt(transform.position, target, camera.up);
    camera.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    viewProjection = camera.projection * camera.view;
    break;
  }
  return viewProjection;
}
}

namespace Engine::CameraControllerSystem {
void update(entt::registry &registry, float dt) {
  auto view = registry.view<Camera, Transform, const CameraController>();

  for (auto entity : view) {
    auto &transform = view.get<Transform>(entity);
    if (!transform.active)
      continue;

    auto &camera = view.get<Camera>(entity);
    auto &cameraController = view.get<const CameraController>(entity);

    glm::vec2 mouseDelta = Engine::Input::instance().getMouseDelta();
    transform.yaw += mouseDelta.x * cameraController.mouseSensitivity * dt * 100.0f;
    transform.yaw = glm::mod(transform.yaw + 180.0f, 360.0f);
    if (transform.yaw < 0.0f)
      transform.yaw += 360.0f;
    transform.yaw -= 180.0f;

    transform.pitch -= mouseDelta.y * cameraController.mouseSensitivity * dt * 100.0f;

    transform.pitch = glm::clamp(transform.pitch, -89.0f, 89.0f);


    const bool* keystates = SDL_GetKeyboardState(NULL);
    glm::vec3 wishDir = glm::vec3(0.0f);
    if (keystates[SDL_SCANCODE_W]) {
      wishDir += camera.front;
    }
    if (keystates[SDL_SCANCODE_S]) {
      wishDir -= camera.front;
    }
    if (keystates[SDL_SCANCODE_A]) {
      wishDir -= camera.right;
    }
    if (keystates[SDL_SCANCODE_D]) {
      wishDir += camera.right;
    }
    // wishDir = glm::normalize(wishDir);
    if (keystates[SDL_SCANCODE_SPACE]) {
      wishDir += glm::vec3(0.0, 1.0, 0.0);
    }
    if (keystates[SDL_SCANCODE_LCTRL]) {
      wishDir -= glm::vec3(0.0, 1.0, 0.0);
    }
    transform.position += wishDir * cameraController.movementSpeed * dt * 100.0f;
  }
}
}
