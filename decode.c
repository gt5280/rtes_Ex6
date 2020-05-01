
#include "decode.h"

#include <stdint.h>
#include <stdio.h>

#define DOT_LENGTH (3)
#define DOT_MIN (DOT_LENGTH - 1)
#define DOT_MAX (DOT_LENGTH + 1)
#define DASH_LENGTH (DOT_LENGTH * 3)
#define DASH_MIN (DASH_LENGTH - 2)
#define DASH_MAX (DASH_LENGTH + 2)

#define SYMBOL_SEPARATOR_LENGTH (DOT_LENGTH)
#define SYMBOL_SEPARATOR_MIN (DOT_MIN)
#define SYMBOL_SEPARATOR_MAX (DOT_MAX)

#define CHARACTER_SEPARATOR_LENGTH (DASH_LENGTH)
#define CHARACTER_SEPARATOR_MIN (DASH_MIN)
#define CHARACTER_SEPARATOR_MAX (DASH_MAX)

#define WORD_SEPARATOR_LENGTH (DOT_LENGTH * 7)
#define WORD_SEPARATOR_MIN (WORD_SEPARATOR_LENGTH - 3)
#define WORD_SEPARATOR_MAX (WORD_SEPARATOR_LENGTH + 3)

#define MAX_MORSE_SEQUENCE (6) // The longest sequence to determine a character is 5, + 1 for 'morse_letter_end' after
#define NUMBER_OF_CHARACTERS (36) // 26 letters + 10 numbers

#define MAX_ON_OFF_LENGTH (7) // the longest on/off sequence is 7 (signifying a space between words)

typedef enum
{
    morse_empty, // a morse character has not been received yet
    morse_dot,
    morse_dash,
    morse_letter_end,
    morse_word_end,
} morse_t;

typedef enum
{
    state_idle,
    state_start,
    state_on,
    state_off,
} state_t;

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

morse_t current_sequence[MAX_MORSE_SEQUENCE] = {};
uint8_t sequence_position = 0;

state_t current_state = state_idle;

static void reset_current_sequence(void)
{
    for (uint8_t i = 0; i < MAX_MORSE_SEQUENCE; i++)
    {
        current_sequence[i] = morse_empty;
    }
    sequence_position = 0;
}

// This function checks the current sequence against all the characters.
// If a match is found, it returns the character. Otherwise it returns 0 (null character)
static char compare_sequence(void)
{
    for (uint8_t i = 0; i < NUMBER_OF_CHARACTERS; i++)
    {
        bool character_match = true;
        uint8_t len = characters[i].len;
        for (uint8_t j = 0; j < len; j++)
        {
            if (current_sequence[j] != characters[i].sequence[j])
            {
                character_match = false;
                break;
            }            
        }

        // If the character matched the sequence, and the next morse symbol in
        // the sequence is morse_letter_end or morse_word_end, we found a match
        if (character_match && (current_sequence[len] == morse_letter_end || current_sequence[len] == morse_word_end))
        {
            // we found a match! Return the char
            return characters[i].name;
        }
    }

    // We didn't find a match on any of the characters. Return 0
    return 0;
}

void decode_init(void)
{
    reset_current_sequence();
}

void decode_run(bool light_on)
{
    static uint8_t on_count = 0;
    static uint8_t off_count = 0;
    morse_t latest_symbol = morse_empty;

    // Update on/off count
    if (light_on)
    {
        on_count++;
    }
    else
    {
        off_count++;
    }
    

    // State Action
    switch (current_state)
    {
    case state_idle:
        break;
    case state_start:
        break;
    case state_on:
        break;
    case state_off:
        break;
    default:
        break;
    }

    // State Update
    switch (current_state)
    {
    case state_idle:
        if (!light_on)
        {
            current_state = state_start;
        }
        break;
    case state_start:
        if (light_on)
        {
            current_state = state_on;
            on_count = 1; // reset to 1 because have seen 1 ON so far
        }
        break;
    case state_on:
        if (on_count > DASH_MAX) // If the light has gone back to an idle state
        {
            current_state = state_idle;
        }
        else if (!light_on)
        {
            if (on_count >= DOT_MIN && on_count <= DOT_MAX)
            {
                // A dot was detected!
                latest_symbol = morse_dot;
            }
            else if (on_count >= DASH_MIN && on_count <= DASH_MAX)
            {
                // A dash was detected!
                latest_symbol = morse_dash;
            }

            current_state = state_off;
            off_count = 1; // reset to 1 because have seen 1 OFF so far
        }
        break;
    case state_off:
        if (light_on)
        {
            if (off_count < SYMBOL_SEPARATOR_MAX)
            {
                // next symbol, nothing to do...
            }
            else if (off_count >= CHARACTER_SEPARATOR_MIN && off_count <= CHARACTER_SEPARATOR_MAX)
            {
                // end of character detected!
                latest_symbol = morse_letter_end;
            }
            else if (off_count > WORD_SEPARATOR_MIN)
            {
                // end of word detected!
                latest_symbol = morse_word_end;
            }

            current_state = state_on;
            on_count = 1; // reset to 1 because have seen 1 ON so far
        }
        break;
    default:
        break;
    }


    
    if (latest_symbol != morse_empty)
    {
        // A symbol was detected! add it to our buffer
        current_sequence[sequence_position] = latest_symbol;
        sequence_position++;
        if (sequence_position >= MAX_MORSE_SEQUENCE)
        {
            if (latest_symbol != morse_letter_end && latest_symbol != morse_word_end)
            {
                printf("Error! received more dots and dashes than expected!\n");
                reset_current_sequence();
                latest_symbol = morse_empty;
            }
        }

        if (latest_symbol == morse_letter_end || latest_symbol == morse_word_end)
        {
            char c = compare_sequence();
            if (c)
            {
                if (latest_symbol == morse_word_end)
                {
                    printf("%c ", c); // print the character plus a space
                }
                else
                {
                    printf("%c", c); // just print the character
                }
            }
            else
            {
                printf("\nNo match!\n");
            }

            reset_current_sequence();
        }
    }
    

}
