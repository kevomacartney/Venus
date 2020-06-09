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
        this->_initialiseVulkanGraphicsCards();

        this->_logger->info("Initialised Vulkan utility");
    }

    void VulkanUtility::_createLogger() {
        this->_logger = Factories::LoggingFactory::CreateLogger_Safe(Factories::LoggerType::ColouredStdOut,
                                                                     "Venus::Plugins::VulkanUtility");
    }

    void VulkanUtility::_initialiseVulkan() {
        this->_throwIfUnsupportedWsiExtensions();
        this->_initialiseVulkanInstance();
    }

    void VulkanUtility::_throwIfUnsupportedWsiExtensions() const {
        for (auto ext : this->_extensions.Extensions) {
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

    std::vector<VkExtensionProperties> VulkanUtility::_getVulkanExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    void VulkanUtility::_initialiseVulkanInstance() {
        this->_registerVulkanDebugCallbackExtension();

        VkInstanceCreateInfo createInfo = _makeVkInstanceCreateInfo();
        VulkanUtility::_enableVulkanValidationLayers(createInfo);

        this->_createVulkanInstance(createInfo);
        this->_configureDebugMessengerExtension();
    }

    void VulkanUtility::_createVulkanInstance(VkInstanceCreateInfo &createInfo) {
        VkResult result = vkCreateInstance(&createInfo, nullptr, &this->_vulkanInstance);

        if (result != VK_SUCCESS) {
            _logger->error("Could not create Vulkan Instance result status {}", result);
            VENUS_EXCEPT(InternalErrorException, "Could NOT create a Vulkan Instance")
        }
    }

    VkInstanceCreateInfo VulkanUtility::_makeVkInstanceCreateInfo() const {
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<int>(this->_extensions.Extensions.size());
        createInfo.ppEnabledExtensionNames = this->_extensions.Extensions.data();

        return createInfo;
    }

    void VulkanUtility::_registerVulkanDebugCallbackExtension() {
#if defined(VULKANUTIL_ENABLE_VALIDATION)
        this->_extensions.Extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
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

    void VulkanUtility::_configureDebugMessengerExtension() {
        auto createInfo = VulkanUtility::_createDebugMessengerCreateInfo();
        this->_createDebugUtilsMessengerEXT(this->_vulkanInstance, &createInfo);
    }

    VkDebugUtilsMessengerCreateInfoEXT VulkanUtility::_createDebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VulkanUtility::_vulkanDebugMessageCallback;
        createInfo.pUserData = nullptr; // Optional

        return createInfo;
    }

    VkResult VulkanUtility::_createDebugUtilsMessengerEXT(VkInstance instance,
                                                          const VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, createInfo, nullptr, &this->_debugMessenger);
        } else {
            VENUS_EXCEPT(InternalErrorException, "Vulkan debug messenger extension not available");
        }
    }

    void VulkanUtility::shutdown() {
        Module::shutdown();
        this->_logger->info("Vulkan utility shutting down");

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->_vulkanInstance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if(func != nullptr)
            func(this->_vulkanInstance, this->_debugMessenger, nullptr);

        for(const auto& vulkanDevice : this->_vulkanDevices){
            vulkanDevice->shutdown();
        }

        vkDestroyInstance(this->_vulkanInstance, nullptr);
    }

    void VulkanUtility::_initialiseVulkanGraphicsCards() {
        uint32_t deviceCount = this->getDeviceCount();

        if (deviceCount < 1) VENUS_EXCEPT(InternalErrorException, "Could not find a GPU that support Vulkan")

        std::vector<VkPhysicalDevice> vkDevices(deviceCount);
        vkEnumeratePhysicalDevices(this->_vulkanInstance, &deviceCount, vkDevices.data());

        this->_registerVulkanDevices(vkDevices);
        this->_defaultDevice = this->_findDefaultDevice();
    }

    uint32_t VulkanUtility::getDeviceCount() {
        uint32_t deviceCount;

        vkEnumeratePhysicalDevices(this->_vulkanInstance, &deviceCount, nullptr);
        return deviceCount;
    }

    std::shared_ptr<VulkanDevice> VulkanUtility::getDefaultDevice() {
        return this->_defaultDevice;
    }

    void VulkanUtility::_registerVulkanDevices(const std::vector<VkPhysicalDevice> &devices) {
        if (devices.empty()) VENUS_EXCEPT(InternalErrorException, "Could not find a graphics card that supports Vulkan")

        auto deviceInstances = this->_createVulkanDevices(devices);

        this->_vulkanDevices.insert(this->_vulkanDevices.end(), deviceInstances.begin(), deviceInstances.end());
    }

    std::vector<std::shared_ptr<VulkanDevice>>
    VulkanUtility::_createVulkanDevices(const std::vector<VkPhysicalDevice> &physicalDevices) {
        std::vector<std::shared_ptr<VulkanDevice>> devices;

        for (int i = 0; i < physicalDevices.size(); i++) {
            auto device = std::make_shared<VulkanDevice>(physicalDevices[i], i);
            device->ignition();
            devices.push_back(device);

            this->_logger->info("Created a VulkanDevice for Physical device '{}'", device->getName());
        }

        return devices;
    }

    std::shared_ptr<VulkanDevice> VulkanUtility::_findDefaultDevice() {
        for (auto device : this->_vulkanDevices) {
            if (device->getDeviceProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return device;
        }

        return _vulkanDevices.at(0);
    }

    VulkanUtility::~VulkanUtility() {

    }
}
