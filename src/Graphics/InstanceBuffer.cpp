#include "Graphics/InstanceBuffer.h"
#include "Core/Globals.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include <cstring>
#include <vector>
using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

InstanceBuffer::InstanceBuffer(glm::uint32_t size) {

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.size = size;
  bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
  bufferInfo.sharingMode = vk::SharingMode::eExclusive;

  vk::resultCheck(
      g_vkContext->device.createBuffer(&bufferInfo, nullptr, &m_handle),
      "error creating vkbuffer");

  vk::MemoryRequirements requirements =
      g_vkContext->device.getBufferMemoryRequirements(m_handle);
  vk::MemoryAllocateInfo memInfo{};
  memInfo.allocationSize = requirements.size;
  memInfo.memoryTypeIndex =
      VKContext::findMemoryType(requirements.memoryTypeBits,
                                vk::MemoryPropertyFlagBits::eHostVisible |
                                    vk::MemoryPropertyFlagBits::eHostCoherent);

  vk::resultCheck(
      g_vkContext->device.allocateMemory(&memInfo, nullptr, &m_memory), "");

  g_vkContext->device.bindBufferMemory(m_handle, m_memory, 0);

  // Note find out if it is needed to unmap this memory
  m_data = g_vkContext->device.mapMemory(m_memory, 0, bufferInfo.size);
}
void InstanceBuffer::update(std::vector<glm::mat4> &matrices) {
  assert(matrices.size() * sizeof(matrices[0]) <= m_size);
  memcpy(m_data, matrices.data(), matrices.size() * sizeof(matrices[0]));
}

void InstanceBuffer::resize(glm::uint32_t size) {

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.size = size;
  bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
  bufferInfo.sharingMode = vk::SharingMode::eExclusive;

  vk::Buffer newBuffer;
  vk::resultCheck(
      g_vkContext->device.createBuffer(&bufferInfo, nullptr, &newBuffer),
      "Failed to create new buffer during resize");

  vk::MemoryRequirements requirements =
      g_vkContext->device.getBufferMemoryRequirements(newBuffer);
  vk::MemoryAllocateInfo memInfo{};
  memInfo.allocationSize = requirements.size;
  memInfo.memoryTypeIndex =
      VKContext::findMemoryType(requirements.memoryTypeBits,
                                vk::MemoryPropertyFlagBits::eHostVisible |
                                    vk::MemoryPropertyFlagBits::eHostCoherent);

  vk::DeviceMemory newMemory;
  vk::resultCheck(
      g_vkContext->device.allocateMemory(&memInfo, nullptr, &newMemory),
      "Failed to allocate memory for new buffer during resize");

  


  if (m_data) {
    g_vkContext->device.unmapMemory(m_memory);
  }

  // Clean up the old buffer and memory
  g_vkContext->device.freeMemory(m_memory);
  g_vkContext->device.destroyBuffer(m_handle);

  // Update instance buffer members to the new buffer and memory
  m_handle = newBuffer;
  m_memory = newMemory;
  m_size = size;

  // Map new memory
  m_data = g_vkContext->device.mapMemory(m_memory, 0, size);

}

InstanceBuffer::~InstanceBuffer() {
  g_vkContext->device.unmapMemory(m_memory);
  g_vkContext->device.freeMemory(m_memory);
  g_vkContext->device.destroyBuffer(m_handle);
}