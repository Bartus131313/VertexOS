# Tools
AS = nasm
CC = gcc
LD = ld

# Flags
# Added -Isrc so you can #include "drivers/io.h" easily
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -Isrc
LDFLAGS = -m elf_i386 -T src/linker.ld -nostdlib

# Directories
SRC_DIR = src
BUILD_DIR = build

# 1. Automatically find all .c and .asm files in all subdirectories
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
ASM_SOURCES = $(shell find $(SRC_DIR) -name "*.asm")

# 2. Generate the list of object files
# This takes 'src/drivers/clock.c', removes 'src/drivers/', and adds 'build/' and '.o'
# Result: build/clock.o
OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o))) \
       $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SOURCES:.asm=.o)))

# 3. Tell Make where to look for source files (the VPATH)
# This allows us to use %.c even if the file is deep in a subdirectory
VPATH = $(shell find $(SRC_DIR) -type d)

# Output files
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/system.iso

all: $(ISO)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link the kernel
$(KERNEL): $(OBJS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

# Compile C files (The VPATH handles finding them in subfolders)
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Create the bootable ISO
$(ISO): $(KERNEL) grub.cfg
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(KERNEL) $(BUILD_DIR)/isodir/boot/system.bin
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BUILD_DIR)/isodir

clean:
	rm -rf $(BUILD_DIR)

run: all
	qemu-system-x86_64 -cdrom $(ISO) \
    -m 512M \
    -cpu max \
    -device pci-ohci -device usb-tablet