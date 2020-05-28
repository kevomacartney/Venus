//
// Created by Kelvin Macartney on 21/05/2020.
//

#include "vulkanDevice.h"

namespace Venus::Plugins::Vulkan {
    VulkanDevice::VulkanDevice(VkPhysicalDevice device, uint32_t index) :
            _deviceIdx(index),
            _physicalDevice(device) {}

    void VulkanDevice::ignition() {

    }
}
