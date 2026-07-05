#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "hidapi.h"

int main(void) {
    hid_init();
    const char *path = "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}";
    hid_device *h = hid_open_path(path);
    if (!h) { printf("couldnt start device: %ls\n", hid_error(NULL)); hid_exit(); return 1; }
    printf("Device started.\n");
    int positions[] = {21,45,69,93,117,141,165,189,213,237,261,285,309};
    int numPos = sizeof(positions)/sizeof(positions[0]);
    for (int f = 0; f < 6; f++)
    {
        printf("--- Frame %d, position=%d starting ---\n", f, positions[f % numPos]);
        unsigned char step1[9] = {0x00, 0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
        int r1 = hid_send_feature_report(h, step1, 9);
        printf("  PREP: %d\n", r1);
        unsigned char clearFrame[65] = {0};
        int rc = hid_write(h, clearFrame, 65);
        printf("  CLEAR: %d\n", rc);
        unsigned char step2[9] = {0x00, 0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
        int r2 = hid_send_feature_report(h, step2, 9);
        printf("  MOD_CUSTOM: %d\n", r2);
        unsigned char step3[9] = {0x00, 0x12,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
        int r3 = hid_send_feature_report(h, step3, 9);
        printf("  BUFFER_PREP: %d\n", r3);
        unsigned char buf[512] = {0};
        int pos = positions[f % numPos];
        buf[pos] = 0xFF; buf[pos+1] = 0x00; buf[pos+2] = 0x00;
        for (int i = 0; i < 8; i++) {
            unsigned char chunk[65];
            chunk[0] = 0x00;
            memcpy(chunk+1, buf+i*64, 64);
            int r = hid_write(h, chunk, 65);
            printf("  CHUNK %d: %d\n", i, r);
        }
        printf("--- Frame %d Sent, Check keyboard, wautung 2secs ---\n", f);
        Sleep(2000);
    }
    hid_close(h);
    hid_exit();
    printf("finished...\n");
    getchar();
    return 0;
}