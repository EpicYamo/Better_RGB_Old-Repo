// Game Garaj (Tongfang/ITE8291) Ozel RGB Kontrol Motoru
// VID_048D PID_600B, Interface 1 (MI_01) uzerinden per-key kontrol
// Protokol reverse-engineering ile cozuldu, offsetler ampirik olarak dogrulandi.
//
// Derleme:
//   gcc rgb_engine.c C:\dev\hidapi\windows\hid.c -IC:\dev\hidapi\hidapi -o rgb_engine.exe -lsetupapi -lhid
//
// Kullanim: rgb_engine.exe   (calisirken F12'ye basarak cikabilirsin)

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include "hidapi.h"

#define DEVICE_PATH "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
#define FRAME_DELAY_MS 10   // ~30 FPS, test edilerek dogrulanmis guvenli hiz
#define BUF_SIZE 512

// ---------------------------------------------------------------
// KEYMAP: reverse-engineering ile cikarilan offset + fiziksel (row,col) tablosu
// row/col, dalga gibi konum-bazli efektler icin yaklasik fiziksel yerlesimi verir.
// ---------------------------------------------------------------
typedef struct {
    const char *name;
    int offset;
    int row;
    int col;
} KeyEntry;

static const KeyEntry KEYMAP[] = {
    // Row 0 - Esc + F tuslari + ust blok (20 tus)
    {"Esc",21,0,0},{"F1",45,0,1},{"F2",69,0,2},{"F3",93,0,3},{"F4",117,0,4},
    {"F5",141,0,5},{"F6",165,0,6},{"F7",189,0,7},{"F8",213,0,8},{"F9",237,0,9},
    {"F10",261,0,10},{"F11",285,0,11},{"F12",309,0,12},{"FnLockCam",333,0,13},
    {"PrtSc",357,0,14},{"Del",381,0,15},{"Home",405,0,16},{"PgUp",429,0,17},
    {"PgDn",453,0,18},{"End",477,0,19},
    // Row 1 - sayi sirasi (18 tus)
    {"Grave",17,1,0},{"1",41,1,1},{"2",65,1,2},{"3",89,1,3},{"4",113,1,4},
    {"5",137,1,5},{"6",161,1,6},{"7",185,1,7},{"8",209,1,8},{"9",233,1,9},
    {"0",257,1,10},{"Minus",281,1,11},{"Equals",305,1,12},{"Backspace",353,1,13},
    {"NumLock",377,1,14},{"NumpadDiv",401,1,15},{"NumpadMul",425,1,16},{"NumpadMinus",449,1,17},
    // Row 2 - QWERTY (18 tus)
    {"Tab",13,2,0},{"Q",61,2,1},{"W",85,2,2},{"E",109,2,3},{"R",133,2,4},
    {"T",157,2,5},{"Y",181,2,6},{"U",205,2,7},{"I",229,2,8},{"O",253,2,9},
    {"P",277,2,10},{"BracketL",301,2,11},{"BracketR",325,2,12},{"Enter",349,2,13},
    {"Numpad7",373,2,14},{"Numpad8",397,2,15},{"Numpad9",421,2,16},{"NumpadPlus",445,2,17},
    // Row 3 - ASDF (16 tus)
    {"ShiftL",9,3,0},{"A",57,3,1},{"S",81,3,2},{"D",105,3,3},{"F",129,3,4},
    {"G",153,3,5},{"H",177,3,6},{"J",201,3,7},{"K",225,3,8},{"L",249,3,9},
    {"Semicolon",273,3,10},{"Quote",297,3,11},{"Backslash",321,3,12},
    {"Numpad4",369,3,13},{"Numpad5",393,3,14},{"Numpad6",417,3,15},
    // Row 4 - ZXCV (17 tus)
    {"ShiftR_ISO",5,4,0},{"ISO_Backslash",53,4,1},{"Z",77,4,2},{"X",101,4,3},
    {"C",125,4,4},{"V",149,4,5},{"B",173,4,6},{"N",197,4,7},{"M",221,4,8},
    {"Comma",245,4,9},{"Period",269,4,10},{"Slash",293,4,11},{"ShiftR_Big",341,4,12},
    {"Numpad1",365,4,13},{"Numpad2",389,4,14},{"Numpad3",413,4,15},{"NumpadEnter",437,4,16},
    // Row 5 - alt satir (10 tus)
    {"CtrlL",1,5,0},{"Fn",49,5,1},{"Windows",73,5,2},{"AltL",97,5,3},
    {"Space",169,5,4},{"AltGr",241,5,5},{"CopilotKey",289,5,6},{"ArrowUp",337,5,7},
    {"Numpad0",385,5,8},{"NumpadDecimal",409,5,9},
    // Row 6 - yon tuslari (3 tus) - fiziksel olarak klavyenin SAGINDA, o yuzden
    // col degerleri row5'in sag tarafindaki (AltGr/Copilot/UpArrow) tuslarina yakin secildi
    {"ArrowLeft",313,6,6},{"ArrowDown",433,6,7},{"ArrowRight",361,6,8},
};
#define KEYMAP_COUNT (sizeof(KEYMAP)/sizeof(KEYMAP[0]))

