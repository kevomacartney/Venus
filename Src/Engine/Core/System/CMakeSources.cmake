set(SYSTEM_DIR "System")

set(ENGINE_SYSTEM_INC
        "${SYSTEM_DIR}/system.h"
        )

set(ENGINE_SYSTEM_SRC
        "${SYSTEM_DIR}/system.cpp"
        )

set(ENGINE_SYSTEM_CORE_SRC
        ${ENGINE_SYSTEM_INC}
        ${ENGINE_SYSTEM_SRC}
        )