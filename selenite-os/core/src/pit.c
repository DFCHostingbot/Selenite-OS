#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void pit_init(uint32_t frequency)
{
    uint32_t divisor =
        1193182 / frequency;

    outb(0x43, 0x36);

    outb(0x40,
         divisor & 0xFF);

    outb(0x40,
         (divisor >> 8) & 0xFF);
}
