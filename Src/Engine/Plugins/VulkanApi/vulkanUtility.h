//
// Created by Kelvin Macartney on 05/03/2020.
//

#ifndef VENUS_VULKANUTILITY_H
#define VENUS_VULKANUTILITY_H

#include <vulkan/vulkan.h>
#include <loggingFactory.h>
#include <Module.h>
#include <memory>

namespace Venus::Plugins::Vulkan {

    /**
     * Extensions configuration for the the window system integration being used
     */
    struct WSIExtensions {
        /** Extensions Count */
        uint32_t ExtensionCount = 0;
        /** The extensions*/
        const char** Extensions;
    };

    class VulkanUtility : public Module<VulkanUtility> {
    public:
        VulkanUtility(WSIExtensions extensions);

        /** Initialises the module*/
        void ignition() override;

        /** Shuts down the  module and deallocates all registered devices */
        void shutdown() override;

    private:
        /** Creates a local logger using logger factory */
        void _createLogger();

        /** Initialises vulkan and connects it to our application */
        void _initialiseVulkan();

        /** Creates a vulkan instance */
        void _createVulkanInstance();

        /** Creates a new VkInstanceCreateInfo with extensions */
        VkInstanceCreateInfo _makeVkInstanceCreateInfo() const;

        /** Enables validation layers for vulakn*/
        static void _enableVulkanValidationLayers(VkInstanceCreateInfo &createInfo) ;

        /** Throws an exception if the Wsi Extensions are not supported by vulkan */
        void _throwIfUnsupportedWsiExtensions() const;

        static std::vector<VkExtensionProperties> _getVulkanExtensions();

        /** Returns true if the extension is supported by Vulkan and false otherwise */
        static bool _isSupportedVulkanExtension(const char* extension);

        std::shared_ptr<spdlog::logger> _logger;
        WSIExtensions _extensions;

        VkInstance _vulkanInstance;
    };
}


#endif //VENUS_VULKANUTILITY_H
