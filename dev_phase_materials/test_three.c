#include <stdio.h>
#include <string.h>
#include "hidapi.h"

void send_feature(hid_device *h, unsigned char *data, int len, const char *label) {
    int res = hid_send_feature_report(h, data, len);
    printf("[%s] %s (%d)\n", label, res < 0 ? "ERROR" : "OK", res);
}

int main(void) {
    hid_init();
    const char *path = "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}";
    hid_device *h = hid_open_path(path);
    if (!h) { printf("COULDNT START DEVICE: %ls\n", hid_error(NULL)); hid_exit(); return 1; }
    printf("Device Started.\n");
    unsigned char step1[9] = {0x00, 0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
    send_feature(h, step1, 9, "PREP");
    unsigned char clearFrame[65] = {0}; // 1 ID + 64 veri
    hid_write(h, clearFrame, 65);
    unsigned char step2[9] = {0x00, 0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
    send_feature(h, step2, 9, "MOD_CUSTOM");
    unsigned char step3[9] = {0x00, 0x12,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
    send_feature(h, step3, 9, "BUFFER_PREP");
    unsigned char buf[512] = {0};
    // only esc , red global offset 21
    buf[21] = 0xFF; buf[22] = 0x00; buf[23] = 0x00;
    for (int i = 0; i < 8; i++) {
        unsigned char chunk[65];
        chunk[0] = 0x00;                          // Report ID
        memcpy(chunk + 1, buf + i*64, 64);        // 64 byte veri
        int r = hid_write(h, chunk, 65);
        printf("[CHUNK %d] hid_write sonucu: %d\n", i, r);
    }
    hid_close(h);
    hid_exit();
    printf("Finished. Check ESC Result.\n");
    getchar();
    return 0;
}