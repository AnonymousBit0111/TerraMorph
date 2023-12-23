#include "Graphics/PipelineLayout.h"
#include "Core/Globals.h"
using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core;

PipelineLayout::PipelineLayout() {

  assert(g_vkContext->device.createPipelineLayout(
             &m_createInfo, nullptr, &m_handle) == vk::Result::eSuccess ||
         "Failed To create pipeline layout");
}

void PipelineLayout::recreate() {
  m_createInfo.pPushConstantRanges = m_pushConstantRanges.data();
  m_createInfo.pushConstantRangeCount = m_pushConstantRanges.size();
  m_createInfo.pSetLayouts = m_descriptorSetLayouts.data();
  m_createInfo.setLayoutCount = m_descriptorSetLayouts.size();
  if (m_handle) {
    g_vkContext->device.destroyPipelineLayout(m_handle);
  }
  assert(g_vkContext->device.createPipelineLayout(
             &m_createInfo, nullptr, &m_handle) == vk::Result::eSuccess ||
         "Failed To create pipeline layout");
}

void PipelineLayout::addDescriptorSetLayout(
    const vk::DescriptorSetLayout &descriptorSetLayout) {
  m_descriptorSetLayouts.push_back(descriptorSetLayout);
  recreate();
}

void PipelineLayout::addPushConstantRange(
    const vk::PushConstantRange &pushConstantRange) {
  m_pushConstantRanges.push_back(pushConstantRange);
  recreate();
}

PipelineLayout::~PipelineLayout() {

  for (auto &i : m_descriptorSetLayouts) {
    g_vkContext->device.destroyDescriptorSetLayout(i);
  }
  g_vkContext->device.destroyPipelineLayout(m_handle);
}