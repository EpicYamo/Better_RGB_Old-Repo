#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "hidapi.h"

int main(void) {
    hid_init();
    const char *path = "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}";
    hid_device *h = hid_open_path(path);
    if (!h) { printf("Couldnt start device: %ls\n", hid_error(NULL)); hid_exit(); return 1; }
    printf("Device started.\n");
    int positions[] = {21,45,69,93,117,141,165,189,213,237,261,285,309};
    int numPos = sizeof(positions)/sizeof(positions[0]);
    int delayMs = 5; // test et
    int totalFrames = 40;
    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);
    for (int f = 0; f < totalFrames; f++) {
        unsigned char step1[9] = {0x00, 0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
        hid_send_feature_report(h, step1, 9);
        unsigned char clearFrame[65] = {0};
        hid_write(h, clearFrame, 65);
        unsigned char step2[9] = {0x00, 0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
        hid_send_feature_report(h, step2, 9);
        unsigned char step3[9] = {0x00, 0x12,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
        hid_send_feature_report(h, step3, 9);
        unsigned char buf[512] = {0};
        int pos = positions[f % numPos];
        buf[pos] = 0xFF; buf[pos+1] = 0x00; buf[pos+2] = 0x00;
        for (int i = 0; i < 8; i++) {
            unsigned char chunk[65];
            chunk[0] = 0x00;
            memcpy(chunk+1, buf+i*64, 64);
            hid_write(h, chunk, 65);
        }
        if (delayMs > 0) Sleep(delayMs);
    }
    QueryPerformanceCounter(&t1);
    double elapsedSec = (double)(t1.QuadPart - t0.QuadPart) / freq.QuadPart;
    printf("Real FPS: %.1f\n", totalFrames / elapsedSec);
    hid_close(h);
    hid_exit();
    printf("Finished...\n");
    getchar();
    return 0;
}