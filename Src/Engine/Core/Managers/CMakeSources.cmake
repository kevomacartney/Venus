set(MANAGERS_DIR "Managers")

set(ENGINE_MANAGER_INC
        "${MANAGERS_DIR}/renderWindowManager.h"
        )

set(ENGINE_MANAGER_SRC
        "${MANAGERS_DIR}/renderWindowManager.cpp"
        )

set(ENGINE_MANAGERS_CORE_SRC
        ${ENGINE_MANAGER_INC}
        ${ENGINE_MANAGER_SRC}
        )