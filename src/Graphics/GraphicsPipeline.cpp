#include "Graphics/GraphicsPipeline.h"
#include "Core/FileHandler.h"
#include "Core/Globals.h"
#include "Core/Vertex.h"
#include "vulkan/vulkan_structs.hpp"
#include <vector>

using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core;

vk::ShaderModule createShaderModule(const std::vector<char> &code) {
  vk::ShaderModule module;
  vk::ShaderModuleCreateInfo createInfo{};
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  vk::Result res =
      g_vkContext->device.createShaderModule(&createInfo, nullptr, &module);

  assert(res == vk::Result::eSuccess);
  return module;
}

vk::VertexInputBindingDescription getBindingDescription() {
  vk::VertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(PosColourVertex);
  bindingDescription.inputRate = vk::VertexInputRate::eVertex;

  return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
  vk::VertexInputAttributeDescription Pos{};
  Pos.binding = 0;
  Pos.location = 0;
  Pos.format = vk::Format::eR32G32B32Sfloat; // x,y,z
  Pos.offset = offsetof(PosColourVertex, x);

  vk::VertexInputAttributeDescription Colour{};
  Colour.binding = 0;
  Colour.location = 1;
  Colour.format = vk::Format::eR32G32B32A32Sfloat; // r,g,b,a
  Colour.offset = offsetof(PosColourVertex, r);

  return {Pos, Colour};
}

Pipeline::Pipeline(const std::string &fsPath, const std::string &vsPath,
                   vk::RenderPass rp, vk::PipelineLayout layout,
                   vk::Extent2D viewportSize) {

  auto vertShaderCode = FileHandler::readFilebin(vsPath);
  auto fragShaderCode = FileHandler::readFilebin(fsPath);
  vk::ShaderModule fragModule = createShaderModule(fragShaderCode);
  vk::ShaderModule vertModule = createShaderModule(vertShaderCode);

  vk::PipelineShaderStageCreateInfo vertStageCreateInfo{};
  vertStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
  vertStageCreateInfo.module = vertModule;
  vertStageCreateInfo.pName = "main";

  vk::PipelineShaderStageCreateInfo fragStageCreateInfo{};
  fragStageCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
  fragStageCreateInfo.module = fragModule;
  fragStageCreateInfo.pName = "main";

  vk::PipelineShaderStageCreateInfo shaderStages[] = {vertStageCreateInfo,
                                                      fragStageCreateInfo};

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

  m_bindingDesc = getBindingDescription();

  m_attributeDescriptions = getAttributeDescriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &m_bindingDesc;

  vertexInputInfo.vertexAttributeDescriptionCount =
      m_attributeDescriptions.size();
  vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  vk::Viewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = viewportSize.width;
  viewport.height = viewportSize.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vk::Rect2D scissor{};
  scissor.offset = vk::Offset2D{0, 0};
  scissor.extent = viewportSize;

  vk::PipelineViewportStateCreateInfo viewportState{};
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  vk::PipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = vk::PolygonMode::eFill;
  rasterizer.lineWidth = 1.0f;

  rasterizer.cullMode = vk::CullModeFlagBits::eBack;
  rasterizer.frontFace = vk::FrontFace::eClockwise;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  vk::PipelineMultisampleStateCreateInfo multisampling{};

  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
  colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
  colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;             // Optional
  colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;  // Optional
  colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
  colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;             // Optional

  vk::PipelineColorBlendStateCreateInfo colourBlendInfo{};
  colourBlendInfo.pAttachments = &colorBlendAttachment;
  colourBlendInfo.attachmentCount = 1;
  colourBlendInfo.logicOp = vk::LogicOp::eCopy;
  colourBlendInfo.logicOpEnable = vk::Bool32(false);

  colourBlendInfo.blendConstants[0] = 1;
  colourBlendInfo.blendConstants[1] = 1;
  colourBlendInfo.blendConstants[2] = 1;
  colourBlendInfo.blendConstants[3] = 1;

  vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};

  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStages;

  pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pRasterizationState = &rasterizer;
  pipelineCreateInfo.pMultisampleState = &multisampling;
  pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
  pipelineCreateInfo.pColorBlendState = &colourBlendInfo;
  // pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.layout = layout;

  pipelineCreateInfo.renderPass = rp;

  vk::Result res = g_vkContext->device.createGraphicsPipelines(
      VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_handle);

  vk::resultCheck(res, "failed");

  g_vkContext->device.destroyShaderModule(fragModule);
  g_vkContext->device.destroyShaderModule(vertModule);
}

Pipeline::~Pipeline() { g_vkContext->device.destroyPipeline(m_handle); }