#include <stdint.h>

typedef struct {
    uint64_t memory_map;
    uint64_t memory_map_size;
    uint64_t memory_descriptor_size;
} BootInfo;

void kernel_main(BootInfo *boot_info)
{
    (void)boot_info;

    volatile uint16_t *video =
        (volatile uint16_t *)0xB8000;

    const char *text =
        "SELENITE CORE v0.1 RUNNING";

    for (int i = 0; text[i] != '\0'; i++) {
        video[i] =
            (uint16_t)text[i] | 0x0700;
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
