#include <stdbool.h>
#include <mqueue.h>

bool decode_task_start(void);

mqd_t decode_task_init_message_queue(void);
