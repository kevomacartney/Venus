set(THREADING_DIR "Threading")

set(ENGINE_THREADING_INC
        "${THREADING_DIR}/engineThread.h"
        )

set(ENGINE_THREADING_SRC
        "${THREADING_DIR}/engineThread.cpp"
        )

set(ENGINE_THREADING_CORE_SRC
        ${ENGINE_THREADING_INC}
        ${ENGINE_THREADING_SRC}
        )