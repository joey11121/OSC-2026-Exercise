RISCV_GNU ?= riscv64-unknown-elf

CC      := $(RISCV_GNU)-gcc
LD      := $(RISCV_GNU)-ld
OBJCOPY := $(RISCV_GNU)-objcopy
QEMU    ?= qemu-system-riscv64

CFLAGS  := -mcmodel=medany -ffreestanding -nostdlib -g -Wall
LDFLAGS := -T link.ld

ELF     := kernel.elf
BIN     := kernel.bin
OBJS    := start.o main.o uart.o

.PHONY: all build run clean

all: build

build: $(BIN)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

uart.o: uart.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	$(QEMU) -M virt -m 512M -kernel $(BIN) -display none -serial stdio

clean:
	rm -f $(OBJS) $(ELF) $(BIN)
