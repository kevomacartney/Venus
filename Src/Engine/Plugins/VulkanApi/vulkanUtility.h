//
// Created by Kelvin Macartney on 05/03/2020.
//

#ifndef VENUS_VULKANUTILITY_H
#define VENUS_VULKANUTILITY_H

#include <vulkan/vulkan.h>

#include <loggingFactory.h>
#include <Module.h>
#include <vulkanDevice.h>

#include <memory>

namespace Venus::Plugins::Vulkan {

    /**
     * Extensions configuration for the the window system integration being used
     */
    struct WSIExtensions {
        /** The extensions*/
        std::vector<const char *> Extensions;
    };

    /**
     * Provides utility functionality for vulkan API in a clean api
     */
    class VulkanUtility : public Module<VulkanUtility> {
    public:
        VulkanUtility(WSIExtensions extensions);

        /** Initialises the module*/
        void ignition() override;

        /** Shuts down the  module and de allocates all registered devices */
        void shutdown() override;

        /** Returns the vulkan device count */
        uint32_t getDeviceCount();

        /** Returns the default physical device */
        std::shared_ptr<VulkanDevice> getDefaultDevice();

        ~VulkanUtility();
    private:

        /** Creates a local logger using logger factory */
        void _createLogger();

        /** Initialises vulkan and connects it to our application */
        void _initialiseVulkan();

        /** Configures the applications vulkan instance */
        void _initialiseVulkanInstance();

        /**
         * Create an instance of vkInstance device
         * @param createInfo The createInfo instance to use
         */
        void _createVulkanInstance(VkInstanceCreateInfo &createInfo);

        /** Adds vulkan debug callback to extensions list*/
        void _registerVulkanDebugCallbackExtension();

        /** Configures the vulkan instance debug message callback*/
        static VkDebugUtilsMessengerCreateInfoEXT _createDebugMessengerCreateInfo();

        /** Configures the vulkan debug messenger extensions */
        void _configureDebugMessengerExtension();

        /**
         * Creates the debug util messenger
         * @param instance The VkInstance to create messenger for
         * @param createInfo The CreateInfo structure to be used to create the Messenger
         * @return
         */
        VkResult _createDebugUtilsMessengerEXT(VkInstance instance,
                                               const VkDebugUtilsMessengerCreateInfoEXT *createInfo);

        /** Creates a new VkInstanceCreateInfo with extensions */
        [[nodiscard]] VkInstanceCreateInfo _makeVkInstanceCreateInfo() const;

        /**
         * Enables validation layers for vulkan
         * @param createInfo The CreateInfo structure to be used to create VulkanInstance
         */
        static void _enableVulkanValidationLayers(VkInstanceCreateInfo &createInfo);

        /** Throws an exception if the Wsi Extensions are not supported by vulkan */
        void _throwIfUnsupportedWsiExtensions() const;

        /** Returns a vector of all vulkan extensions supported by the platform */
        static std::vector<VkExtensionProperties> _getVulkanExtensions();

        /** Returns true if the extension is supported by Vulkan and false otherwise */
        static bool _isSupportedVulkanExtension(const char *extension);

        /** Method which will hooked up to vulkan to received messages */
        static VKAPI_ATTR VkBool32 VKAPI_CALL
        _vulkanDebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                    void *pUserData) {
            spdlog::warn("[Vulkan] {}", pCallbackData->pMessage);
            return VK_FALSE;
        }

        /** Initialises all vulkan compatible graphics cards on the system */
        void _initialiseVulkanGraphicsCards();

        /**
         * Registers the physical devices into the utility
         * @param A vector of Vulkan physical devices
         */
        void _registerVulkanDevices(const std::vector<VkPhysicalDevice> &devices);

        /**
         * Creates VulkanDevices from the physical devices
         * @param physicalDevices A vector of vulkan physical device instances
         * @return A vector of initialised VulkanDevice instances
         */
        std::vector<std::shared_ptr<VulkanDevice>>
        _createVulkanDevices(const std::vector<VkPhysicalDevice> &physicalDevices);

        /**
         * Finds the default graphical device to used
         * @note Finds the first discrete physical device otherwise the first device in the list
         * @return A default device
         */
        std::shared_ptr<VulkanDevice> _findDefaultDevice();

        std::vector<std::shared_ptr<VulkanDevice>> _vulkanDevices;
        std::shared_ptr<VulkanDevice> _defaultDevice;
        std::shared_ptr<spdlog::logger> _logger;
        WSIExtensions _extensions;

        VkInstance _vulkanInstance;
        VkDebugUtilsMessengerEXT _debugMessenger;
    };
}


#endif //VENUS_VULKANUTILITY_H
