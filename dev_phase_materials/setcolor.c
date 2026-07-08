#include <stdio.h>
#include <string.h>
#include "hidapi.h"

int send_report(hid_device *handle, unsigned char *data, int len, const char *label) {
    int res = hid_send_feature_report(handle, data, len);
    if (res < 0) {
        printf("[%s] Error: %ls\n", label, hid_error(handle));
    } else {
        printf("[%s] Sent, %d byte.\n", label, res);
    }
    return res;
}

int main(void) {
    if (hid_init() != 0) {
        printf("hid_init basarisiz\n");
        return 1;
    }
    const char *path = "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}";
    hid_device *handle = hid_open_path(path);
    if (!handle) {
        printf("Couldnt start device! Error: %ls\n", hid_error(NULL));
        hid_exit();
        return 1;
    }
    printf("Device started\n");
    //Color command test: [ReportID=0][0x14][0x00][0x01][R][G][B][0x00][0x00]
    unsigned char colorCmd[9] = { 0x00, 0x14, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00 };
    send_report(handle, colorCmd, sizeof(colorCmd), "COLOR");
    // Mod command test: [ReportID=0][0x08][0x02][0x01][0x03][0x32][0x08][0x00][0x00]
    unsigned char modeCmd[9] = { 0x00, 0x08, 0x02, 0x01, 0x03, 0x32, 0x08, 0x00, 0x00 };
    send_report(handle, modeCmd, sizeof(modeCmd), "MOD");
    hid_close(handle);
    hid_exit();
    printf("exit\n");
    getchar();
    return 0;
}