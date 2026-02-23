# Tools
ASM = nasm
CC = gcc
LD = ld

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -Isrc
LDFLAGS = -m elf_i386 -T src/linker.ld -nostdlib

# Directories
SRC_DIR = src
BUILD_DIR = build

# Sources & Objects
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
ASM_SOURCES = $(shell find $(SRC_DIR) -name "*.asm")
OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o))) \
       $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SOURCES:.asm=.o)))

VPATH = $(shell find $(SRC_DIR) -type d)

# Output files
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/system.iso
GRUB = src/boot/grub.cfg

.PHONY: all clean run

all: $(ISO)
	@echo ""
	@echo "[SUCCESS] VertexOS build is complete!"
	@echo "[OUTPUT] $(ISO)"
	@echo ""

$(BUILD_DIR):
	@echo "[DIR] Creating build directory: $@"
	@mkdir -p $(BUILD_DIR)

# Linking the Kernel
$(KERNEL): $(OBJS) | $(BUILD_DIR)
	@echo "[LINK] Linking objects into kernel: $@"
	@$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

# Compiling C - Using $< and $@ to show filenames
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "[CC]  Compiling: $<  ->  $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assembling ASM
$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	@echo "[ASM]  Assembling: $<  ->  $@"
	@$(ASM) $(ASFLAGS) $< -o $@

# ISO Creation
$(ISO): $(KERNEL) $(GRUB)
	@echo "[ISO] Preparing ISO filesystem..."
	@mkdir -p $(BUILD_DIR)/isodir/boot/grub
	@cp $(KERNEL) $(BUILD_DIR)/isodir/boot/system.bin
	@cp $(GRUB) $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	@echo "[ISO] Generating bootable image with grub-mkrescue..."
	@grub-mkrescue -o $(ISO) $(BUILD_DIR)/isodir > /dev/null 2>&1

clean:
	@echo "[CLEAN] Removing all build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "[CLEAN] Workspace is clear."

# Run command with the hardware flags for Mouse and Audio
run: all
	@echo "[RUN] Launching VertexOS in QEMU..."
	@qemu-system-x86_64 -cdrom $(ISO) \
		-cpu max -m 512M \
		-device pci-ohci -device usb-tablet