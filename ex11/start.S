.section ".text.boot"
.global _start
_start:
    // TODO: Clear the .bss section
    // TODO: Set up the stack pointer
    la sp, _end
    la s2, __bss_start
    la s3, __bss_stop
    
loop_start:
    beq s2, s3, loop_end
    sb zero, 0(s2)
    addi s2, s2, 1
    j loop_start
loop_end:
    tail start_kernel
