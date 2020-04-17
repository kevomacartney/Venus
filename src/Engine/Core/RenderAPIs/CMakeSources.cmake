set(RENDERER_DIR "RendererAPI")

set(ENGINE_RENDERER_INC
        "${RENDERER_DIR}/renderer.h"
        )

set(ENGINE_RENDERER_SRC
        "${RENDERER_DIR}/renderer.cpp"
        )

set(ENGINE_RENDER_CORE_SRC
        ${ENGINE_RENDERER_INC}
        ${ENGINE_RENDERER_SRC}
        )