#include "Graphics/GraphicsPipeline.h"
#include "Core/FileHandler.h"
#include "Core/Globals.h"
#include "Core/Vertex.h"
#include "Graphics/InstanceBuffer.h"
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

vk::VertexInputBindingDescription getInstanceBindingDescription() {
  vk::VertexInputBindingDescription bindingDescription{};
  bindingDescription.binding =
      1; // I am storing instance data in a different buffer
  bindingDescription.stride =
      sizeof(InstanceData); // this may need to change if more variables are
                            // required for each instance
  bindingDescription.inputRate = vk::VertexInputRate::eInstance;

  return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {

  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
  vk::VertexInputAttributeDescription Pos{};
  Pos.binding = 0;
  Pos.location = 0;
  Pos.format = vk::Format::eR32G32B32Sfloat; // x,y,z
  Pos.offset = offsetof(PosColourVertex, pos);

  vk::VertexInputAttributeDescription Colour{};
  Colour.binding = 0;
  Colour.location = 1;
  Colour.format = vk::Format::eR32G32B32A32Sfloat; // r,g,b,a
  Colour.offset = offsetof(PosColourVertex, colour);

  attributeDescriptions.push_back(Pos);
  attributeDescriptions.push_back(Colour);

  for (int i = 0; i < 5; ++i) // 4 for the matrix , 1 for colour
  {

    vk::VertexInputAttributeDescription desc{};
    desc.binding = 1;      // same as binding desc
    desc.location = i + 2; // Different location for each row
    desc.format =
        vk::Format::eR32G32B32A32Sfloat; // 4 floats per row of the matrix
    desc.offset = (sizeof(float) * 4 * i);
    attributeDescriptions.push_back(desc);
  }

  return attributeDescriptions;
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

  m_bindingDescs = {getBindingDescription(), getInstanceBindingDescription()};

  m_attributeDescriptions = getAttributeDescriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 2;
  vertexInputInfo.pVertexBindingDescriptions = m_bindingDescs.data();

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

  vk::PipelineDepthStencilStateCreateInfo depthStencil{};

  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = vk::CompareOp::eLess;

  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front.failOp = vk::StencilOp::eKeep;
  depthStencil.front.passOp = vk::StencilOp::eIncrementAndWrap;
  depthStencil.front.depthFailOp = vk::StencilOp::eKeep;
  depthStencil.front.compareOp = vk::CompareOp::eAlways;
  depthStencil.front.compareMask = 0xFF;
  depthStencil.front.writeMask = 0xFF;
  depthStencil.front.reference = 1;

  depthStencil.back.failOp = vk::StencilOp::eKeep;
  depthStencil.back.passOp = vk::StencilOp::eDecrementAndWrap;
  depthStencil.back.depthFailOp = vk::StencilOp::eKeep;
  depthStencil.back.compareOp = vk::CompareOp::eAlways;
  depthStencil.back.compareMask = 0xFF;
  depthStencil.back.writeMask = 0xFF;
  depthStencil.back.reference = 1;

  vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};

  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStages;

  pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pRasterizationState = &rasterizer;
  pipelineCreateInfo.pMultisampleState = &multisampling;
  pipelineCreateInfo.pDepthStencilState = &depthStencil;
  pipelineCreateInfo.pColorBlendState = &colourBlendInfo;
  pipelineCreateInfo.layout = layout;

  pipelineCreateInfo.renderPass = rp;

  vk::Result res = g_vkContext->device.createGraphicsPipelines(
      VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_handle);

  vk::resultCheck(res, "failed");

  g_vkContext->device.destroyShaderModule(fragModule);
  g_vkContext->device.destroyShaderModule(vertModule);
}

Pipeline::~Pipeline() { g_vkContext->device.destroyPipeline(m_handle); }