// ---------------------------------------------------------------
// DUSUK SEVIYE PROTOKOL KATMANI
// ---------------------------------------------------------------
static hid_device *g_h = NULL;

int enter_custom_mode(void) {
    unsigned char step1[9] = {0x00,0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
    if (hid_send_feature_report(g_h, step1, 9) < 0) return -1;
    unsigned char clearFrame[65] = {0};
    hid_write(g_h, clearFrame, 65);
    unsigned char step2[9] = {0x00,0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
    if (hid_send_feature_report(g_h, step2, 9) < 0) return -1;
    return 0;
}

// Bir 512 byte'lik frame'i cihaza gonderir (her seferinde BUFFER_HAZIRLIK ile birlikte)
int send_frame(unsigned char *buf) {
    unsigned char step3[9] = {0x00,0x12,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
    if (hid_send_feature_report(g_h, step3, 9) < 0) return -1;
    for (int i = 0; i < 8; i++) {
        unsigned char chunk[65];
        chunk[0] = 0x00;
        memcpy(chunk+1, buf + i*64, 64);
        if (hid_write(g_h, chunk, 65) < 0) return -1;
    }
    return 0;
}

void set_key(unsigned char *buf, int offset, unsigned char r, unsigned char g, unsigned char b) {
    if (offset < 0 || offset+2 >= BUF_SIZE) return;
    buf[offset] = r; buf[offset+1] = g; buf[offset+2] = b;
}

int should_exit(void) {
    // F12'ye basarak programdan cikabilirsin
    return (GetAsyncKeyState(VK_F12) & 0x8000) != 0;
}

// ---------------------------------------------------------------
// EFEKT 1: NEFES ALMA (Breathing) - tum klavye tek renk, parlaklik sinus ile degisir
// ---------------------------------------------------------------
void effect_breathing(void) {
    int r,g,b;
    printf("Renk gir (R G B, 0-255 arasi, ornek: 0 120 255): ");
    scanf("%d %d %d", &r, &g, &b);
    printf("NEFES ALMA modu. Cikmak icin F12.\n");
    double t = 0.0;
    unsigned char baseR = (unsigned char)r, baseG = (unsigned char)g, baseB = (unsigned char)b;
    while (!should_exit()) {
        double brightness = (sin(t) + 1.0) / 2.0; // 0..1 arasi
        unsigned char r = (unsigned char)(baseR * brightness);
        unsigned char g = (unsigned char)(baseG * brightness);
        unsigned char b = (unsigned char)(baseB * brightness);

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i = 0; i < KEYMAP_COUNT; i++)
            set_key(buf, KEYMAP[i].offset, r, g, b);

        send_frame(buf);
        t += 0.12; // nefes hizi
        Sleep(FRAME_DELAY_MS);
    }
}

// ---------------------------------------------------------------
// EFEKT 2: DALGA (Wave) - renk, sutun (col) pozisyonuna gore kayar (HSV tabanli gokkusagi)
// ---------------------------------------------------------------
void hsv_to_rgb(double h, unsigned char *r, unsigned char *g, unsigned char *b) {
    h = fmod(h, 360.0);
    if (h < 0) h += 360.0;
    double c = 1.0, x = c * (1 - fabs(fmod(h/60.0, 2)-1)), m = 0.0;
    double rr,gg,bb;
    if (h < 60){rr=c;gg=x;bb=0;} else if (h<120){rr=x;gg=c;bb=0;}
    else if (h<180){rr=0;gg=c;bb=x;} else if (h<240){rr=0;gg=x;bb=c;}
    else if (h<300){rr=x;gg=0;bb=c;} else {rr=c;gg=0;bb=x;}
    *r = (unsigned char)((rr+m)*255);
    *g = (unsigned char)((gg+m)*255);
    *b = (unsigned char)((bb+m)*255);
}

void effect_wave(void) {
    printf("DALGA modu. Cikmak icin F12.\n");
    double t = 0.0;
    while (!should_exit()) {
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i = 0; i < KEYMAP_COUNT; i++) {
            double hue = t + KEYMAP[i].col * 18.0; // sutuna gore faz kaymasi
            unsigned char r,g,b;
            hsv_to_rgb(hue, &r, &g, &b);
            set_key(buf, KEYMAP[i].offset, r, g, b);
        }
        send_frame(buf);
        t += 4.0; // dalga kayma hizi (derece/frame)
        Sleep(FRAME_DELAY_MS);
    }
}

// ---------------------------------------------------------------
// EFEKT 3: RASTGELE PARILTI (Sparkle)
// ---------------------------------------------------------------
void effect_sparkle(void) {
    int density;
    printf("Yogunluk gir (1-10 arasi, 10=en yogun): ");
    scanf("%d", &density);
    if (density < 1) density = 1;
    if (density > 10) density = 10;
    int spawnChance = density * 4; // 1->4%%, 10->40%% civari deneme olasiligi

    printf("RASTGELE PARILTI modu. Cikmak icin F12.\n");
    // Her tus icin kalan "parlama suresi" (frame sayisi) tutuyoruz
    int *life = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr = (unsigned char*)calloc(KEYMAP_COUNT,1);
    unsigned char *gg = (unsigned char*)calloc(KEYMAP_COUNT,1);
    unsigned char *bb = (unsigned char*)calloc(KEYMAP_COUNT,1);

    while (!should_exit()) {
        // Yogunluga bagli olasilikla yeni bir tusu parlat
        if ((rand() % 100) < spawnChance) {
            int idx = rand() % KEYMAP_COUNT;
            life[idx] = 15; // ~0.5 saniye (15 frame * ~33ms degil, 10ms*15=150ms, istersen arttir)
            rr[idx] = rand() % 256; gg[idx] = rand() % 256; bb[idx] = rand() % 256;
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i = 0; i < KEYMAP_COUNT; i++) {
            if (life[i] > 0) {
                double fade = life[i] / 15.0;
                set_key(buf, KEYMAP[i].offset,
                        (unsigned char)(rr[i]*fade),
                        (unsigned char)(gg[i]*fade),
                        (unsigned char)(bb[i]*fade));
                life[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(life); free(rr); free(gg); free(bb);
}

// ---------------------------------------------------------------
// EFEKT 4: TUSA BASINCA TEPKI (Reactive typing)
// ---------------------------------------------------------------
// Windows Virtual-Key kodunu bizim KEYMAP offsetine baglayan basit tablo.
// Tam kapsamli degil ama en cok kullanilan tuslarin coğunu kapsiyor.
int vk_to_keymap_index(int vk) {
    for (size_t i = 0; i < KEYMAP_COUNT; i++) {
        const char *n = KEYMAP[i].name;
        if (vk >= 'A' && vk <= 'Z' && n[0] == (char)vk && n[1] == '\0') return (int)i;
        if (vk >= '0' && vk <= '9' && n[0] == (char)vk && n[1] == '\0') return (int)i;
    }
    // ozel tuslar
    switch (vk) {
        case VK_ESCAPE: return -100; // ozel isliyoruz asagida (isim eslesmesi zor oldugu icin index donduremeyiz direkt)
        case VK_SPACE:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"Space")==0) return (int)i;
            break;
        case VK_RETURN:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"Enter")==0) return (int)i;
            break;
        case VK_BACK:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"Backspace")==0) return (int)i;
            break;
        case VK_TAB:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"Tab")==0) return (int)i;
            break;
        case VK_LSHIFT:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ShiftL")==0) return (int)i;
            break;
        case VK_RSHIFT:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ShiftR_Big")==0) return (int)i;
            break;
        case VK_LCONTROL:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"CtrlL")==0) return (int)i;
            break;
        case VK_LMENU:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"AltL")==0) return (int)i;
            break;
        case VK_RMENU:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"AltGr")==0) return (int)i;
            break;
        case VK_LEFT:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ArrowLeft")==0) return (int)i;
            break;
        case VK_RIGHT:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ArrowRight")==0) return (int)i;
            break;
        case VK_UP:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ArrowUp")==0) return (int)i;
            break;
        case VK_DOWN:
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name,"ArrowDown")==0) return (int)i;
            break;
    }
    return -1;
}

