
#include "Graphics/VkContext.h"
#include "glm/fwd.hpp"

#include "Core/Globals.h"
#include "Core/Logging.h"
#include "Graphics/IndexBuffer.h"
#include "vulkan/vulkan_structs.hpp"

using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

IndexBuffer::IndexBuffer(glm::uint32_t size) {

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.size = size;
  bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
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
void IndexBuffer::update(const std::vector<glm::uint32_t> &indices) {
  assert(indices.size() * sizeof(indices[0]) <= m_size);
  memcpy(m_data, indices.data(), indices.size() * sizeof(indices[0]));
}

IndexBuffer::~IndexBuffer() {
  g_vkContext->device.freeMemory(m_memory);
  g_vkContext->device.destroyBuffer(m_handle);
}