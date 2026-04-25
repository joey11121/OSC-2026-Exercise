RISCV_GNU ?= riscv64-unknown-elf
CC = $(RISCV_GNU)-gcc
LD = $(RISCV_GNU)-ld
OBJCOPY = $(RISCV_GNU)-objcopy
CFLAGS = -march=rv64gc_zicbom -mcmodel=medany -ffreestanding -nostdlib -g -Wall
QEMU ?= qemu-system-riscv64
TARGET = kernel

build: clean
	$(CC) $(CFLAGS) -c *.S *.c
	$(CC) -T link.ld -nostdlib -o $(TARGET).elf *.o -lgcc
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)

run: build $(TARGET)
<<<<<<< HEAD
	$(QEMU) -M virt -m 8G -kernel $(TARGET) -serial stdio -device ramfb -display sdl 
=======
	$(QEMU) -M virt -m 8G -kernel $(TARGET) -serial stdio -device ramfb
>>>>>>> ac63b0b2257b14c8614426c45c4c7f22f945b564

clean:
	rm -f $(TARGET) $(TARGET).elf *.o
