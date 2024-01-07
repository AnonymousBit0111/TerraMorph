#include "Graphics/Camera.h"
#include "Core/Logging.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <string>
using namespace TerraMorph::Graphics;

Camera::Camera(glm::vec2 size, glm::vec3 position)
    : m_size(size), m_pos(position) {

  m_target = glm::vec3(1, 1, 1);

  m_yaw = -90;

  m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front.y = sin(glm::radians(m_pitch));
  m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_front));
  glm::vec3 cameraUp = glm::cross(m_front, cameraRight);

  m_view = glm::lookAt(m_pos, m_pos + m_front, cameraUp);
  float fov = glm::radians(60.0f);
  float aspect = m_size.x / m_size.y;
  float nearClip = 0.1f;
  float farClip = 10000.0f;
  m_proj = glm::perspective(fov, aspect, nearClip, farClip);
  m_viewProj = m_proj * m_view;
}
void Camera::updateMatrices() {

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_front));
  glm::vec3 cameraUp = glm::cross(m_front, cameraRight);

  m_view = glm::lookAt(m_pos, m_pos + m_front, cameraUp);
  float fov = glm::radians(60.0f);
  float aspect = m_size.x / m_size.y;

  float nearClip = 0.1f;
  float farClip = 10000.0f;
  m_proj = glm::perspective(fov, aspect, nearClip, farClip);
  m_viewProj = m_proj * m_view;
}

void Camera::move(glm::vec3 movement) {

  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  m_right = glm::normalize(glm::cross(m_front, worldUp));
  m_up = glm::cross(m_right, m_front);

  // turns out i needed to transform all of the movement coords into the cameras
  // axes
  m_pos += m_right * movement.x;
  m_pos += m_up * movement.y;
  m_pos += m_front * movement.z;

  updateMatrices();
}

void Camera::setPosition(glm::vec3 position) {

  this->m_pos = position;

  updateMatrices();
}
void Camera::lookAt(glm::vec3 target) {
  m_target = target;

  m_front = glm::normalize(m_pos - m_target);
  updateMatrices();
}

void Camera::calculateDirection(glm::vec2 mouseMovement) {
  float sensitivity = 0.5f;
  mouseMovement.x *= sensitivity;
  mouseMovement.y *= sensitivity;

  // so you cant look backwards by looking up
  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;

  m_yaw += mouseMovement.x;
  m_yaw = fmod(m_yaw, 360);
  m_pitch += mouseMovement.y;

  m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front.y = sin(glm::radians(m_pitch));
  m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front = glm::normalize(m_front);

  updateMatrices();
}