
set(VULKAN_PLUGIN_INC # include directories
        "vulkanUtility.h"
        )

set(VULKAN_PLUGIN_SRC # source directories
        "vulkanUtility.cpp"
        )

set(VULKAN_API_PLUGIN_SRC
        ${VULKAN_PLUGIN_INC}
        ${VULKAN_PLUGIN_SRC}
        )