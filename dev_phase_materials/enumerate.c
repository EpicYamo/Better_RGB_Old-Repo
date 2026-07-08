#include <stdio.h>
#include <wchar.h>
#include "hidapi.h"

int main(void) {
    if (hid_init() != 0) {
        printf("hid_init fail\n");
        return 1;
    }
    struct hid_device_info *devs, *cur;
    devs = hid_enumerate(0x048D, 0x600B);
    cur = devs;
    if (!cur) {
        printf("dev not found, VID/PID might be wrong or GCUBridge(game garaj servisi) is interrupting\n");
    }
    while (cur) {
        printf("Path: %s\n", cur->path);
        printf("  Interface Number: %d\n", cur->interface_number);
        printf("  Usage Page: 0x%04hx  Usage: 0x%04hx\n", cur->usage_page, cur->usage);
        printf("  Manufacturer: %ls\n", cur->manufacturer_string ? cur->manufacturer_string : L"(yok)");
        printf("  Product: %ls\n", cur->product_string ? cur->product_string : L"(yok)");
        printf("------\n");
        cur = cur->next;
    }
    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}
