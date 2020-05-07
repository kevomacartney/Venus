//
// Created by Kelvin Macartney on 05/03/2020.
//

#include "vulkanUtility.h"

namespace Venus::Plugins::Vulkan {

    VulkanUtility::VulkanUtility(WSIExtensions extensions) :
            _extensions(extensions) {}

    void VulkanUtility::ignition() {
        Module::ignition();
        this->_createLogger();
        this->_initialiseVulkan();

        this->_logger->info("Initialised Vulkan utility");
    }

    void VulkanUtility::_createLogger() {
        this->_logger = Factories::LoggingFactory::CreateLogger_Safe(Factories::LoggerType::ColouredStdOut,
                                                                     "Venus::Plugins::VulkanUtility");
    }

    void VulkanUtility::_initialiseVulkan() {
        this->_throwIfUnsupportedWsiExtensions();
        this->_createVulkanInstance();
    }

    void VulkanUtility::_throwIfUnsupportedWsiExtensions() const {
        for (int i = 0; i < this->_extensions.ExtensionCount; ++i) {
            const char *ext = this->_extensions.Extensions[i];

            if (!VulkanUtility::_isSupportedVulkanExtension(ext)) {
                _logger->error(
                        "Vulkan extension '{}' from the RenderSurface's WSI is is not supported by the current installation",
                        ext);

                VENUS_EXCEPT(InternalErrorException,
                             "Vulkan Installation does not support one of the required WSI extensions")
            }

            _logger->info("Vulkan Extension '{}' supported by installation", ext);
        }
    }

    std::vector<VkExtensionProperties> VulkanUtility::_getVulkanExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    bool VulkanUtility::_isSupportedVulkanExtension(const char *extension) {
        static std::vector<VkExtensionProperties> availableExtensions;

        if (availableExtensions.empty())
            availableExtensions = VulkanUtility::_getVulkanExtensions();

        for (auto ext : availableExtensions) {
            if (strcmp(ext.extensionName, extension) != 0)
                return true;
        }

        return false;
    }

    void VulkanUtility::_createVulkanInstance() {
        VkInstanceCreateInfo createInfo = _makeVkInstanceCreateInfo();
        _enableVulkanValidationLayers(createInfo);

        VkResult result = vkCreateInstance(&createInfo, nullptr, &this->_vulkanInstance);

        if (result != VK_SUCCESS) {
            _logger->error("Could not create Vulkan Instance result status {}", result);
            VENUS_EXCEPT(InternalErrorException, "Could NOT create a Vulkan Instance")
        }
    }

    VkInstanceCreateInfo VulkanUtility::_makeVkInstanceCreateInfo() const {
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = _extensions.ExtensionCount;
        createInfo.ppEnabledExtensionNames = _extensions.Extensions;
        return createInfo;
    }

    void VulkanUtility::_enableVulkanValidationLayers(VkInstanceCreateInfo &createInfo) {
#if defined(VULKANUTIL_ENABLE_VALIDATION)
        static const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#endif
    }

    void VulkanUtility::shutdown() {
        Module::shutdown();
        vkDestroyInstance(this->_vulkanInstance, nullptr);

        this->_logger->info("Vulkan utility shutting down");
    }
}