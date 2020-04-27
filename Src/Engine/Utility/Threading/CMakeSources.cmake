
set(ENGINE_THREADING_INC
        Threading/threading.h
        Threading/pooledThread.h
        Threading/threadPool.h
        Threading/asyncResult.h
        Threading/asyncResultImpl.h
        Threading/asyncWaitHandle.h
        Threading/asyncWaitHandleImpl.h
        Threading/TaskScheduler/taskScheduler.h
        Threading/venusThread.h
        Threading/TaskScheduler/task.h
        )

set(ENGINE_THREADING_SRC
        Threading/TaskScheduler/taskScheduler.cpp
        Threading/pooledThread.cpp
        Threading/threadPool.cpp
        Threading/asyncResult.cpp
        Threading/asyncWaitHandle.cpp
        )
