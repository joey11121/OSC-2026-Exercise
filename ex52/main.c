extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);
extern void video_init();
extern void video_bmp_display(unsigned int* bmp_image, int width, int height);

#define TIME_FREQ 10000000
<<<<<<< HEAD

unsigned long get_time(){
    unsigned long now;
    asm volatile("rdtime %0":"=r"(now)); 
    return now; 
}
int usleep(unsigned int usec) {
    // TODO: Implement this function
    unsigned long start = get_time(); 
    unsigned long uticks = usec * TIME_FREQ / 1000000UL; //get the number of ticks 
    unsigned long end = start + uticks; 
    while(get_time() < end) {
        asm volatile("nop"); 
    }
    return 0;  
=======
int usleep(unsigned int usec) {
    // TODO: Implement this function
>>>>>>> ac63b0b2257b14c8614426c45c4c7f22f945b564
}

void display_video() {
#include "bird.h"
    while (1) {
        for (int f = 0; f < FRAME_COUNT; f++) {
            unsigned int* frame = (frames + (f * FRAME_WIDTH * FRAME_HEIGHT));
            video_bmp_display(frame, FRAME_WIDTH, FRAME_HEIGHT);
            usleep(50000);
        }
    }
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    // TODO: Initialize the QEMU frame buffer device
<<<<<<< HEAD
    video_init(); 
=======
>>>>>>> ac63b0b2257b14c8614426c45c4c7f22f945b564
    display_video();
}
