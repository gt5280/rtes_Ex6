
#include "decode.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>

int main()
{
    struct timeval initial_time_val;
    struct timeval final_time_val;

    decode_init();

    gettimeofday(&initial_time_val, (struct timezone *)0);
    decode_run(true);
    gettimeofday(&final_time_val, (struct timezone *)0);
    printf( "execution time: %ldus\n", (long)(final_time_val.tv_usec- initial_time_val.tv_usec ));
    
    return 0;
}
