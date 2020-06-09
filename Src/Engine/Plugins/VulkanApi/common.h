//
// Created by Kelvin Macartney on 08/06/2020.
//

#ifndef VENUS_COMMON_H
#define VENUS_COMMON_H

/** Allows us to limit the maximum number of GPU queues*/
#define VENUS_GPU_MAX_QUEUE_COUNT 2u

/** Types of GPU queues. */
enum GpuQueueType
{
    /**
     * Queue used for rendering. Allows the use of draw commands, but also all commands supported by compute
     * or upload buffers.
     */
    GQT_GRAPHICS,
    /** Discrete queue used for compute operations. Allows the use of dispatch and upload commands. */
    GQT_COMPUTE,
    /** Queue used for memory transfer operations only. No rendering or compute dispatch allowed. */
    GQT_UPLOAD,
    /** Number of queues */
    GQT_COUNT
};
#endif //VENUS_COMMON_H
