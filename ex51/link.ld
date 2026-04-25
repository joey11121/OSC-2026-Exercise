OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS
{
<<<<<<< HEAD
    . = 0x80000000;
=======
    . = 0x80200000;
>>>>>>> ac63b0b2257b14c8614426c45c4c7f22f945b564
    _start = .;
    .text : { *(.text.boot) *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : {
        __bss_start = .;
        *(.sbss) *(.bss)
        __bss_stop = .;
    }
    . = ALIGN(16);
    . = . + 0x4000;
    _end = .;
}
