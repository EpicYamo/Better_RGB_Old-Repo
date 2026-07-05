// Game Garaj (Tongfang/ITE8291) Better_RGB - (GUI) VERSIYONU
// Version 1
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "hidapi.h"

#define DEVICE_PATH "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
#define FRAME_DELAY_MS 10
#define BUF_SIZE 512

#define IDC_BTN_BREATH   101
#define IDC_BTN_WAVE     102
#define IDC_BTN_SPARKLE  103
#define IDC_BTN_REACTIVE 104
#define IDC_BTN_STOP     105
#define IDC_SLIDER_DENS  106
#define IDC_BTN_COLOR_BREATH 107
#define IDC_BTN_COLOR_REACT  108
#define IDC_SWATCH_BREATH    109
#define IDC_SWATCH_REACT     110
#define IDC_STATIC_STATUS    111
#define IDC_STATIC_DENSLABEL 112
#define IDC_BTN_REACT_RANDOM 113

// KEYMAP
typedef struct { const char *name; int offset; int row; int col; } KeyEntry;

static const KeyEntry KEYMAP[] = {
    {"Esc",21,0,0},{"F1",45,0,1},{"F2",69,0,2},{"F3",93,0,3},{"F4",117,0,4},
    {"F5",141,0,5},{"F6",165,0,6},{"F7",189,0,7},{"F8",213,0,8},{"F9",237,0,9},
    {"F10",261,0,10},{"F11",285,0,11},{"F12",309,0,12},{"FnLockCam",333,0,13},
    {"PrtSc",357,0,14},{"Del",381,0,15},{"Home",405,0,16},{"PgUp",429,0,17},
    {"PgDn",453,0,18},{"End",477,0,19},
    {"Grave",17,1,0},{"1",41,1,1},{"2",65,1,2},{"3",89,1,3},{"4",113,1,4},
    {"5",137,1,5},{"6",161,1,6},{"7",185,1,7},{"8",209,1,8},{"9",233,1,9},
    {"0",257,1,10},{"Minus",281,1,11},{"Equals",305,1,12},{"Backspace",353,1,13},
    {"NumLock",377,1,14},{"NumpadDiv",401,1,15},{"NumpadMul",425,1,16},{"NumpadMinus",449,1,17},
    {"Tab",13,2,0},{"Q",61,2,1},{"W",85,2,2},{"E",109,2,3},{"R",133,2,4},
    {"T",157,2,5},{"Y",181,2,6},{"U",205,2,7},{"I",229,2,8},{"O",253,2,9},
    {"P",277,2,10},{"BracketL",301,2,11},{"BracketR",325,2,12},{"Enter",349,2,13},
    {"Numpad7",373,2,14},{"Numpad8",397,2,15},{"Numpad9",421,2,16},{"NumpadPlus",445,2,17},
    {"CapsLock",9,3,0},{"A",57,3,1},{"S",81,3,2},{"D",105,3,3},{"F",129,3,4},
    {"G",153,3,5},{"H",177,3,6},{"J",201,3,7},{"K",225,3,8},{"L",249,3,9},
    {"Semicolon",273,3,10},{"Quote",297,3,11},{"Backslash",321,3,12},
    {"Numpad4",369,3,13},{"Numpad5",393,3,14},{"Numpad6",417,3,15},
    {"ShiftL",29,4,0},{"ISO_Backslash",53,4,1},{"Z",77,4,2},{"X",101,4,3},
    {"C",125,4,4},{"V",149,4,5},{"B",173,4,6},{"N",197,4,7},{"M",221,4,8},
    {"Comma",245,4,9},{"Period",269,4,10},{"Slash",293,4,11},{"ShiftR",341,4,12},
    {"Numpad1",365,4,13},{"Numpad2",389,4,14},{"Numpad3",413,4,15},{"NumpadEnter",437,4,16},
    {"CtrlL",1,5,0},{"Fn",49,5,1},{"Windows",73,5,2},{"AltL",97,5,3},
    {"Space",169,5,4},{"AltGr",241,5,5},{"CopilotKey",289,5,6},{"ArrowUp",337,5,7},
    {"Numpad0",385,5,8},{"NumpadDecimal",409,5,9},
    {"ArrowLeft",313,6,6},{"ArrowDown",433,6,7},{"ArrowRight",361,6,8},
};
#define KEYMAP_COUNT (sizeof(KEYMAP)/sizeof(KEYMAP[0]))

