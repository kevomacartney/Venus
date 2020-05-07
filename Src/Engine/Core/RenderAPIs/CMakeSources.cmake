set(RENDERAPI_DIR "RenderAPIs")

set(ENGINE_RENDERAPIs_INC
        "${RENDERAPI_DIR}/renderWindow.h"
        "${RENDERAPI_DIR}/renderApi.h"
        )

set(ENGINE_RENDERAPIs_SRC
        "${RENDERAPI_DIR}/renderWindow.cpp"
        "${RENDERAPI_DIR}/renderApi.cpp"
        )

set(ENGINE_RENDERAPI_CORE_SRC
        ${ENGINE_RENDERAPIs_INC}
        ${ENGINE_RENDERAPIs_SRC}
        )