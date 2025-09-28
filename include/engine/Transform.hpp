#pragma once

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"

namespace Engine {
struct Transform {
  bool active;
  glm::vec3 position;
  float yaw, pitch, roll;
  glm::vec3 scale;
  glm::vec3 front() const {
    float yawRad = glm::radians(this->yaw);
    float pitchRad = glm::radians(this->pitch);
    
    return glm::normalize(glm::vec3{
      cos(yawRad) * cos(pitchRad),
      sin(pitchRad),
      sin(yawRad) * cos(pitchRad)
    });
  }
  glm::vec3 right() const {
    return glm::normalize(glm::cross(this->front(), glm::vec3(0.0f, 1.0f, 0.0f)));
  }

  glm::vec3 up() const {
    return glm::normalize(glm::cross(this->right(), this->front()));
  }
};
}