// GLOBAL DURUM
static hid_device *g_h = NULL;
static HANDLE g_effectThread = NULL;
static volatile LONG g_stopFlag = 0;
static volatile LONG g_activeEffect = 0;
static volatile LONG g_sparkleDensity = 5;
static COLORREF g_breathColor = RGB(0, 120, 255);
static COLORREF g_reactiveColor = RGB(255, 255, 255);
static volatile LONG g_reactiveRandomMode = 1;
static HWND g_hStatus, g_hSwatchBreath, g_hSwatchReact, g_hSlider;

// PROTOKOL KATMANI
int enter_custom_mode(void) {
    unsigned char step1[9] = {0x00,0x12,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
    if (hid_send_feature_report(g_h, step1, 9) < 0) return -1;
    unsigned char clearFrame[65] = {0};
    hid_write(g_h, clearFrame, 65);
    unsigned char step2[9] = {0x00,0x08,0x02,0x33,0x00,0x32,0x00,0x00,0x00};
    if (hid_send_feature_report(g_h, step2, 9) < 0) return -1;
    return 0;
}
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
void hsv_to_rgb(double h, unsigned char *r, unsigned char *g, unsigned char *b) {
    h = fmod(h, 360.0); if (h < 0) h += 360.0;
    double c = 1.0, x = c * (1 - fabs(fmod(h/60.0, 2)-1));
    double rr,gg,bb;
    if (h < 60){rr=c;gg=x;bb=0;} else if (h<120){rr=x;gg=c;bb=0;}
    else if (h<180){rr=0;gg=c;bb=x;} else if (h<240){rr=0;gg=x;bb=c;}
    else if (h<300){rr=x;gg=0;bb=c;} else {rr=c;gg=0;bb=x;}
    *r=(unsigned char)(rr*255); *g=(unsigned char)(gg*255); *b=(unsigned char)(bb*255);
}

// EFEKTLER
DWORD WINAPI effect_breathing(LPVOID p) {
    (void)p;
    double t = 0.0;
    while (!g_stopFlag) {
        unsigned char br = GetRValue(g_breathColor), bg = GetGValue(g_breathColor), bb = GetBValue(g_breathColor);
        double bright = (sin(t) + 1.0) / 2.0;
        unsigned char r=(unsigned char)(br*bright), g=(unsigned char)(bg*bright), b=(unsigned char)(bb*bright);
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) set_key(buf, KEYMAP[i].offset, r,g,b);
        send_frame(buf);
        t += 0.12;
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_wave(LPVOID p) {
    (void)p;
    double t = 0.0;
    while (!g_stopFlag) {
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            double hue = t + KEYMAP[i].col * 18.0;
            unsigned char r,g,b;
            hsv_to_rgb(hue,&r,&g,&b);
            set_key(buf, KEYMAP[i].offset, r,g,b);
        }
        send_frame(buf);
        t += 4.0;
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_sparkle(LPVOID p) {
    (void)p;
    int *life = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr=(unsigned char*)calloc(KEYMAP_COUNT,1), *gg=(unsigned char*)calloc(KEYMAP_COUNT,1), *bb=(unsigned char*)calloc(KEYMAP_COUNT,1);
    while (!g_stopFlag) {
        int spawnChance = (int)g_sparkleDensity * 4;
        if ((rand()%100) < spawnChance) {
            int idx = rand() % KEYMAP_COUNT;
            life[idx] = 15;
            rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256;
        }
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            if (life[i] > 0) {
                double fade = life[i]/15.0;
                set_key(buf, KEYMAP[i].offset, (unsigned char)(rr[i]*fade),(unsigned char)(gg[i]*fade),(unsigned char)(bb[i]*fade));
                life[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(life); free(rr); free(gg); free(bb);
    return 0;
}

int vk_to_keymap_index(int vk) {
    for (size_t i=0;i<KEYMAP_COUNT;i++) {
        const char *n = KEYMAP[i].name;
        if (((vk>='A'&&vk<='Z')||(vk>='0'&&vk<='9')) && n[0]==(char)vk && n[1]=='\0') return (int)i;
    }
    struct { int vk; const char* name; } table[] = {
        {VK_SPACE,"Space"},{VK_RETURN,"Enter"},{VK_BACK,"Backspace"},{VK_TAB,"Tab"},
        {VK_LSHIFT,"ShiftL"},{VK_RSHIFT,"ShiftR"},{VK_CAPITAL,"CapsLock"},{VK_LCONTROL,"CtrlL"},
        {VK_LMENU,"AltL"},{VK_RMENU,"AltGr"},{VK_LEFT,"ArrowLeft"},{VK_RIGHT,"ArrowRight"},
        {VK_UP,"ArrowUp"},{VK_DOWN,"ArrowDown"},{VK_OEM_102,"ISO_Backslash"},{VK_LWIN,"Windows"},
        {VK_OEM_4,"BracketL"},{VK_OEM_6,"BracketR"},{VK_OEM_1,"Semicolon"},{VK_OEM_7,"Quote"},
        {VK_OEM_5,"Backslash"},{VK_OEM_COMMA,"Comma"},{VK_OEM_PERIOD,"Period"},{VK_OEM_2,"Slash"},
        {VK_OEM_MINUS,"Minus"},{VK_OEM_PLUS,"Equals"},{VK_OEM_3,"Grave"},
        {VK_NUMPAD0,"Numpad0"},{VK_NUMPAD1,"Numpad1"},{VK_NUMPAD2,"Numpad2"},{VK_NUMPAD3,"Numpad3"},
        {VK_NUMPAD4,"Numpad4"},{VK_NUMPAD5,"Numpad5"},{VK_NUMPAD6,"Numpad6"},{VK_NUMPAD7,"Numpad7"},
        {VK_NUMPAD8,"Numpad8"},{VK_NUMPAD9,"Numpad9"},{VK_DECIMAL,"NumpadDecimal"},{VK_ADD,"NumpadPlus"},
        {VK_SUBTRACT,"NumpadMinus"},{VK_MULTIPLY,"NumpadMul"},{VK_DIVIDE,"NumpadDiv"}
    };
    for (size_t k=0;k<sizeof(table)/sizeof(table[0]);k++) {
        if (vk == table[k].vk) {
            for (size_t i=0;i<KEYMAP_COUNT;i++) if (strcmp(KEYMAP[i].name, table[k].name)==0) return (int)i;
        }
    }
    return -1;
}

DWORD WINAPI effect_reactive(LPVOID p) {
    (void)p;
    int *life = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr=(unsigned char*)calloc(KEYMAP_COUNT,1), *gg=(unsigned char*)calloc(KEYMAP_COUNT,1), *bb=(unsigned char*)calloc(KEYMAP_COUNT,1);

    while (!g_stopFlag) {
        unsigned char fr=GetRValue(g_reactiveColor), fg=GetGValue(g_reactiveColor), fb=GetBValue(g_reactiveColor);
        for (int vk='A'; vk<='Z'; vk++) if (GetAsyncKeyState(vk)&0x8000) {
            int idx = vk_to_keymap_index(vk);
            if (idx>=0) { life[idx]=20; if (g_reactiveRandomMode){rr[idx]=rand()%256;gg[idx]=rand()%256;bb[idx]=rand()%256;} else {rr[idx]=fr;gg[idx]=fg;bb[idx]=fb;} }
        }
        for (int vk='0'; vk<='9'; vk++) if (GetAsyncKeyState(vk)&0x8000) {
            int idx = vk_to_keymap_index(vk);
            if (idx>=0) { life[idx]=20; if (g_reactiveRandomMode){rr[idx]=rand()%256;gg[idx]=rand()%256;bb[idx]=rand()%256;} else {rr[idx]=fr;gg[idx]=fg;bb[idx]=fb;} }
        }
        int specials[] = {VK_SPACE,VK_RETURN,VK_BACK,VK_TAB,VK_LSHIFT,VK_RSHIFT,VK_CAPITAL,VK_LCONTROL,VK_LMENU,VK_RMENU,VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_OEM_102,VK_LWIN,
                           VK_OEM_4,VK_OEM_6,VK_OEM_1,VK_OEM_7,VK_OEM_5,VK_OEM_COMMA,VK_OEM_PERIOD,VK_OEM_2,VK_OEM_MINUS,VK_OEM_PLUS,VK_OEM_3,
                           VK_NUMPAD0,VK_NUMPAD1,VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD6,VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD9,
                           VK_DECIMAL,VK_ADD,VK_SUBTRACT,VK_MULTIPLY,VK_DIVIDE};
        for (size_t s=0;s<sizeof(specials)/sizeof(specials[0]);s++) if (GetAsyncKeyState(specials[s])&0x8000) {
            int idx = vk_to_keymap_index(specials[s]);
            if (idx>=0) { life[idx]=20; if (g_reactiveRandomMode){rr[idx]=rand()%256;gg[idx]=rand()%256;bb[idx]=rand()%256;} else {rr[idx]=fr;gg[idx]=fg;bb[idx]=fb;} }
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            if (life[i] > 0) {
                double fade = life[i]/20.0;
                set_key(buf, KEYMAP[i].offset, (unsigned char)(rr[i]*fade),(unsigned char)(gg[i]*fade),(unsigned char)(bb[i]*fade));
                life[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(life); free(rr); free(gg); free(bb);
    return 0;
}

// THREAD YONETIMI
void set_status(const wchar_t *text) {
    if (g_hStatus) SetWindowText(g_hStatus, text);
}

void stop_current_effect(void) {
    if (g_effectThread) {
        InterlockedExchange(&g_stopFlag, 1);
        WaitForSingleObject(g_effectThread, 2000);
        CloseHandle(g_effectThread);
        g_effectThread = NULL;
    }
    g_activeEffect = 0;
}

void start_effect(int id, LPTHREAD_START_ROUTINE fn, const wchar_t *label) {
    stop_current_effect();
    InterlockedExchange(&g_stopFlag, 0);
    g_activeEffect = id;
    g_effectThread = CreateThread(NULL, 0, fn, NULL, 0, NULL);
    set_status(label);
}

// RENK KUTUSU (SWATCH) - kucuk ozel pencere sinifi
LRESULT CALLBACK SwatchProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        COLORREF col = (COLORREF)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        HBRUSH br = CreateSolidBrush(col);
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        FrameRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void set_swatch_color(HWND hwnd, COLORREF col) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)col);
    InvalidateRect(hwnd, NULL, TRUE);
}

COLORREF pick_color(HWND hwnd, COLORREF initial) {
    static COLORREF custom[16] = {0};
    CHOOSECOLOR cc = {0};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.rgbResult = initial;
    cc.lpCustColors = custom;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&cc)) return cc.rgbResult;
    return initial;
}

// ANA PENCERE
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HFONT hFont = CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");

            HWND b1 = CreateWindow(L"BUTTON", L"Nefes Alma", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                20,20,160,45, hwnd, (HMENU)IDC_BTN_BREATH, NULL, NULL);
            HWND b2 = CreateWindow(L"BUTTON", L"Dalga", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                200,20,160,45, hwnd, (HMENU)IDC_BTN_WAVE, NULL, NULL);
            HWND b3 = CreateWindow(L"BUTTON", L"Rastgele Parilti", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                20,75,160,45, hwnd, (HMENU)IDC_BTN_SPARKLE, NULL, NULL);
            HWND b4 = CreateWindow(L"BUTTON", L"Tusa Basinca Tepki", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                200,75,160,45, hwnd, (HMENU)IDC_BTN_REACTIVE, NULL, NULL);
            HWND b5 = CreateWindow(L"BUTTON", L"Durdur (Isiklari Kapat)", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                20,130,340,35, hwnd, (HMENU)IDC_BTN_STOP, NULL, NULL);

            HWND lblDens = CreateWindow(L"STATIC", L"Rastgele Parilti Yogunlugu:", WS_CHILD|WS_VISIBLE,
                20,185,250,20, hwnd, (HMENU)IDC_STATIC_DENSLABEL, NULL, NULL);

            INITCOMMONCONTROLSEX icc; icc.dwSize = sizeof(icc); icc.dwICC = ICC_BAR_CLASSES;
            InitCommonControlsEx(&icc);
            g_hSlider = CreateWindow(TRACKBAR_CLASS, L"", WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,
                20,205,340,30, hwnd, (HMENU)IDC_SLIDER_DENS, NULL, NULL);
            SendMessage(g_hSlider, TBM_SETRANGE, TRUE, MAKELONG(1,10));
            SendMessage(g_hSlider, TBM_SETPOS, TRUE, g_sparkleDensity);

            CreateWindow(L"STATIC", L"Nefes Alma Rengi:", WS_CHILD|WS_VISIBLE,
                20,255,150,20, hwnd, NULL, NULL, NULL);
            WNDCLASS swc = {0};
            swc.lpfnWndProc = SwatchProc;
            swc.hInstance = GetModuleHandle(NULL);
            swc.lpszClassName = L"SwatchClass";
            swc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
            RegisterClass(&swc);

            g_hSwatchBreath = CreateWindow(L"SwatchClass", L"", WS_CHILD|WS_VISIBLE|WS_BORDER,
                20,278,50,28, hwnd, (HMENU)IDC_SWATCH_BREATH, NULL, NULL);
            set_swatch_color(g_hSwatchBreath, g_breathColor);
            CreateWindow(L"BUTTON", L"Renk Sec", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                80,278,120,28, hwnd, (HMENU)IDC_BTN_COLOR_BREATH, NULL, NULL);

            CreateWindow(L"STATIC", L"Tusa Basinca Tepki Rengi:", WS_CHILD|WS_VISIBLE,
                20,320,200,20, hwnd, NULL, NULL, NULL);
            g_hSwatchReact = CreateWindow(L"SwatchClass", L"", WS_CHILD|WS_VISIBLE|WS_BORDER,
                20,343,50,28, hwnd, (HMENU)IDC_SWATCH_REACT, NULL, NULL);
            set_swatch_color(g_hSwatchReact, g_reactiveColor);
            CreateWindow(L"BUTTON", L"Renk Sec", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                80,343,120,28, hwnd, (HMENU)IDC_BTN_COLOR_REACT, NULL, NULL);
            CreateWindow(L"BUTTON", L"Rastgele", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                210,343,90,28, hwnd, (HMENU)IDC_BTN_REACT_RANDOM, NULL, NULL);

            g_hStatus = CreateWindow(L"STATIC", L"Aktif efekt: Yok", WS_CHILD|WS_VISIBLE|SS_LEFT,
                20,395,340,25, hwnd, (HMENU)IDC_STATIC_STATUS, NULL, NULL);

            HWND children[] = {b1,b2,b3,b4,b5,lblDens,g_hStatus};
            for (size_t i=0;i<sizeof(children)/sizeof(children[0]);i++)
                SendMessage(children[i], WM_SETFONT, (WPARAM)hFont, TRUE);

            return 0;
        }

        case WM_HSCROLL:
            if ((HWND)lp == g_hSlider) {
                g_sparkleDensity = (LONG)SendMessage(g_hSlider, TBM_GETPOS, 0, 0);
            }
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case IDC_BTN_BREATH:   start_effect(IDC_BTN_BREATH, effect_breathing, L"Aktif efekt: Nefes Alma"); break;
                case IDC_BTN_WAVE:     start_effect(IDC_BTN_WAVE, effect_wave, L"Aktif efekt: Dalga"); break;
                case IDC_BTN_SPARKLE:  start_effect(IDC_BTN_SPARKLE, effect_sparkle, L"Aktif efekt: Rastgele Parilti"); break;
                case IDC_BTN_REACTIVE: start_effect(IDC_BTN_REACTIVE, effect_reactive, L"Aktif efekt: Tusa Basinca Tepki"); break;
                case IDC_BTN_STOP:     stop_current_effect(); set_status(L"Aktif efekt: Yok"); break;
                case IDC_BTN_COLOR_BREATH:
                    g_breathColor = pick_color(hwnd, g_breathColor);
                    set_swatch_color(g_hSwatchBreath, g_breathColor);
                    break;
                case IDC_BTN_COLOR_REACT:
                    g_reactiveColor = pick_color(hwnd, g_reactiveColor);
                    set_swatch_color(g_hSwatchReact, g_reactiveColor);
                    g_reactiveRandomMode = 0;
                    break;
                case IDC_BTN_REACT_RANDOM:
                    g_reactiveRandomMode = 1;
                    set_swatch_color(g_hSwatchReact, RGB(200,200,200));
                    break;
            }
            return 0;

        case WM_DESTROY:
            stop_current_effect();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    (void)hPrev; (void)cmd; (void)show;
    srand((unsigned int)time(NULL));

    hid_init();
    g_h = hid_open_path(DEVICE_PATH);
    if (!g_h) {
        MessageBox(NULL, L"Cihaz acilamadi! GCUBridge servisinin kapali oldugundan emin ol.", L"Hata", MB_OK|MB_ICONERROR);
        return 1;
    }
    if (enter_custom_mode() < 0) {
        MessageBox(NULL, L"Per-key moduna girilemedi!", L"Hata", MB_OK|MB_ICONERROR);
        hid_close(g_h); hid_exit();
        return 1;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"RGBMainWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"RegisterClass basarisiz!", L"Hata", MB_OK|MB_ICONERROR);
        hid_close(g_h); hid_exit();
        return 1;
    }

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Game Garaj RGB Engine",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 470,
        NULL, NULL, hInst, NULL);
    if (!hwnd) {
        MessageBox(NULL, L"CreateWindow basarisiz!", L"Hata", MB_OK|MB_ICONERROR);
        hid_close(g_h); hid_exit();
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hid_close(g_h);
    hid_exit();
    return 0;
}
