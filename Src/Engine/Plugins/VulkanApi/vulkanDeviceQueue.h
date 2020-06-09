//
// Created by Kelvin Macartney on 08/06/2020.
//

#ifndef VENUS_VULKANDEVICEQUEUE_H
#define VENUS_VULKANDEVICEQUEUE_H

#include <vulkan/vulkan.h>
#include <common.h>

namespace Venus::Plugins::Vulkan {
    /**
     * Represents a queue which belongs to a vulkan device,
     */
    class VulkanDeviceQueue {
    public:
        /**
         * Constructor
         * @param queueFamilyFlags The GPU Queue family index
         * @param vulkanDevice The vulkan device it belongs to
         */
        VulkanDeviceQueue(VkQueue queue, uint32_t queueFamilyFlags);

        /** Initialises the device Queue*/
        void ignition();

    private:

        uint32_t _queueFamilyFlags;
        VkQueue _queue;
    };
};


#endif //VENUS_VULKANDEVICEQUEUE_H
