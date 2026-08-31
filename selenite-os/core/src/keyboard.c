#include <stdint.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

volatile uint8_t keyboard_last_scancode = 0;
volatile uint64_t keyboard_ticks = 0;

volatile char keyboard_last_char = 0;

#define KEYBOARD_BUFFER_SIZE 128

volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
volatile uint64_t keyboard_buffer_pos = 0;
volatile uint8_t keyboard_command_ready = 0;

static const char scancode_table[128] = {
    0, 27,
    '1','2','3','4','5','6','7','8','9','0','-','=',
    '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']',
    '\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\',
    'z','x','c','v','b','n','m',',','.','/',
    0,'*',0,' '
};

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    keyboard_last_scancode = scancode;

    if (!(scancode & 0x80)) {

        keyboard_ticks++;

        if (scancode < 128) {

            char c = scancode_table[scancode];

            if (c) {

                keyboard_last_char = c;

                if (c == '\n') {
                    keyboard_command_ready = 1;
                } else if (keyboard_buffer_pos < KEYBOARD_BUFFER_SIZE - 1) {
                    keyboard_buffer[keyboard_buffer_pos++] = c;
                    keyboard_buffer[keyboard_buffer_pos] = '\0';
                }
            }
        }
    }
}
