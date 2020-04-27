//
// Created by Kelvin Macartney on 14/04/2020.
//

#ifndef VENUS_CORETHREADQUEUEFLAG_H
#define VENUS_CORETHREADQUEUEFLAG_H

/** Flags that control how is a command submitted to the command queue. */
enum CoreThreadQueueFlag {
    /**
     * Default flag, meaning the commands will be added to the per-thread queue and only begin executing after
     * submit() has been called.
     */
    CTQF_Default = 0,
    /**
     * Specifies that the queued command should be executed on the internal queue. Internal queue doesn't require
     * a separate CoreThread::submit() call, and the queued command is instead immediately visible to the core thread.
     * @note It is slower than normal queue because it requires additional synchronization.
     */
    CTQF_InternalQueue = 1 << 0,
    /**
     * If true, the method will block until the command finishes executing on the core thread. Only relevant for the
     * internal queue commands since contents of the normal queue won't be submitted to the core thread until the
     * CoreThread::submit() call.
     */
    CTQF_BlockUntilComplete = 1 << 1
};


#endif //VENUS_CORETHREADQUEUEFLAG_H
