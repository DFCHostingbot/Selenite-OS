CC=gcc
LD=ld
OBJCOPY=objcopy

CFLAGS=-I/usr/include/efi \
        -I/usr/include/efi/x86_64 \
        -I/usr/include/efi/protocol \
        -fpic \
        -ffreestanding \
        -fno-stack-protector \
        -fno-stack-check \
        -fshort-wchar \
        -mno-red-zone \
        -DEFI_FUNCTION_WRAPPER

LDFLAGS=-shared \
        -Bsymbolic \
        -T /usr/lib/elf_x86_64_efi.lds \
        /usr/lib/crt0-efi-x86_64.o

all:
	mkdir -p build
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o
	$(LD) $(LDFLAGS) build/main.o /usr/lib/libgnuefi.a /usr/lib/libefi.a -o build/subs.so
	$(OBJCOPY) \
		-j .text \
		-j .sdata \
		-j .data \
		-j .dynamic \
		-j .dynsym \
		-j .rel \
		-j .rela \
		-j .reloc \
		--target=efi-app-x86_64 \
		build/subs.so \
		build/BOOTX64.EFI

clean:
	rm -rf build
