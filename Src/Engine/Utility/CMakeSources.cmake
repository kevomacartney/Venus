include(Time/CMakeSources.cmake)
include(Error/CMakeSources.cmake)
include(Events/CMakeSources.cmake)
include(Threading/CMakeSources.cmake)
include(Helpers/CMakeSources.cmake)
include(DataStructures/CMakeSources.cmake)

set(ENGINE_UTILITY_FREE_ROAMING_INC # files with folders
        PlatformDefines.h
        genericObject.h
        Module.h
        StringOps.h
        )

set(ENGINE_UTILITY_COMBINED_INC # include directories
        ${ENGINE_TIME_INC}
        ${ENGINE_ERROR_INC}
        ${ENGINE_THREADING_INC}
        ${ENGINE_HELPERS_INC}
        ${ENGINE_VENUSEVENT_INC}
        ${ENGINE_DATA_STRUCTURES_INC}
        ${ENGINE_UTILITY_FREE_ROAMING_INC}
        )

set(ENGINE_UTILITY_COMBINED_SRC # source directories
        ${ENGINE_TIME_SRC}
        ${ENGINE_ERROR_SRC}
        ${ENGINE_THREADING_SRC}
        ${ENGINE_HELPERS_SRC}
        ${ENGINE_VENUSEVENT_SRC}
        ${ENGINE_DATA_STRUCTURES_SRC}
        )

set(ENGINE_UTILITY_SRC
        ${ENGINE_UTILITY_COMBINED_INC}
        ${ENGINE_UTILITY_COMBINED_SRC}
        )