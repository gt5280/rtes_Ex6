
#ifndef DECODE_H
#define DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void decode_init(void);

void decode_run(bool light_on);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // DECODE_H
