#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) IDTEntry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDTPointer;

static IDTEntry idt[256];
static IDTPointer idt_pointer;

extern void isr0(void);
extern void irq0(void);
extern void irq1(void);

extern void timer_handler(void);

static uint16_t get_cs(void)
{
    uint16_t cs;

    __asm__ volatile (
        "mov %%cs, %0"
        : "=r"(cs)
    );

    return cs;
}

void exception_handler(void)
{
    for (;;)
        __asm__ volatile ("hlt");
}

static void idt_set_gate(
    int vector,
    uint64_t handler,
    uint16_t selector
)
{
    idt[vector].offset_low =
        handler & 0xFFFF;

    idt[vector].selector =
        selector;

    idt[vector].ist = 0;
    idt[vector].type_attr = 0x8E;

    idt[vector].offset_mid =
        (handler >> 16) & 0xFFFF;

    idt[vector].offset_high =
        (handler >> 32) & 0xFFFFFFFF;

    idt[vector].zero = 0;
}

static void idt_load(void)
{
    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_pointer)
    );
}

void idt_init(void)
{
    uint16_t cs = get_cs();

    for (int i = 0; i < 256; i++)
        idt_set_gate(i, 0, cs);

    idt_set_gate(0, (uint64_t)isr0, cs);
    idt_set_gate(32, (uint64_t)irq0, cs);
    idt_set_gate(33, (uint64_t)irq1, cs);

    idt_pointer.limit =
        sizeof(idt) - 1;

    idt_pointer.base =
        (uint64_t)&idt;

    idt_load();
}