void effect_reactive(void) {
    int mode, fixedR=255, fixedG=255, fixedB=255;
    printf("Renk modu sec: 1) Sabit renk  2) Rastgele renk (her basista farkli): ");
    scanf("%d", &mode);
    if (mode == 1) {
        printf("Renk gir (R G B, 0-255 arasi): ");
        scanf("%d %d %d", &fixedR, &fixedG, &fixedB);
    }

    printf("TUSA BASINCA TEPKI modu. Cikmak icin F12 (F12 isik yakmaz, sadece cikis icin kullanilir).\n");
    int *life = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr = (unsigned char*)calloc(KEYMAP_COUNT,1);
    unsigned char *gg = (unsigned char*)calloc(KEYMAP_COUNT,1);
    unsigned char *bb = (unsigned char*)calloc(KEYMAP_COUNT,1);
    for (size_t i=0;i<KEYMAP_COUNT;i++){ rr[i]=(unsigned char)fixedR; gg[i]=(unsigned char)fixedG; bb[i]=(unsigned char)fixedB; }

    while (!should_exit()) {
        // A-Z, 0-9 ve ozel tuslari kontrol et
        for (int vk = 'A'; vk <= 'Z'; vk++) {
            if (GetAsyncKeyState(vk) & 0x8000) {
                int idx = vk_to_keymap_index(vk);
                if (idx >= 0) {
                    life[idx] = 20;
                    if (mode == 2) { rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256; }
                }
            }
        }
        for (int vk = '0'; vk <= '9'; vk++) {
            if (GetAsyncKeyState(vk) & 0x8000) {
                int idx = vk_to_keymap_index(vk);
                if (idx >= 0) {
                    life[idx] = 20;
                    if (mode == 2) { rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256; }
                }
            }
        }
        int specials[] = {VK_SPACE,VK_RETURN,VK_BACK,VK_TAB,VK_LSHIFT,VK_RSHIFT,
                           VK_LCONTROL,VK_LMENU,VK_RMENU,VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN};
        for (size_t s = 0; s < sizeof(specials)/sizeof(specials[0]); s++) {
            if (GetAsyncKeyState(specials[s]) & 0x8000) {
                int idx = vk_to_keymap_index(specials[s]);
                if (idx >= 0) {
                    life[idx] = 20;
                    if (mode == 2) { rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256; }
                }
            }
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i = 0; i < KEYMAP_COUNT; i++) {
            if (life[i] > 0) {
                double fade = life[i] / 20.0;
                set_key(buf, KEYMAP[i].offset,
                        (unsigned char)(rr[i]*fade), (unsigned char)(gg[i]*fade), (unsigned char)(bb[i]*fade));
                life[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(life); free(rr); free(gg); free(bb);
}

// ---------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------
int main(void) {
    srand((unsigned int)time(NULL));
    hid_init();
    g_h = hid_open_path(DEVICE_PATH);
    if (!g_h) {
        printf("Cihaz acilamadi: %ls\n", hid_error(NULL));
        hid_exit();
        return 1;
    }
    printf("Cihaz acildi.\n");
    if (enter_custom_mode() < 0) {
        printf("Per-key moduna girilemedi!\n");
        hid_close(g_h); hid_exit();
        return 1;
    }

    int choice;
    printf("\nHangi efekt?\n");
    printf(" 1) Nefes alma (Breathing)\n");
    printf(" 2) Dalga (Wave)\n");
    printf(" 3) Rastgele Parilti (Sparkle)\n");
    printf(" 4) Tusa basinca tepki (Reactive typing)\n");
    printf("Secim: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: effect_breathing(); break;
        case 2: effect_wave(); break;
        case 3: effect_sparkle(); break;
        case 4: effect_reactive(); break;
        default: printf("Gecersiz secim.\n");
    }

    hid_close(g_h);
    hid_exit();
    printf("Cikildi.\n");
    return 0;
}