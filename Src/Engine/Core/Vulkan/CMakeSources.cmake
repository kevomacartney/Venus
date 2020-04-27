set(VULKAN_DIR "Vulkan")

set(ENGINE_VULKAN_INC # include directories
        "${VULKAN_DIR}/vulkanManager.h"
        )

set(ENGINE_VULKAN_SRC # source directories
        "${VULKAN_DIR}/vulkanManager.cpp"
        )

set(ENGINE_VULKAN_CORE_SRC
        ${ENGINE_VULKAN_INC}
        ${ENGINE_VULKAN_SRC}
        )