#include <stdint.h>
extern volatile uint8_t keyboard_last_scancode;
extern volatile char keyboard_last_char;
extern volatile char keyboard_buffer[128];
extern volatile uint64_t keyboard_buffer_pos;
extern volatile uint8_t keyboard_command_ready;
extern volatile uint64_t keyboard_ticks;
static void shell_command(const char *cmd);

typedef struct {
    uint64_t memory_map;
    uint64_t memory_map_size;
    uint64_t memory_descriptor_size;
} BootInfo;

typedef struct {
    uint32_t type;
    uint32_t pad;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
} MemoryDescriptor;

#define PAGE_SIZE 4096
#define MAX_HEAP_BLOCKS 256
#define MAX_FREE_PAGES 1024

static uint64_t total_pages = 0;
static uint64_t used_pages = 0;

static uint64_t allocator_start = 0;
static uint64_t allocator_pages = 0;

static uint64_t free_pages[MAX_FREE_PAGES];
static uint64_t free_page_count = 0;

typedef struct {
    uint64_t address;
    uint64_t pages;
    uint64_t size;
    uint8_t used;
} HeapBlock;

static HeapBlock heap_blocks[MAX_HEAP_BLOCKS];

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

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void serial_init(void)
{
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x03);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);
}

static void serial_putc(char c)
{
    while (!(inb(0x3FD) & 0x20))
        ;

    outb(0x3F8, (uint8_t)c);
}

static void serial_print(const char *text)
{
    for (int i = 0; text[i] != '\0'; i++)
        serial_putc(text[i]);
}

static void serial_print_hex(uint64_t value)
{
    const char *hex = "0123456789ABCDEF";

    serial_print("0x");

    for (int i = 15; i >= 0; i--)
        serial_putc(hex[(value >> (i * 4)) & 0xF]);
}

static void memory_init(BootInfo *boot_info)
{
    uint64_t entries =
        boot_info->memory_map_size /
        boot_info->memory_descriptor_size;

    const uint64_t SAFE_START = 0x200000;

    for (uint64_t i = 0; i < entries; i++) {

        MemoryDescriptor *desc =
            (MemoryDescriptor *)(
                boot_info->memory_map +
                i * boot_info->memory_descriptor_size
            );

        if (desc->type != 7)
            continue;

        total_pages += desc->number_of_pages;

        uint64_t region_start =
            desc->physical_start;

        uint64_t region_end =
            region_start +
            desc->number_of_pages * PAGE_SIZE;

        if (region_end <= SAFE_START)
            continue;

        if (region_start < SAFE_START)
            region_start = SAFE_START;

        uint64_t pages =
            (region_end - region_start) / PAGE_SIZE;

        if (allocator_pages == 0 && pages > 0) {
            allocator_start = region_start;
            allocator_pages = pages;
        }
    }
}

static uint64_t page_alloc(void)
{
    if (free_page_count > 0)
        return free_pages[--free_page_count];

    if (used_pages >= allocator_pages)
        return 0;

    uint64_t address =
        allocator_start +
        used_pages * PAGE_SIZE;

    used_pages++;

    return address;
}

static void page_free(uint64_t address)
{
    if (address < allocator_start)
        return;

    if (address >= allocator_start +
                    allocator_pages * PAGE_SIZE)
        return;

    if (free_page_count >= MAX_FREE_PAGES)
        return;

    free_pages[free_page_count++] = address;
}

static void *kmalloc(uint64_t size)
{
    if (size == 0)
        return 0;

    uint64_t pages =
        (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint64_t i = 0; i < MAX_HEAP_BLOCKS; i++) {

        if (heap_blocks[i].used)
            continue;

        uint64_t first_page = 0;

        for (uint64_t p = 0; p < pages; p++) {

            uint64_t page = page_alloc();

            if (page == 0)
                return 0;

            if (p == 0)
                first_page = page;
        }

        heap_blocks[i].address = first_page;
        heap_blocks[i].pages = pages;
        heap_blocks[i].size = size;
        heap_blocks[i].used = 1;

        return (void *)first_page;
    }

    return 0;
}

