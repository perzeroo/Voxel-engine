#pragma once
#include "glm/glm.hpp"
#include "entt/entt.hpp"
#include "engine/Transform.hpp"


namespace Engine {
struct Camera {
  glm::vec3 up;
  glm::vec3 front;
  glm::vec3 right;
  float fov;
  glm::mat4 view;
  glm::mat4 projection;
};
struct CameraController {
  float movementSpeed;
  float mouseSensitivity;
};

}
namespace Engine::CameraSystem {
glm::mat4 update(entt::registry&, float);
}
namespace Engine::CameraControllerSystem {
void update(entt::registry&, float dt);
}

