
#include "decode.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    morse_empty, // a morse character has not been received yet
    morse_dot,
    morse_dash,
    morse_letter_end,
    morse_word_end,
} morse_t;

typedef struct
{
    char name;
    uint8_t len;
    morse_t sequence[MAX_MORSE_SEQUENCE];
} character_t;

static const character_t characters[NUMBER_OF_CHARACTERS] = 
{
    { 'A', 2, {morse_dot, morse_dash} },
    { 'B', 4, {morse_dash, morse_dot, morse_dot, morse_dot} },
    { 'C', 4, {morse_dash, morse_dot, morse_dash, morse_dot} },
    { 'D', 3, {morse_dash, morse_dot, morse_dot} },
    { 'E', 1, {morse_dot} },
    { 'F', 4, {morse_dot, morse_dot, morse_dash, morse_dot} },
    { 'G', 3, {morse_dash, morse_dash, morse_dot} },
    { 'H', 4, {morse_dot, morse_dot, morse_dot, morse_dot} },
    { 'I', 2, {morse_dot, morse_dot} },
    { 'J', 4, {morse_dot, morse_dash, morse_dash, morse_dash,} },
    { 'K', 3, {morse_dash, morse_dot, morse_dash} },
    { 'L', 4, {morse_dot, morse_dash, morse_dot, morse_dot} },
    { 'M', 2, {morse_dash, morse_dash} },
    { 'N', 2, {morse_dash, morse_dot} },
    { 'O', 3, {morse_dash, morse_dash, morse_dash} },
    { 'P', 4, {morse_dot,  morse_dash, morse_dash, morse_dot} },
    { 'Q', 4, {morse_dash, morse_dash, morse_dot, morse_dash} },
    { 'R', 3, {morse_dot, morse_dash, morse_dot} },
    { 'S', 3, {morse_dot, morse_dot, morse_dot} },
    { 'T', 1, {morse_dash} },
    { 'U', 3, {morse_dot, morse_dot, morse_dash} },
    { 'V', 4, {morse_dot, morse_dot, morse_dot, morse_dash} },
    { 'W', 3, {morse_dot, morse_dot, morse_dot, morse_dash} },
    { 'X', 4, {morse_dash, morse_dot, morse_dot, morse_dash} },
    { 'Y', 4, {morse_dash, morse_dot, morse_dash, morse_dash} },
    { 'Z', 4, {morse_dash, morse_dash, morse_dot, morse_dot} },
    { '1', 5, {morse_dot, morse_dash, morse_dash, morse_dash, morse_dash} },
    { '2', 5, {morse_dot, morse_dot, morse_dash, morse_dash, morse_dash} },
    { '3', 5, {morse_dot, morse_dot, morse_dot, morse_dash, morse_dash} },
    { '4', 5, {morse_dot, morse_dot, morse_dot, morse_dot, morse_dash} },
    { '5', 5, {morse_dot, morse_dot, morse_dot, morse_dot, morse_dot} },
    { '6', 5, {morse_dash, morse_dot, morse_dot, morse_dot, morse_dot} },
    { '7', 5, {morse_dash, morse_dash, morse_dot, morse_dot, morse_dot} },
    { '8', 5, {morse_dash, morse_dash, morse_dash, morse_dot, morse_dot} },
    { '9', 5, {morse_dash, morse_dash, morse_dash, morse_dash, morse_dot} },
    { '0', 5, {morse_dash, morse_dash, morse_dash, morse_dash, morse_dash} }
};

static void send_dot(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        decode_run(true);
    }
}

static void send_dash(void)
{
    for (uint8_t i = 0; i < 9; i++)
    {
        decode_run(true);
    }
}

static void send_symbol_break(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        decode_run(false);
    }
}

static void send_char_break(void)
{
    for (uint8_t i = 0; i < 9; i++)
    {
        decode_run(false);
    }
}

static void send_word_break(void)
{
    for (uint8_t i = 0; i < 21; i++)
    {
        decode_run(false);
    }
}

static void send_char(char c)
{
    for (uint8_t i = 0; i < 36; i++)
    {
        if (c == characters[i].name)
        {
            for (uint8_t pos = 0; pos < characters[i].len; pos++)
            {
                if (pos != 0)
                {
                    send_symbol_break();
                }

                if (characters[i].sequence[pos] == morse_dot)
                {
                    send_dot();
                }
                else if (characters[i].sequence[pos] == morse_dash)
                {
                    send_dash();
                }
                else
                {
                    printf("Sequence error!");
                }
                
            }
        }
    }
}

static void send_word(char* str, uint8_t len)
{
    for (uint8_t i = 0; i < len)
    {
        if (i != 0)
        {
            send_char_break();
        }
        send_char(str[i]);
    }
}

int main()
{
    struct timeval initial_time_val;
    struct timeval final_time_val;

    decode_init();

    //gettimeofday(&initial_time_val, (struct timezone *)0);
    //decode_run(true);
    //gettimeofday(&final_time_val, (struct timezone *)0);
    //printf( "execution time: %ldus\n", (long)(final_time_val.tv_usec- initial_time_val.tv_usec ));

    decode_run(true);
    decode_run(true);
    decode_run(true);

    // send start signal!
    decode_run(false);
    decode_run(false);
    decode_run(false);

    send_word("test", 4);
    send_word_break();
    send_word("abcdefghijklmnopqrstuvwxyz0123456789", 36);
    
    return 0;
}
