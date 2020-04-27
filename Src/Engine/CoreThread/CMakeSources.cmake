
set(ENGINE_CORETHREAD_INC # include directories
        "queuedCommand.h"
        "commandQueue.h"
        "commandQueueBase.h"
        "coreThreadQueueFlag.h"
        "coreThread.h"
        )

set(ENGINE_CORETHREAD_SRC # source directories
        "commandQueueBase.cpp"
        "commandQueue.cpp"
        "coreThread.cpp"
        )

set(ENGINE_CORETHREAD_CORE_SRC
        ${ENGINE_CORETHREAD_INC}
        ${ENGINE_CORETHREAD_SRC}
        )