static void kfree(void *ptr)
{
    if (!ptr)
        return;

    uint64_t address =
        (uint64_t)ptr;

    for (uint64_t i = 0; i < MAX_HEAP_BLOCKS; i++) {

        if (!heap_blocks[i].used)
            continue;

        if (heap_blocks[i].address != address)
            continue;

        for (uint64_t p = 0;
             p < heap_blocks[i].pages;
             p++) {

            page_free(
                address + p * PAGE_SIZE
            );
        }

        heap_blocks[i].used = 0;

        return;
    }
}

static void heap_test(void)
{
    serial_print(
        "HEAP ALLOCATOR v0.6.1\r\n"
    );

    void *a = kmalloc(64);

    serial_print(
        "kmalloc(64): "
    );

    serial_print_hex(
        (uint64_t)a
    );

    serial_print("\r\n");

    void *b = kmalloc(8192);

    serial_print(
        "kmalloc(8192): "
    );

    serial_print_hex(
        (uint64_t)b
    );

    serial_print("\r\n");

    kfree(b);

    serial_print(
        "kfree(8192): OK\r\n"
    );

    void *c = kmalloc(8192);

    serial_print(
        "kmalloc(8192) reused: "
    );

    serial_print_hex(
        (uint64_t)c
    );

    serial_print("\r\n");

    serial_print(
        "Heap ready\r\n"
    );
}

void idt_init(void);
void pic_remap(void);
void pit_init(uint32_t frequency);


volatile uint64_t timer_ticks = 0;

void timer_handler(void)
{
    timer_ticks++;
}


static int str_equal(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

static void shell_command(const char *cmd)
{
    if (str_equal(cmd, "help")) {
        serial_print("commands:\r\n");
        serial_print("  help\r\n");
        serial_print("  mem\r\n");
        serial_print("  uptime\r\n");

    } else if (str_equal(cmd, "mem")) {
        serial_print("Memory manager: OK\r\n");

    } else if (str_equal(cmd, "uptime")) {
        serial_print("Uptime ticks: ");
        serial_print_hex(timer_ticks);
        serial_print("\r\n");

    } else {
        serial_print("Unknown command\r\n");
    }
}

void kernel_main(BootInfo *boot_info)
{
    serial_init();

    serial_print(
        "SELENITE CORE v0.6.1 RUNNING\r\n"
    );

    if (!boot_info) {

        serial_print(
            "ERROR: No BootInfo\r\n"
        );

        for (;;)
            __asm__ volatile ("hlt");
    }

    serial_print(
        "BootInfo received\r\n"
    );

    idt_init();
    pic_remap();
    pit_init(100);

    serial_print(
        "IDT initialized\r\n"
    );

    memory_init(boot_info);

    serial_print(
        "PAGE ALLOCATOR\r\n"
    );

    serial_print(
        "Page size: 4096 bytes\r\n"
    );

    serial_print(
        "Usable pages: "
    );

    serial_print_hex(total_pages);

    serial_print("\r\n");

    serial_print(
        "Allocator initialized\r\n"
    );

    uint64_t page1 = page_alloc();

    serial_print(
        "Allocated page: "
    );

    serial_print_hex(page1);

    serial_print("\r\n");

    uint64_t page2 = page_alloc();

    serial_print(
        "Allocated page: "
    );

    serial_print_hex(page2);

    serial_print("\r\n");

    page_free(page2);

    serial_print(
        "Freed page: "
    );

    serial_print_hex(page2);

    serial_print("\r\n");

    serial_print(
        "Page allocator ready\r\n"
    );

    heap_test();

    serial_print(
        "Timer ready\r\n"
    );

    __asm__ volatile ("sti");

    uint64_t last_tick = 0;
    uint64_t last_key = 0;

    for (;;) {

        if (timer_ticks != last_tick) {

            last_tick = timer_ticks;

            if ((timer_ticks % 100) == 0) {
                serial_print("Timer ticks: ");
                serial_print_hex(timer_ticks);
                serial_print("\r\n");
            }
        }

        if (keyboard_command_ready) {
            keyboard_command_ready = 0;

            shell_command((const char *)keyboard_buffer);

            keyboard_buffer_pos = 0;
            keyboard_buffer[0] = '\0';
        }

    }
}
