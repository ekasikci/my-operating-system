#include <lib/utility.h>

// Converts an integer to a string representation.
void intToStr(int32_t value, char *str);

// Simple implementation of printf that outputs to video memory.
void printf(char *str)
{
    static uint16_t *VideoMemory = (uint16_t *)0xb8000; // Start of video memory for text.

    static uint8_t cursorX = 0, cursorY = 0; // Cursor positions.

    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
        case '\n': // Handle newline by resetting x and incrementing y.
            cursorX = 0;
            cursorY++;
            break;
        default: // Print character to screen.
            VideoMemory[80 * cursorY + cursorX] = (VideoMemory[80 * cursorY + cursorX] & 0xFF00) | str[i];
            cursorX++;
            break;
        }

        if (cursorX >= 80) // Line wrap.
        {
            cursorX = 0;
            cursorY++;
        }

        if (cursorY >= 25) // Screen scroll.
        {
            for (cursorY = 0; cursorY < 25; cursorY++)
                for (cursorX = 0; cursorX < 80; cursorX++)
                    VideoMemory[80 * cursorY + cursorX] = (VideoMemory[80 * cursorY + cursorX] & 0xFF00) | ' ';
            cursorX = 0;
            cursorY = 0;
        }
    }
}

// Prints a hexadecimal value as a string.
void printfHex(uint8_t key)
{
    char hexValue[3] = "00";
    const char *hexDigits = "0123456789ABCDEF";
    hexValue[0] = hexDigits[(key >> 4) & 0xF];
    hexValue[1] = hexDigits[key & 0xF];
    printf(hexValue);
}

// Prints a formatted message to the screen using a specified format and arguments list.
void print(const char *format, uint8_t numArgs, int32_t args[])
{
    const uint16_t bufferSize = 512;
    const uint8_t numBuffSize = 16;
    uint16_t bufferIndex = 0;
    uint8_t argsIndex = 0;

    int8_t outputBuffer[bufferSize];
    int8_t numberBuffer[numBuffSize];

    for (uint16_t i = 0; i < bufferSize; ++i)
        outputBuffer[i] = 0;

    for (uint16_t i = 0; format[i] != 0; ++i)
    {
        if (format[i] == '%')
        {
            i++; // Skip '%d'

            intToStr(args[argsIndex++], numberBuffer);

            for (uint8_t j = 0; numberBuffer[j] != 0; ++j)
                outputBuffer[bufferIndex++] = numberBuffer[j];
        }
        else
        {
            outputBuffer[bufferIndex++] = format[i];
        }
    }

    printf(outputBuffer);
}

// Converts an integer to a string, ensuring proper handling of negative values.
void intToStr(int32_t value, char *str)
{
    int32_t i = 0, j = 0;
    int8_t temp;

    if (value < 0)
    {
        str[0] = '-';
        i = 1;
        value = -value;
    }

    do
    {
        str[i++] = (value % 10) + '0';
        value /= 10;
    } while (value);

    str[i] = '\0';

    // Reverse the string for correct display
    if (str[0] == '-')
        j = 1;

    for (--i; j < i; j++, i--)
    {
        temp = str[j];
        str[j] = str[i];
        str[i] = temp;
    }
}

// Print unsigned integer value.
void printUint(uint32_t var)
{
    int8_t buffer[16];

    for (uint32_t i = 0; i < 16; ++i)
        buffer[i] = 0;

    intToStr(var, buffer);
    printf(buffer);
}
