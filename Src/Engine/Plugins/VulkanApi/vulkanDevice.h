//
// Created by Kelvin Macartney on 21/05/2020.
//

#ifndef VENUS_VULKANDEVICE_H
#define VENUS_VULKANDEVICE_H

#include "vulkanQueueInfo.h"

#include <Error/venusExceptions.h>
#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <loggingFactory.h>
#include <magic_enum.hpp>

#include <vector>
#include <string>

namespace Venus::Plugins::Vulkan {

    /**
     * Represents a single GPU device which can be used by vulkan
     */
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

        /** Does object de-initialisation and memory cleanup*/
        void shutdown();

        /** Gets the name of the device */
        std::string getName();

        /** Gets the physical device instance*/
        const VkPhysicalDevice &getPhysicalDevice() { return this->_physicalDevice; }

        /** Gets the physical device properties. */
        const VkPhysicalDeviceProperties &getDeviceProperties() { return this->_physicalDeviceProperties; }

        /** Gets the device's features */
        const VkPhysicalDeviceFeatures &getDeviceFeatures() { return this->_physicalDeviceFeatures; }

        /** Gets the device's memory properties */
        const VkPhysicalDeviceMemoryProperties &getDeviceMemoryProperties() { return this->_physicalMemoryProperties; }

        /** Calculates and returns the performance score. */
        uint32_t getDeviceScore();

        ~VulkanDevice();

    private:
        void _setDeviceProperties();

        void _setDeviceFeatures();

        void _setPhysicalMemoryProperties();

        std::vector<VulkanQueueInfo> _initialiseDeviceQueues();

        uint32_t _getQueueFamilyQueueCount();

        static VkDeviceQueueCreateInfo
        _initialiseVkQueueCreateInfo(std::pair<uint32_t, VkQueueFamilyProperties> indexFamilyPair);

        void _createLogicalDevice(const std::vector<VulkanQueueInfo> &);

        VkDeviceCreateInfo _initialiseCreateLogicalDeviceInfo(const std::vector<VulkanQueueInfo> &queueInfos);

        static VkDeviceQueueCreateInfo *
        _extractQueueCreateInfosAsArrPtr(const std::vector<VulkanQueueInfo> &queueInfos);

        std::vector<const char *> _getSupportedLogicalDeviceExtensions();

        static inline void _deleteQueueCreateInfoArray(VkDeviceQueueCreateInfo *createInfoPtr);

        void _initialiseVulkanQueueInstances(std::vector<VulkanQueueInfo> &queueInfos);

        /** Registers a new VulkanDeviceQueue into the vulkanQueueInfo */
        void _registerNewVulkanDeviceQueue(VulkanQueueInfo &vulkanQueueInfo, uint32_t queueIndex);

        VkPhysicalDeviceFeatures _physicalDeviceFeatures;
        VkPhysicalDeviceProperties _physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties _physicalMemoryProperties;

        VkPhysicalDevice _physicalDevice{};
        VkDevice _logicalDevice{};

        std::vector<VulkanQueueInfo> _queueInfos;
        std::shared_ptr<spdlog::logger> _logger;
        const uint32_t _deviceIdx{0};

    };
}

#endif //VENUS_VULKANDEVICE_H
