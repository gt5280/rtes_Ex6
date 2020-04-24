
#include "decode.h"

#include <stdint.h>

#define DOT_LENGTH (3)
#define DOT_MIN (DOT_LENGTH - 1)
#define DOT_MAX (DOT_LENGTH + 1)
#define DASH_LENGTH (DOT_LENGTH * 3)
#define DASH_MIN (DASH_LENGTH - 1)
#define DASH_MAX (DASH_LENGTH + 1)

#define MAX_MORSE_SEQUENCE (6) // The longest sequence to determine a character is 5, + 1 for 'morse_letter_end' after
#define NUMBER_OF_CHARACTERS (36) // 26 letters + 10 numbers

typedef enum
{
    morse_empty, // a morse character has not been received yet
    morse_dot,
    morse_dash,
    morse_letter_end,
    morse_space,
} morse_t;

typedef struct
{
    char name;
    uint8_t len;
    morse_t sequence[MAX_MORSE_SEQUENCE];
} character_t;

const character_t characters[NUMBER_OF_CHARACTERS] = 
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
}

morse_t current_sequence[MAX_MORSE_SEQUENCE] = {};

static void reset_current_sequence(void)
{
    for (uint8_t i = 0; i < MAX_MORSE_SEQUENCE)
    {
        current_sequence[i] = morse_empty;
    }
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
        // the sequence is morse_letter_end, we found a match
        if (character_match && (current_sequence[len] == morse_letter_end))
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

    // This section will not be in the released code.
    // I am intializing to an invalid character to try to estimate the WCET
    // as the code has to do the comparison with every character
    current_sequence[0] = morse_dot;
    current_sequence[1] = morse_dot;
    current_sequence[2] = morse_dot;
    current_sequence[3] = morse_dot;
    current_sequence[4] = morse_dot;
    current_sequence[5] = morse_letter_end;    
}

void decode_run(bool light_on)
{
    // State machine has not been developed yet...
    // Right now, this just simulates that we just received the 'letter end'
    // code, meaning we need to try to match the received sequence to a letter.
    // We have initialized the sequence to an invalid character, which should be
    // the worst case for the comparisons
    morse_t latest_symbol = morse_letter_end;
    
    if (latest_symbol == morse_letter_end)
    {
        char c = compare_sequence();
        if (c)
        {
            printf("%c", c);
        }
        else
        {
            printf("No match!");
        }

        reset_current_sequence();
    }

}