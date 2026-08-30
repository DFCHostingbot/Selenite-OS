AS=as
CC=gcc
LD=ld
OBJCOPY=objcopy

ASFLAGS=-g
CFLAGS=-ffreestanding \
       -fno-stack-protector \
       -fno-stack-check \
       -mno-red-zone \
       -I./include

LDFLAGS=-T linker.ld -nostdlib

BUILD_DIR=build
SRC_DIR=src

SOURCES=$(SRC_DIR)/entry.asm $(SRC_DIR)/kernel.c
OBJECTS=$(BUILD_DIR)/entry.o $(BUILD_DIR)/kernel.o
TARGET=$(BUILD_DIR)/selenite-core.elf

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: $(SRC_DIR)/entry.asm
	$(AS) $(ASFLAGS) $(SRC_DIR)/entry.asm -o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
