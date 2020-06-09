//
// Created by Kelvin Macartney on 08/06/2020.
//

#include "vulkanDeviceQueue.h"

namespace Venus::Plugins::Vulkan {
    VulkanDeviceQueue::VulkanDeviceQueue(VkQueue queue, uint32_t queueFamilyFlags)
            : _queueFamilyFlags(queueFamilyFlags),
              _queue(queue) {}

    void VulkanDeviceQueue::ignition() {

    }
}
