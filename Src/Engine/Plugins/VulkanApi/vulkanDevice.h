//
// Created by Kelvin Macartney on 21/05/2020.
//

#ifndef VENUS_VULKANDEVICE_H
#define VENUS_VULKANDEVICE_H

#include <vulkan/vulkan.h>

#include <string>

namespace Venus::Plugins::Vulkan {
    class VulkanDevice {
    public:
        /**
         * Constructor
         * @param device The vulkan physical device instance
         * @param index The device's index in the list of GPUs
         */
        VulkanDevice(VkPhysicalDevice device, uint32_t index);

        /** Initialises the device */
        void ignition();

        /** Gets the name of the device */
        std::string getName(){
            return "Intel";
        }
    private:
        VkPhysicalDevice _physicalDevice;
        const uint32_t _deviceIdx;
    };
}

#endif //VENUS_VULKANDEVICE_H
