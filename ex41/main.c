
extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);
extern int hextoi(const char* s, int n);
extern int align(int n, int byte);
extern int memcmp(const void* s1, const void* s2, int n);
extern void* alloc_page();

// TODO: Check the RAM disk base address
#define INITRD_BASE 0x88200000
#define STACK_SIZE  0x1000

struct cpio_t {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
};

void uart_binary(unsigned long k)
{
    if (k == 0) {
        uart_putc('0');
        return;
    }

    char binary[64];
    int i = 0;

    while (k != 0) {
        binary[i++] = '0' + (k & 1);
        k >>= 1;
    }

    while (i) {
        uart_putc(binary[--i]);
    }
    uart_putc('\n');
}



int exec(const char* filename) {
    char* p = (char*)INITRD_BASE;
    while (memcmp(p + sizeof(struct cpio_t), "TRAILER!!!", 10)) {
        struct cpio_t* hdr = (struct cpio_t*)p;
        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);
        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        int datasize = align(filesize, 4);
        if (!memcmp(p + sizeof(struct cpio_t), filename, namesize)) {
            // TODO: Finish this function
            //1. get the file content of the exec file 
            char* data = p + headsize; 
            //2. Create the stack for the exec file, set the user stack to the highest address in the stack. 
            void* user_stack = alloc_page(); 
            unsigned long user_sp = (unsigned long)user_stack + STACK_SIZE; 
            

            //3. Save the user_sp to the sscratch, and set sstatus as the start of the file. 
            //asm volatile("csrw sscratch, %0"::"r"(user_sp));
            

            //4. Switch from kernel to the user mode. 
            unsigned long sstatus; 
            asm volatile("csrr %0, sstatus":"=r"(sstatus));
            
            sstatus &= ~(1UL << 8);
            asm volatile("csrw sstatus, %0"::"r"(sstatus)); 

            asm volatile("csrw sepc, %0"::"r"(data)); 

            asm volatile("mv sp, %0": : "r"(user_sp)); 
            
            //5. Return from the kernel mode to user mode based on sstatus
            asm volatile("sret"); 
            __builtin_unreachable(); 

        }
        p += headsize + datasize;
    }
    return -1;
}

// TODO: Define the trap frame structure
struct pt_regs {
    unsigned long ra;
    unsigned long sp;     
    unsigned long gp, tp; 
    unsigned long t0, t1, t2;
    unsigned long s0, s1; 
    unsigned long a0, a1, a2, a3, a4, a5, a6, a7; 
    unsigned long s2, s3, s4, s5, s6, s7, s8, s9, s10, s11; 
    unsigned long t3, t4, t5, t6;
    unsigned long sepc; 
    unsigned long sstatus; 
    unsigned long scause; 
    unsigned long stval;    
};

void do_trap(struct pt_regs* regs) {
    // TODO: Implement this function
    // (1) Print the sepc and scause registers
    // (2) Increment the sepc register by 4 for traps
     
    uart_puts("sepc: "); 
    uart_hex(regs -> sepc);
    uart_puts(", scause: "); 
    uart_hex(regs -> scause);  
    uart_puts("\n");
    //while(1); 
    regs -> sepc += 4; 
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    if (exec("prog.bin"))
        uart_puts("Failed to exec user program!\n");
    while (1) {
        uart_putc(uart_getc());
    }
}
