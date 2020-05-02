#ifndef DECODE_TASK_H
#define DECODE_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <mqueue.h>

bool decode_task_start(void);

mqd_t decode_task_init_message_queue(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // DECODE_TASK_H
