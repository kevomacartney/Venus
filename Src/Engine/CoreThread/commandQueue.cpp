//
// Created by Kelvin Macartney on 27/04/2020.
//

#include "commandQueue.h"
#include "coreThread.h"

namespace Venus::Core {
    CommandQueueSync::CommandQueueSync(ThreadId id)
            : _threadId(id) {}

    bool CommandQueueSync::isValidThread() {
        return true;
    }

    LockGuard CommandQueueSync::lock() {
        return LockGuard{Lock(_commandQueueMutex)};
    }

    CommandQueueUnSynced::CommandQueueUnSynced(ThreadId id)
            : _threadId(id) {}

    bool CommandQueueUnSynced::isValidThread() {
        return this->_threadId == THREAD_CURRENT_ID || THREAD_CURRENT_ID == CoreThread::instance()->getThreadId();
    }

    LockGuard CommandQueueUnSynced::lock() {
        return LockGuard();
    }
}