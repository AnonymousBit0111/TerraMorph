#pragma once

#include "glm/fwd.hpp"
#include "glm/glm.hpp"
namespace TerraMorph {
namespace Graphics {
class Camera {
private:
  glm::mat4 m_view;
  glm::mat4 m_proj;

  glm::mat4 m_viewProj; // so that we arent recalculating it unnecessarily
  glm::vec3 m_pos;
  glm::vec2 m_size;
//   glm::vec3 m_cameraTarget;
//   glm::vec3 m_cameraDirection;
//   glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

  void updateMatrices();

public:
  Camera(glm::vec2 size, glm::vec3 position);

  glm::vec3 getPosition() const { return m_pos; }
  void setPosition(glm::vec3 position);
  void move(glm::vec3 movement);
  glm::mat4 getViewProj() const { return m_viewProj; }
  ~Camera() {}
};

} // namespace Graphics
} // namespace TerraMorph
