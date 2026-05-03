#define UART_BASE 0x10000000UL
#define UART_RBR  (unsigned char*)(UART_BASE + 0x0) //Receive
#define UART_THR  (unsigned char*)(UART_BASE + 0x0) //Send
#define UART_LSR  (unsigned char*)(UART_BASE + 0x5) //status
#define LSR_DR    (1 << 0)
#define LSR_TDRQ  (1 << 5)

char uart_getc() {
    // TODO: Implement this function
    //Read and return a single character through the UART
    while ((*UART_LSR & LSR_DR) == 0);
    char c = (char)*UART_RBR;
    return c == '\r' ? '\n' : c;
}

void uart_putc(char c) {
    // TODO: Implement this function
    // Transmit a single character through the UART
    if (c == '\n')
        uart_putc('\r');

    while ((*UART_LSR & LSR_TDRQ) == 0);
    *UART_THR = c;

}

void uart_puts(const char* s) {
    // TODO: Implement this function
    // Transmit a string throught the UART
    while(*s){
        uart_putc(*s++);
    }
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    while (1) {
        uart_putc(uart_getc());
    }
}
