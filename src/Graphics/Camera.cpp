#include "Graphics/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace TerraMorph::Graphics;

Camera::Camera(glm::vec2 size, glm::vec3 position)
    : m_size(size), m_pos(position) {

  m_view =
      glm::translate(glm::mat4(1.0f), -glm::vec3(m_pos.x, m_pos.y, m_pos.z));
  float fov = glm::radians(45.0f);
  float aspect = m_size.x / m_size.y;
  float nearClip = 0.1f;
  float farClip = 1000.0f;
  m_proj = glm::perspective(fov, aspect, nearClip, farClip);
  m_viewProj = m_proj * m_view;
}
void Camera::updateMatrices() {
  m_view =
      glm::translate(glm::mat4(1.0f), -glm::vec3(m_pos.x, m_pos.y, m_pos.z));
  float fov = glm::radians(45.0f);
  float aspect = m_size.x / m_size.y;
  float nearClip = 0.1f;
  float farClip = 1000.0f;
  m_proj = glm::perspective(fov, aspect, nearClip, farClip);
  m_viewProj = m_proj * m_view;
}

void Camera::move(glm::vec3 movement) {

  this->m_pos += movement;

  updateMatrices();
}

void Camera::setPosition(glm::vec3 position) {

  this->m_pos = position;

  updateMatrices();
}
