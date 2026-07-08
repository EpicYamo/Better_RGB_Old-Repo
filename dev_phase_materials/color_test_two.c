#include <stdio.h>
#include <string.h>
#include "hidapi.h"

void send_feature(hid_device *h, unsigned char *data, int len, const char *label) {
    int res = hid_send_feature_report(h, data, len);
    printf("[%s] %s (%d)\n", label, res < 0 ? "HATA" : "OK", res);
}

int main(void) {
    hid_init();
    const char *path = "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}";
    hid_device *h = hid_open_path(path);
    if (!h) { printf("Couldnt start device: %ls\n", hid_error(NULL)); hid_exit(); return 1; }
    printf("Device Started.\n");
    unsigned char step1[9] = {0x00, 0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
    send_feature(h, step1, 9, "PREP");
    // Bos interrupt frame temizleme with hid_write
    unsigned char clearFrame[64] = {0};
    int wres = hid_write(h, clearFrame, 64);
    printf("[CLEAR] hid_write result: %d\n", wres);
    unsigned char step2[9] = {0x00, 0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
    send_feature(h, step2, 9, "MOD_CUSTOM");
    unsigned char step3[9] = {0x00, 0x12,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
    send_feature(h, step3, 9, "BUFFER_PREP");
    // 512 byte per-key buffer, hepsi sifir yani siyah
    unsigned char buf[512] = {0};
    // tersine muhendislikle elde ettigim keymap.jsondan bazi offsetler: Esc=21, Enter=349, W=85, A=57, S=81, D=105
    // W = kirmizi
    buf[85] = 0xFF; buf[86] = 0x00; buf[87] = 0x00;
    // A = yesil
    buf[57] = 0x00; buf[58] = 0xFF; buf[59] = 0x00;
    // S = mavi
    buf[81] = 0x00; buf[82] = 0x00; buf[83] = 0xFF;
    // D = beyaz
    buf[105] = 0xFF; buf[106] = 0xFF; buf[107] = 0xFF;
    // 512 byte 8 adet 64 byte chunk halinde gonder
    for (int i = 0; i < 8; i++) {
        int r = hid_write(h, buf + i*64, 64);
        printf("[CHUNK %d] hid_write result: %d\n", i, r);
    }
    hid_close(h);
    hid_exit();
    printf("Finished. WASD\n");
    getchar();
    return 0;
}