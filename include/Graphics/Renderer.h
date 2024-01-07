#pragma once

#include "Core/Vertex.h"
#include "Graphics/Camera.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/InstanceBuffer.h"
#include "Graphics/PipelineLayout.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include "Graphics/VertexBuffer.h"
#include "Terrain/Cube.h"
#include "glm/fwd.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <SDL_video.h>
#include <memory>
#include <vector>
namespace TerraMorph {
namespace Graphics {
class Renderer {

private:
  std::unique_ptr<Swapchain> m_swapChain;
  std::unique_ptr<RenderPass> m_renderPass;
  std::unique_ptr<PipelineLayout> m_pipelineLayout;
  std::unique_ptr<Pipeline> m_pipeline;
  std::unique_ptr<VertexBuffer> m_vertexBuffer;
  std::unique_ptr<InstanceBuffer> m_instanceBuffer;
  std::unique_ptr<IndexBuffer> m_indexBuffer;

  // TODO , render more than 1 frame ahead
  vk::Semaphore m_renderFinished;
  vk::Semaphore m_imageAvailable;
  vk::Fence m_frameInflight;

  vk::CommandPool m_commandPool;
  vk::CommandBuffer m_commandBuffer;

  vk::DescriptorPool imGuiDescriptorPool;

  SDL_Window *p_window;

  std::vector<Core::PosColourVertex> cubeVertices;
  std::vector<glm::uint32_t> cubeIndices;
  std::vector<InstanceData> instances;

  int m_maxInstanceCount = 0;
  int m_instanceCount = 0;
  glm::mat4 rotation;
  Camera *camera;

  void initImGui();

public:
  Renderer(SDL_Window *win, Camera *cam, int instanceCount);
  void beginFrame();
  void drawFrame();
  void addInstance(glm::mat4 model,glm::vec4 colour);
  void recordCommandBuffer(int imageIndex);

  std::vector<InstanceData> *getInstanceModels() {
    return &instances;
  } // so that i can change the models externally
  ~Renderer();
};

} // namespace Graphics
} // namespace TerraMorph