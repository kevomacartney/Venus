//
// Created by Kelvin Macartney on 15/04/2020.
//

#ifndef VENUS_EXECUTIONFLAGS_H
#define VENUS_EXECUTIONFLAGS_H
enum EventExecutionFlag {
    /**
     * Default flag, meaning the invocation will be passed to the task scheduler and scheduled
     * on a background thread
     */
    EEF_Default = 0,

    /**
     * Specifies that the invocation to be invoked by the calling thread
     */
    EEF_Caller,

    /**
     * Specifies that the invocation will be queued on the cored thread
     * @note It is slower than normal queue because it requires additional synchronization.
     */
    EEF_CoreThreadQueue,
};
#endif //VENUS_EXECUTIONFLAGS_H
