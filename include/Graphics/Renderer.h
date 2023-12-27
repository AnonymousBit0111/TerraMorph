#pragma once

#include "Core/Vertex.h"
#include "Graphics/Camera.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/PipelineLayout.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include "Graphics/VertexBuffer.h"
#include "glm/fwd.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <SDL_video.h>
#include <memory>
namespace TerraMorph {
namespace Graphics {
class Renderer {

private:
  std::unique_ptr<Swapchain> m_swapChain;
  std::unique_ptr<RenderPass> m_renderPass;
  std::unique_ptr<PipelineLayout> m_pipelineLayout;
  std::unique_ptr<Pipeline> m_pipeline;
  std::unique_ptr<VertexBuffer> m_vertexBuffer;
  std::unique_ptr<IndexBuffer> m_indexBuffer;

  // TODO , render more than 1 frame ahead
  vk::Semaphore m_renderFinished;
  vk::Semaphore m_imageAvailable;
  vk::Fence m_frameInflight;

  vk::CommandPool m_commandPool;
  vk::CommandBuffer m_commandBuffer;

  vk::DescriptorPool imGuiDescriptorPool;

  SDL_Window *p_window;

  std::vector<Core::PosColourVertex> testVertices;
  std::vector<glm::uint32_t> testIndices;
  glm::mat4 rotation;
  Camera *camera;

  void initImGui();

public:
  Renderer(SDL_Window *win, Camera *cam);
  void beginFrame();
  void drawFrame();
  void recordCommandBuffer(int imageIndex);
  void setData(glm::mat4 rotData); // temporary function to set the value of the
                                   // rotation matrix which is a pushconstant
  glm::mat4 getData() { return rotation; }
  ~Renderer();
};

} // namespace Graphics
} // namespace TerraMorph