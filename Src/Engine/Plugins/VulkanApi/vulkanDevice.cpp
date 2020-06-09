//
// Created by Kelvin Macartney on 21/05/2020.
//

#include "vulkanDevice.h"

#include <utility>
#include "common.h"

namespace Venus::Plugins::Vulkan {
    VulkanDevice::VulkanDevice(VkPhysicalDevice device, uint32_t index) :
            _deviceIdx(index),
            _physicalDevice(device) {}

    void VulkanDevice::ignition() {
        this->_setDeviceProperties();
        this->_setDeviceFeatures();
        this->_setPhysicalMemoryProperties();
        this->_logger = Factories::LoggingFactory::CreateLogger_Safe(Factories::LoggerType::ColouredStdOut,
                                                                     this->getName());

        this->_queueInfos = this->_initialiseDeviceQueues();

        this->_createLogicalDevice(this->_queueInfos);
        this->_initialiseVulkanQueueInstances(this->_queueInfos);
    }

    void VulkanDevice::shutdown() {
        vkDestroyDevice(this->_logicalDevice, nullptr);
        this->_logicalDevice = VK_NULL_HANDLE;
    }

    void VulkanDevice::_setDeviceProperties() {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(this->_physicalDevice, &deviceProperties);

        this->_physicalDeviceProperties = deviceProperties;
    }

    void VulkanDevice::_setDeviceFeatures() {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(this->_physicalDevice, &deviceFeatures);

        this->_physicalDeviceFeatures = deviceFeatures;
    }

    void VulkanDevice::_setPhysicalMemoryProperties() {
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(this->_physicalDevice, &deviceMemoryProperties);

        this->_physicalMemoryProperties = deviceMemoryProperties;
    }

    std::string VulkanDevice::getName() {
        return this->_physicalDeviceProperties.deviceName;
    }

    uint32_t VulkanDevice::getDeviceScore() {
        return 0;
    }

    std::vector<VulkanQueueInfo> VulkanDevice::_initialiseDeviceQueues() {
        auto createInfos = std::vector<VulkanQueueInfo>();

        uint32_t queueFamilyCount = _getQueueFamilyQueueCount();
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->_physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            auto createQueueInfo = _initialiseVkQueueCreateInfo(std::make_pair(i, queueFamilies[i]));
            auto queueInfo = VulkanQueueInfo(createQueueInfo, queueFamilies[i].queueFlags);
            createInfos.push_back(queueInfo);
        }

        return createInfos;
    }

    uint32_t VulkanDevice::_getQueueFamilyQueueCount() {
        uint32_t queueFamilyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(this->_physicalDevice, &queueFamilyCount, nullptr);
        return queueFamilyCount;
    }

    VkDeviceQueueCreateInfo
    VulkanDevice::_initialiseVkQueueCreateInfo(std::pair<uint32_t, VkQueueFamilyProperties> indexFamilyPair) {
        uint32_t queueCount = std::min(indexFamilyPair.second.queueCount, VENUS_GPU_MAX_QUEUE_COUNT);
        auto priority = new float;
        *priority = 0.f;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indexFamilyPair.first;
        queueCreateInfo.pQueuePriorities = priority;
        queueCreateInfo.flags = 0;
        queueCreateInfo.queueCount = queueCount;

        return queueCreateInfo;
    }


    void VulkanDevice::_createLogicalDevice(const std::vector<VulkanQueueInfo> &queueInfos) {
        auto createDeviceInfo = this->_initialiseCreateLogicalDeviceInfo(queueInfos);
        auto createResult = vkCreateDevice(this->_physicalDevice, &createDeviceInfo, nullptr, &this->_logicalDevice);

        if (createResult != VK_SUCCESS) {
            _logger->error("Vulkan logical device creation failed with error, '{}'",
                           magic_enum::enum_name(createResult));
            VENUS_EXCEPT(InternalErrorException, "There was an error while creating a logical device")
        }

        // Free the array we allocated in _extractQueueCreateInfosAsArrPtr()
        VulkanDevice::_deleteQueueCreateInfoArray(
                const_cast<VkDeviceQueueCreateInfo *>(createDeviceInfo.pQueueCreateInfos));
    }

    VkDeviceCreateInfo
    VulkanDevice::_initialiseCreateLogicalDeviceInfo(const std::vector<VulkanQueueInfo> &queueInfos) {
        // By making it a pointer, it ensures we can retain the data. Stack variable will does not work
        // because other elements will be freed when we exit the function since we don't reference them (we only ref the 1st)
        auto queueCreateInfos = VulkanDevice::_extractQueueCreateInfosAsArrPtr(queueInfos);
        auto supportedExtensions = this->_getSupportedLogicalDeviceExtensions();

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos;
        createInfo.queueCreateInfoCount = queueInfos.size();
        createInfo.ppEnabledExtensionNames = supportedExtensions.data();
        createInfo.enabledExtensionCount = supportedExtensions.size();
        createInfo.pEnabledFeatures = &this->_physicalDeviceFeatures;
        createInfo.flags = 0;

        return createInfo;
    }

    VkDeviceQueueCreateInfo *
    VulkanDevice::_extractQueueCreateInfosAsArrPtr(const std::vector<VulkanQueueInfo> &queueInfos) {
        auto *createInfos = new VkDeviceQueueCreateInfo[queueInfos.size()];

        for (int i = 0; i < queueInfos.size(); ++i) {
            createInfos[i] = queueInfos[i].QueueCreateInfo;
        }

        return createInfos;
    }

    std::vector<const char *> VulkanDevice::_getSupportedLogicalDeviceExtensions() {
        return std::vector<const char *>(); // TODO, retrieve supported extensions
    }

    void VulkanDevice::_deleteQueueCreateInfoArray(VkDeviceQueueCreateInfo *createInfoPtr) {
        delete[] createInfoPtr;
    }

    void VulkanDevice::_initialiseVulkanQueueInstances(std::vector<VulkanQueueInfo> &queueInfos) {
        for (auto &queueInfo : queueInfos) {
            for (int i = 0; i < queueInfo.QueueCreateInfo.queueCount; ++i) {
                this->_registerNewVulkanDeviceQueue(queueInfo, i);
            }
        }
    }

    void VulkanDevice::_registerNewVulkanDeviceQueue(VulkanQueueInfo &vulkanQueueInfo, uint32_t queueIndex) {
        VkQueue queue;
        vkGetDeviceQueue(this->_logicalDevice, vulkanQueueInfo.QueueCreateInfo.queueFamilyIndex, queueIndex, &queue);

        auto deviceQueue = std::make_shared<VulkanDeviceQueue>(queue, vulkanQueueInfo.QueueFamilyFlag);
        vulkanQueueInfo.Queues.push_back(deviceQueue);
    }

    VulkanDevice::~VulkanDevice() {
        if (this->_logicalDevice != VK_NULL_HANDLE)
            vkDestroyDevice(this->_logicalDevice, nullptr);
    }
}
