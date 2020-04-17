set(CORETHREAD_DIR "CoreThread")

set(ENGINE_CORETHREAD_INC # include directories
        "${CORETHREAD_DIR}/queuedCommand.h"
        "${CORETHREAD_DIR}/commandQueueBase.h"
        "${CORETHREAD_DIR}/commandQueue.h"
        "${CORETHREAD_DIR}/coreThread.h"
        )

set(ENGINE_CORETHREAD_SRC # source directories
        "${CORETHREAD_DIR}/commandQueueBase.cpp"
#        "${CORETHREAD_DIR}/commandQueue.cpp"
        "${CORETHREAD_DIR}/coreThread.cpp"
        )

set(ENGINE_CORETHREAD_CORE_SRC
        ${ENGINE_CORETHREAD_INC}
        ${ENGINE_CORETHREAD_SRC}
        )