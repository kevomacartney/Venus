//
// Created by Kelvin Macartney on 09/06/2020.
//

#ifndef VENUS_VULKANQUEUEINFO_H
#define VENUS_VULKANQUEUEINFO_H

#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanDeviceQueue.h"

namespace Venus::Plugins::Vulkan {

    /** Structure containing a queue's attributes */
    struct VulkanQueueInfo {
    public:
        explicit VulkanQueueInfo() = default;

        VulkanQueueInfo(VkDeviceQueueCreateInfo createInfo, uint32_t queueFamilyFlags)
                : QueueCreateInfo(createInfo), QueueFamilyFlag(queueFamilyFlags) {}

        VkDeviceQueueCreateInfo QueueCreateInfo{};

        std::vector<std::shared_ptr<VulkanDeviceQueue>> Queues;

        uint32_t QueueFamilyFlag{};
    };
}
#endif //VENUS_VULKANQUEUEINFO_H
