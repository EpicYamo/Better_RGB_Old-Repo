// Game Garaj (Tongfang/ITE8291) Better_RGB - (GUI) VERSIYONU
// Version 2

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "hidapi.h"
#include "resource.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEVICE_PATH "\\\\?\\HID#VID_048D&PID_600B&MI_01#9&29ab79e5&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
#define FRAME_DELAY_MS 10
#define BUF_SIZE 512

// Mode indices (match combo box order)
#define MODE_BREATH   0
#define MODE_WAVE     1
#define MODE_SPARKLE  2
#define MODE_REACTIVE 3
#define MODE_WHEEL    4
#define MODE_LIGHTNING 5
#define MODE_FLAME     6
#define MODE_RAIN      7
#define MODE_MATRIX    8
#define MODE_COUNT    9

// Control IDs
#define IDC_COMBO_MODE       200
#define IDC_BTN_STOP         201
#define IDC_SLIDER_BRIGHT    202
#define IDC_STATIC_STATUS    203

#define IDC_BTN_COLOR_BREATH 210
#define IDC_SWATCH_BREATH    211
#define IDC_SLIDER_BSPEED    212
#define IDC_SLIDER_BSMOOTH   213

#define IDC_SLIDER_ANGLE     220
#define IDC_SLIDER_WSPEED    221

#define IDC_SLIDER_SPSPEED   230
#define IDC_SLIDER_SPDENS    231

#define IDC_BTN_COLOR_REACT  240
#define IDC_SWATCH_REACT     241
#define IDC_BTN_REACT_RANDOM 242
#define IDC_RADIO_SHORT      243
#define IDC_RADIO_MEDIUM     244
#define IDC_RADIO_LONG       245

#define IDC_CHK_WHEEL_REVERSE 250
#define IDC_SLIDER_WHSPEED    251

#define IDC_SLIDER_LTSPEED    261
#define IDC_SLIDER_LTSMOOTH   262
#define IDC_SLIDER_LTWIDTH    263
#define IDC_SLIDER_LTCONCUR   264
#define IDC_SLIDER_FLSPEED    265
#define IDC_SLIDER_FLSMOOTH   266
#define IDC_SLIDER_RNSPEED    267
#define IDC_RAIN_DENSITY_BASE 400   // 400..404 for target values 1-5
#define IDC_RAIN_COLORCNT_BASE 410  // 410..414 for values 1-5
#define IDC_RAIN_COLOR_BASE   270 // 270..274 for up to 5 color buttons
#define IDC_SLIDER_MXSPEED    280
#define IDC_MATRIX_DENSITY_BASE 420  // 420..424 for target values 1-5
#define IDC_MATRIX_COLORCNT_BASE 430 // 430..432 for values 1-3
#define IDC_COMBO_MXSTYLE     282
#define IDC_MATRIX_COLOR_BASE 285 // 285..287 for up to 3 color buttons

// ---------------------------------------------------------------
// KEYMAP
// ---------------------------------------------------------------
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

// ---------------------------------------------------------------
// GLOBAL STATE
// ---------------------------------------------------------------
static hid_device *g_h = NULL;
static HANDLE g_effectThread = NULL;
static volatile LONG g_stopFlag = 0;
static volatile LONG g_activeMode = -1; // -1 = none

static volatile LONG g_brightness = 100;         // 0-100, always active

static COLORREF g_breathColor = RGB(0, 120, 255);
static volatile LONG g_breathSpeed = 12;         // 1-40 -> actual t-increment = value/100.0
static volatile LONG g_breathSmooth = 60;        // 2-150 quantization levels (higher = smoother)

static volatile LONG g_waveAngle = 0;            // 0-359 degrees
static volatile LONG g_waveSpeed = 4;            // 1-20 -> t increment directly

static volatile LONG g_sparkleSpeed = 5;         // 1-10 -> how fast slots refill
static volatile LONG g_sparkleDensity = 8;       // 3-20 -> target number of simultaneously lit keys

static COLORREF g_reactiveColor = RGB(255, 255, 255);
static volatile LONG g_reactiveRandomMode = 1;
static volatile LONG g_reactiveDuration = 20; // fade-out duration in frames: Short=10 Medium=20 Long=45

static volatile LONG g_wheelReverse = 0;
static volatile LONG g_wheelSpeed = 4;           // 1-20 -> t increment directly

static volatile LONG g_lightningSpeed = 5;       // 1-10 -> how fast the bolt travels top-to-bottom
static volatile LONG g_lightningSmooth = 10;     // 1-20 -> trailing glow length (higher = softer/longer)
static volatile LONG g_lightningWidth = 2;       // 1-4 -> strike column width in key-widths
static volatile LONG g_lightningConcurrent = 1;  // 1-4 -> how many bolts can strike at once

static volatile LONG g_flameSpeed = 5;           // 1-10 -> how fast heat drifts/changes
static volatile LONG g_flameSmooth = 8;          // 1-20 -> how gradually heat blends toward new targets

#define MAX_RAIN_DROPS 20
static volatile LONG g_rainSpeed = 5;            // 1-10 -> how fast each drop falls/completes
static volatile LONG g_rainDensityTarget = 3;    // 1-5 -> target simultaneous drops (probabilistic +-1)
static volatile LONG g_rainColorCount = 1;       // 1-5
static COLORREF g_rainColors[5] = { RGB(80,140,255), RGB(120,80,255), RGB(80,220,255), RGB(255,255,255), RGB(80,255,180) };
static HWND g_rainColorBtn[5];
static HWND g_rainDensityBtn[5];
static HWND g_rainColorCountBtn[5];

static volatile LONG g_matrixSpeed = 5;          // 1-10 -> streak speed
static volatile LONG g_matrixStyle = 0;          // 0 = diagonal streaks, 1 = vertical columns
static volatile LONG g_matrixDensityTarget = 2;  // 1-5 -> target simultaneous diagonal streaks (probabilistic +-1)
static volatile LONG g_matrixColorCount = 1;     // 1-3
static COLORREF g_matrixColors[3] = { RGB(0,255,60), RGB(0,180,255), RGB(255,0,120) };
static HWND g_matrixColorBtn[3];
static HWND g_matrixDensityBtn[5];
static HWND g_matrixColorCountBtn[3];
static HWND g_hComboMatrixStyle;

#define WM_TRAYICON (WM_USER + 1)
#define IDC_TRAY_RESTORE 900
#define IDC_TRAY_EXIT    901

static HWND g_hStatus, g_hComboMode, g_hSliderBright, g_hLblAngle, g_hLblConcurrent;
static HWND g_hSwatchBreath, g_hSwatchReact;
static HBRUSH g_bgBrush;
static NOTIFYICONDATA g_nid;

// Panel control groups (shown/hidden together based on selected mode)
#define MAX_PANEL_CTRLS 25
static HWND g_panelBreath[MAX_PANEL_CTRLS]; static int g_panelBreathCount = 0;
static HWND g_panelWave[MAX_PANEL_CTRLS];   static int g_panelWaveCount = 0;
static HWND g_panelSparkle[MAX_PANEL_CTRLS];static int g_panelSparkleCount = 0;
static HWND g_panelReactive[MAX_PANEL_CTRLS];static int g_panelReactiveCount = 0;
static HWND g_panelWheel[MAX_PANEL_CTRLS];  static int g_panelWheelCount = 0;
static HWND g_panelLightning[MAX_PANEL_CTRLS]; static int g_panelLightningCount = 0;
static HWND g_panelFlame[MAX_PANEL_CTRLS];  static int g_panelFlameCount = 0;
static HWND g_panelRain[MAX_PANEL_CTRLS];   static int g_panelRainCount = 0;
static HWND g_panelMatrix[MAX_PANEL_CTRLS]; static int g_panelMatrixCount = 0;

#define CLR_BG        RGB(24,24,28)
#define CLR_PANEL     RGB(45,45,52)
#define CLR_PANEL_HOV RGB(55,55,64)
#define CLR_ACCENT    RGB(127,90,240)
#define CLR_ACCENT_LT RGB(170,140,255)
#define CLR_TEXT      RGB(235,235,240)
#define CLR_BORDER    RGB(70,70,80)

// ---------------------------------------------------------------
// PROTOCOL LAYER
// ---------------------------------------------------------------
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
    int bright = (int)g_brightness;
    if (bright < 100) {
        unsigned char scaled[BUF_SIZE];
        for (int i = 0; i < BUF_SIZE; i++) scaled[i] = (unsigned char)((int)buf[i] * bright / 100);
        buf = scaled;
    }
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

int compute_prob_count(int target, int minCount, int maxCount) {
    if (target >= maxCount) return maxCount; // at max, always exactly max
    int r = rand() % 100;
    if (r < 25) { int v = target-1; return (v < minCount) ? minCount : v; }
    else if (r < 75) return target;
    else { int v = target+1; return (v > maxCount) ? maxCount : v; }
}

double get_key_x(const KeyEntry *k) {
    // MANUAL OVERRIDES: these keys sit after wide keys (Space etc.) in their row,
    // so their raw column index doesn't reflect their true physical x position.
    // Tune these numbers by feel: higher = further right. Row is unaffected by this table.
    if (strcmp(k->name,"AltGr")==0)         return 11.5;
    if (strcmp(k->name,"CopilotKey")==0)    return 13.0;
    if (strcmp(k->name,"ArrowUp")==0)       return 15.5;
    if (strcmp(k->name,"ArrowLeft")==0)     return 14.5;
    if (strcmp(k->name,"ArrowDown")==0)     return 15.5;
    if (strcmp(k->name,"ArrowRight")==0)    return 16.5;
    if (strcmp(k->name,"Numpad0")==0)       return 18.0;
    if (strcmp(k->name,"NumpadDecimal")==0) return 19.5;

    const double GAP = 1.5;
    if (strncmp(k->name, "Numpad", 6) == 0 || strcmp(k->name, "NumLock") == 0) return k->col + GAP;
    if (strcmp(k->name,"FnLockCam")==0 || strcmp(k->name,"PrtSc")==0 || strcmp(k->name,"Del")==0 ||
        strcmp(k->name,"Home")==0 || strcmp(k->name,"PgUp")==0 || strcmp(k->name,"PgDn")==0 || strcmp(k->name,"End")==0)
        return k->col + GAP;
    return (double)k->col;
}

// ---------------------------------------------------------------
// EFFECTS
// ---------------------------------------------------------------
DWORD WINAPI effect_breathing(LPVOID p) {
    (void)p;
    double t = 0.0;
    while (!g_stopFlag) {
        unsigned char br = GetRValue(g_breathColor), bg = GetGValue(g_breathColor), bb = GetBValue(g_breathColor);
        double raw = (sin(t) + 1.0) / 2.0;
        int levels = (int)g_breathSmooth;
        if (levels < 2) levels = 2;
        double bright = floor(raw * levels) / (double)levels;
        unsigned char r=(unsigned char)(br*bright), g=(unsigned char)(bg*bright), b=(unsigned char)(bb*bright);
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) set_key(buf, KEYMAP[i].offset, r,g,b);
        send_frame(buf);
        t += (double)g_breathSpeed / 100.0;
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_wave(LPVOID p) {
    (void)p;
    double t = 0.0;
    while (!g_stopFlag) {
        double angleRad = (double)g_waveAngle * M_PI / 180.0;
        double dirX = cos(angleRad), dirY = sin(angleRad);
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            double x = get_key_x(&KEYMAP[i]);
            double y = (double)KEYMAP[i].row;
            double proj = x*dirX + y*dirY;
            double hue = t + proj * 15.0;
            unsigned char r,g,b;
            hsv_to_rgb(hue,&r,&g,&b);
            set_key(buf, KEYMAP[i].offset, r,g,b);
        }
        send_frame(buf);
        t += (double)g_waveSpeed;
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_wheel(LPVOID p) {
    (void)p;
    double cx = 0, cy = 0;
    for (size_t i=0;i<KEYMAP_COUNT;i++) { cx += get_key_x(&KEYMAP[i]); cy += KEYMAP[i].row; }
    cx /= KEYMAP_COUNT; cy /= KEYMAP_COUNT;

    double t = 0.0;
    while (!g_stopFlag) {
        double dirMul = g_wheelReverse ? -1.0 : 1.0;
        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            double x = get_key_x(&KEYMAP[i]) - cx;
            double y = (double)KEYMAP[i].row - cy;
            double angleDeg = atan2(y, x) * 180.0 / M_PI;
            double hue = t*dirMul + angleDeg;
            unsigned char r,g,b;
            hsv_to_rgb(hue,&r,&g,&b);
            set_key(buf, KEYMAP[i].offset, r,g,b);
        }
        send_frame(buf);
        t += (double)g_wheelSpeed;
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

#define MAX_STRIKES 4
DWORD WINAPI effect_lightning(LPVOID p) {
    (void)p;
    double headPos[MAX_STRIKES], boltX[MAX_STRIKES];
    int countdown[MAX_STRIKES];
    for (int i=0;i<MAX_STRIKES;i++) { headPos[i] = 100.0; boltX[i] = 10.0; countdown[i] = 10 + rand()%30; }

    while (!g_stopFlag) {
        double speed = 0.05 + (double)g_lightningSpeed * 0.08;
        double bandWidth = 0.4 + (double)g_lightningSmooth * 0.15;  // trailing tail length behind the head
        double tailLen = bandWidth * 2.5;
        double finishPos = 6.0 + tailLen + 0.5; // let the tail fully clear the bottom before resetting
        double xWidth = 0.35 + ((double)g_lightningWidth - 1.0) * 0.383; // level1~0.35 .. level4~1.5
        int concurrent = (int)g_lightningConcurrent;
        if (concurrent < 1) concurrent = 1;
        if (concurrent > MAX_STRIKES) concurrent = MAX_STRIKES;

        double bright[KEYMAP_COUNT];
        for (size_t i=0;i<KEYMAP_COUNT;i++) bright[i] = 0;

        for (int s=0; s<concurrent; s++) {
            if (headPos[s] > finishPos) {
                countdown[s]--;
                if (countdown[s] <= 0) {
                    headPos[s] = -2.0;
                    boltX[s] = (double)(rand() % 20);
                    countdown[s] = 20 + rand()%40;
                }
            }
            if (headPos[s] <= finishPos) {
                for (size_t i=0;i<KEYMAP_COUNT;i++) {
                    double dv = headPos[s] - KEYMAP[i].row; // positive = trailing behind (above), negative = not yet reached
                    double dh = fabs(get_key_x(&KEYMAP[i]) - boltX[s]);
                    if (dv < -0.3 || dv > tailLen) continue;
                    if (dh > xWidth) continue;
                    double vFall = (dv < 0) ? 1.0 : (1.0 - dv/tailLen);
                    if (vFall < 0) vFall = 0;
                    double hFall = 1.0 - dh/xWidth;
                    if (hFall < 0) hFall = 0;
                    double b = vFall * hFall;
                    if (b > bright[i]) bright[i] = b;
                }
                headPos[s] += speed;
            }
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            if (bright[i] > 0) {
                unsigned char v = (unsigned char)(bright[i]*255);
                set_key(buf, KEYMAP[i].offset, v, v, 255);
            } else {
                set_key(buf, KEYMAP[i].offset, 4,4,14); // dim stormy base
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_flame(LPVOID p) {
    (void)p;
    double heat[KEYMAP_COUNT];
    for (size_t i=0;i<KEYMAP_COUNT;i++) heat[i] = 100.0 + rand()%60;
    int frameCounter = 0;

    while (!g_stopFlag) {
        double speed = (double)g_flameSpeed;      // 1-10
        double smooth = (double)g_flameSmooth;    // 1-20
        double alpha = 1.0 / (1.0 + smooth*0.6);  // higher smoothness -> slower blending
        double volatility = 40.0 + speed*18.0;    // how wide the random target range is
        int updateInterval = 11 - (int)speed;     // speed 1 -> update every 10 frames (slow), speed 10 -> every frame
        if (updateInterval < 1) updateInterval = 1;

        frameCounter++;
        if (frameCounter % updateInterval == 0) {
            for (size_t i=0;i<KEYMAP_COUNT;i++) {
                double rowFrac = KEYMAP[i].row / 6.0; // weak bias only, bottom slightly hotter on average
                double bias = 40.0 * rowFrac;
                double target = 90.0 + bias + (rand()%(int)volatility - volatility/2.0);
                heat[i] = heat[i]*(1.0-alpha) + target*alpha;
                if (heat[i] < 0) heat[i] = 0;
                if (heat[i] > 255) heat[i] = 255;
            }
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            int h = (int)heat[i];
            unsigned char r,g,b;
            if (h < 85) { r=(unsigned char)(h*3); g=0; b=0; }
            else if (h < 170) { r=255; g=(unsigned char)((h-85)*3); b=0; }
            else { r=255; g=255; b=(unsigned char)((h-170)*3); }
            set_key(buf, KEYMAP[i].offset, r,g,b);
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

#define RAIN_DROPS MAX_RAIN_DROPS
DWORD WINAPI effect_rain(LPVOID p) {
    (void)p;
    double dropX[RAIN_DROPS], dropRow[RAIN_DROPS];
    int dropColorIdx[RAIN_DROPS];
    for (int i=0;i<RAIN_DROPS;i++) {
        dropX[i] = rand()%20;
        dropRow[i] = -(double)(rand()%8);
        dropColorIdx[i] = rand() % (int)g_rainColorCount;
    }
    int activeDrops = (int)g_rainDensityTarget * 3;

    while (!g_stopFlag) {
        double speed = 0.05 + (double)g_rainSpeed*0.03;
        int colorCount = (int)g_rainColorCount;
        if (colorCount < 1) colorCount = 1;
        if (colorCount > 5) colorCount = 5;

        int target = (int)g_rainDensityTarget;
        if (target < 1) target = 1;
        if (target > 5) target = 5;
        activeDrops = target * 3; // level1=3 drops ... level5=15 drops
        if (activeDrops > MAX_RAIN_DROPS) activeDrops = MAX_RAIN_DROPS;

        unsigned char buf[BUF_SIZE] = {0};
        for (int d=0; d<activeDrops; d++) {
            COLORREF col = g_rainColors[dropColorIdx[d] % colorCount];
            unsigned char cr=GetRValue(col), cg=GetGValue(col), cb=GetBValue(col);
            for (int rr2 = 0; rr2 <= 6; rr2++) {
                double trailDist = dropRow[d] - rr2; // 0 at head, positive = trailing behind (above)
                if (trailDist < -0.3 || trailDist > 1.8) continue; // only light head + short trailing tail
                double bright = (trailDist < 0) ? 1.0 : (1.0 - trailDist/1.8);
                if (bright < 0) bright = 0;
                int best = -1; double bestDist = 1e9;
                for (size_t i=0;i<KEYMAP_COUNT;i++) {
                    if (KEYMAP[i].row != rr2) continue;
                    double dx = fabs(get_key_x(&KEYMAP[i]) - dropX[d]);
                    if (dx < bestDist) { bestDist = dx; best = (int)i; }
                }
                if (best >= 0 && bestDist < 1.2) {
                    set_key(buf, KEYMAP[best].offset, (unsigned char)(cr*bright), (unsigned char)(cg*bright), (unsigned char)(cb*bright));
                }
            }
            dropRow[d] += speed;
            if (dropRow[d] > 8) {
                dropRow[d] = -(double)(rand()%5);
                dropX[d] = rand()%20;
                dropColorIdx[d] = rand() % colorCount;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

#define MATRIX_STREAKS 5
#define MATRIX_COLUMNS 24
DWORD WINAPI effect_matrix(LPVOID p) {
    (void)p;
    // Style 0: diagonal streaks
    double angle[MATRIX_STREAKS], pos[MATRIX_STREAKS], spd[MATRIX_STREAKS];
    int streakColorIdx[MATRIX_STREAKS];
    for (int i=0;i<MATRIX_STREAKS;i++) {
        angle[i] = (rand()%360) * M_PI / 180.0;
        pos[i] = -30.0;
        spd[i] = 0.2 + (rand()%100)/100.0;
        streakColorIdx[i] = rand() % (int)g_matrixColorCount;
    }
    int activeStreaks = (int)g_matrixDensityTarget;
    int rerollCounter = 0;
    // Style 1: vertical falling columns (classic "digital rain")
    double colPos[MATRIX_COLUMNS]; double colSpeed[MATRIX_COLUMNS]; int colColorIdx[MATRIX_COLUMNS];
    for (int i=0;i<MATRIX_COLUMNS;i++) {
        colPos[i] = -(double)(rand()%8);
        colSpeed[i] = 0.05 + (rand()%100)/300.0;
        colColorIdx[i] = rand() % (int)g_matrixColorCount;
    }
    // Style 2: laser sweep (2-3 sharp thin lines)
    #define MAX_LASERS 3
    double laserAngle[MAX_LASERS], laserPos[MAX_LASERS], laserSpd[MAX_LASERS];
    int laserColorIdx[MAX_LASERS];
    for (int i=0;i<MAX_LASERS;i++) {
        laserAngle[i] = (rand()%4) * (M_PI/2.0); // 0/90/180/270 for crisp horizontal/vertical sweeps
        laserPos[i] = -30.0 - i*8.0; // staggered start
        laserSpd[i] = 0.6 + (rand()%100)/100.0;
        laserColorIdx[i] = rand() % (int)g_matrixColorCount;
    }
    // Style 3: ripple (expanding rings from random points)
    #define MAX_RIPPLES 3
    double rippleX[MAX_RIPPLES], rippleY[MAX_RIPPLES], rippleR[MAX_RIPPLES];
    int rippleColorIdx[MAX_RIPPLES];
    for (int i=0;i<MAX_RIPPLES;i++) {
        rippleX[i] = rand()%20; rippleY[i] = rand()%7; rippleR[i] = i*8.0;
        rippleColorIdx[i] = rand() % (int)g_matrixColorCount;
    }

    while (!g_stopFlag) {
        int colorCount = (int)g_matrixColorCount; if (colorCount<1) colorCount=1; if (colorCount>3) colorCount=3;
        int style = (int)g_matrixStyle;
        double speedMul = 0.3 + (double)g_matrixSpeed*0.15;
        unsigned char buf[BUF_SIZE] = {0};

        if (style == 0) {
            rerollCounter++;
            if (rerollCounter >= 60) {
                activeStreaks = compute_prob_count((int)g_matrixDensityTarget, 1, 5);
                rerollCounter = 0;
            }
            for (size_t i=0;i<KEYMAP_COUNT;i++) {
                double x = get_key_x(&KEYMAP[i]), y = KEYMAP[i].row;
                double bestBright = 0; int bestColor = 0;
                for (int s=0;s<activeStreaks;s++) {
                    double dirX = cos(angle[s]), dirY = sin(angle[s]);
                    double proj = x*dirX + y*dirY;
                    double dist = fabs(proj - pos[s]);
                    if (dist < 1.5) {
                        double b = 1.0 - dist/1.5;
                        if (b > bestBright) { bestBright = b; bestColor = streakColorIdx[s] % colorCount; }
                    }
                }
                if (bestBright > 1) bestBright = 1;
                COLORREF col = g_matrixColors[bestColor];
                unsigned char cr=(unsigned char)(GetRValue(col)*bestBright), cg=(unsigned char)(GetGValue(col)*bestBright), cb=(unsigned char)(GetBValue(col)*bestBright);
                set_key(buf, KEYMAP[i].offset, cr, cg, cb);
            }
            for (int s=0;s<activeStreaks;s++) {
                pos[s] += spd[s]*speedMul;
                if (pos[s] > 30) { pos[s]=-30; angle[s]=(rand()%360)*M_PI/180.0; spd[s]=0.2+(rand()%100)/100.0; streakColorIdx[s]=rand()%colorCount; }
            }
        } else if (style == 1) {
            // vertical columns: each key belongs to nearest integer column bucket based on x
            for (size_t i=0;i<KEYMAP_COUNT;i++) {
                double x = get_key_x(&KEYMAP[i]);
                int col = (int)(x + 0.5);
                if (col < 0) col = 0; if (col >= MATRIX_COLUMNS) col = MATRIX_COLUMNS-1;
                double dv = colPos[col] - KEYMAP[i].row;
                double bright = 0;
                if (dv >= -0.3 && dv < 4.0) bright = (dv < 0) ? 1.0 : (1.0 - dv/4.0);
                if (bright < 0) bright = 0;
                COLORREF cc = g_matrixColors[colColorIdx[col] % colorCount];
                set_key(buf, KEYMAP[i].offset, (unsigned char)(GetRValue(cc)*bright), (unsigned char)(GetGValue(cc)*bright), (unsigned char)(GetBValue(cc)*bright));
            }
            for (int i=0;i<MATRIX_COLUMNS;i++) {
                colPos[i] += colSpeed[i]*speedMul;
                if (colPos[i] > 10) { colPos[i] = -(double)(rand()%6); colSpeed[i]=0.05+(rand()%100)/300.0; colColorIdx[i]=rand()%colorCount; }
            }
        } else if (style == 2) {
            // laser sweep: 2-3 crisp thin lines, count tied to density selector
            int target = (int)g_matrixDensityTarget;
            int laserCount = (target >= 4) ? 3 : 2;
            for (size_t i=0;i<KEYMAP_COUNT;i++) {
                double x = get_key_x(&KEYMAP[i]), y = KEYMAP[i].row;
                double bestBright = 0; int bestColor = 0;
                for (int s=0;s<laserCount;s++) {
                    double dirX = cos(laserAngle[s]), dirY = sin(laserAngle[s]);
                    double proj = x*dirX + y*dirY;
                    double dist = fabs(proj - laserPos[s]);
                    if (dist < 0.6) {
                        double b = 1.0 - dist/0.6;
                        if (b > bestBright) { bestBright = b; bestColor = laserColorIdx[s] % colorCount; }
                    }
                }
                if (bestBright > 1) bestBright = 1;
                COLORREF col = g_matrixColors[bestColor];
                unsigned char cr=(unsigned char)(GetRValue(col)*bestBright), cg=(unsigned char)(GetGValue(col)*bestBright), cb=(unsigned char)(GetBValue(col)*bestBright);
                set_key(buf, KEYMAP[i].offset, cr, cg, cb);
            }
            for (int s=0;s<laserCount;s++) {
                laserPos[s] += laserSpd[s]*speedMul*1.5;
                if (laserPos[s] > 30) {
                    laserPos[s] = -30.0 - rand()%20;
                    laserAngle[s] = (rand()%4) * (M_PI/2.0);
                    laserSpd[s] = 0.6 + (rand()%100)/100.0;
                    laserColorIdx[s] = rand()%colorCount;
                }
            }
        } else {
            // ripple: expanding rings from random points
            for (size_t i=0;i<KEYMAP_COUNT;i++) {
                double x = get_key_x(&KEYMAP[i]), y = KEYMAP[i].row;
                double bestBright = 0; int bestColor = 0;
                for (int s=0;s<MAX_RIPPLES;s++) {
                    double dx = x - rippleX[s], dy = y - rippleY[s];
                    double dist = sqrt(dx*dx + dy*dy);
                    double ringDist = fabs(dist - rippleR[s]);
                    if (ringDist < 1.2) {
                        double b = 1.0 - ringDist/1.2;
                        if (b > bestBright) { bestBright = b; bestColor = rippleColorIdx[s] % colorCount; }
                    }
                }
                if (bestBright > 1) bestBright = 1;
                COLORREF col = g_matrixColors[bestColor];
                unsigned char cr=(unsigned char)(GetRValue(col)*bestBright), cg=(unsigned char)(GetGValue(col)*bestBright), cb=(unsigned char)(GetBValue(col)*bestBright);
                set_key(buf, KEYMAP[i].offset, cr, cg, cb);
            }
            for (int s=0;s<MAX_RIPPLES;s++) {
                rippleR[s] += speedMul * 0.25;
                if (rippleR[s] > 24) {
                    rippleX[s] = rand()%20; rippleY[s] = rand()%7; rippleR[s] = 0;
                    rippleColorIdx[s] = rand()%colorCount;
                }
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    return 0;
}

DWORD WINAPI effect_sparkle(LPVOID p) {
    (void)p;
    int *life = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    int *maxLife = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr=(unsigned char*)calloc(KEYMAP_COUNT,1), *gg=(unsigned char*)calloc(KEYMAP_COUNT,1), *bb=(unsigned char*)calloc(KEYMAP_COUNT,1);
    while (!g_stopFlag) {
        int target = (int)g_sparkleDensity;         // exact number of keys kept lit at once (3-20)
        int speed = (int)g_sparkleSpeed;             // controls how long each spark stays lit (turnover rate)
        int lifeDuration = 50 - speed*4;
        if (lifeDuration < 8) lifeDuration = 8;

        int aliveCount = 0;
        for (size_t i=0;i<KEYMAP_COUNT;i++) if (life[i] > 0) aliveCount++;
        int deficit = target - aliveCount;

        for (int n = 0; n < deficit; n++) {
            int idx = -1;
            for (int attempt = 0; attempt < 20; attempt++) {
                int c = rand() % KEYMAP_COUNT;
                if (life[c] == 0) { idx = c; break; }
            }
            if (idx < 0) break; // keyboard full, no off key left
            life[idx] = lifeDuration;
            maxLife[idx] = lifeDuration;
            rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256;
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            if (life[i] > 0) {
                double fade = life[i] / (double)maxLife[i];
                set_key(buf, KEYMAP[i].offset, (unsigned char)(rr[i]*fade),(unsigned char)(gg[i]*fade),(unsigned char)(bb[i]*fade));
                life[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(life); free(maxLife); free(rr); free(gg); free(bb);
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
        {VK_ESCAPE,"Esc"},{VK_F1,"F1"},{VK_F2,"F2"},{VK_F3,"F3"},{VK_F4,"F4"},{VK_F5,"F5"},
        {VK_F6,"F6"},{VK_F7,"F7"},{VK_F8,"F8"},{VK_F9,"F9"},{VK_F10,"F10"},{VK_F11,"F11"},{VK_F12,"F12"},
        {VK_SNAPSHOT,"PrtSc"},{VK_DELETE,"Del"},{VK_HOME,"Home"},{VK_PRIOR,"PgUp"},{VK_NEXT,"PgDn"},{VK_END,"End"},
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
    int *wasDown = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    int *fadeLife = (int*)calloc(KEYMAP_COUNT, sizeof(int));
    unsigned char *rr=(unsigned char*)calloc(KEYMAP_COUNT,1), *gg=(unsigned char*)calloc(KEYMAP_COUNT,1), *bb=(unsigned char*)calloc(KEYMAP_COUNT,1);

    int vkList[128]; int vkCount = 0;
    for (int vk='A'; vk<='Z'; vk++) vkList[vkCount++] = vk;
    for (int vk='0'; vk<='9'; vk++) vkList[vkCount++] = vk;
    int specials[] = {VK_SPACE,VK_RETURN,VK_BACK,VK_TAB,VK_LSHIFT,VK_RSHIFT,VK_CAPITAL,VK_LCONTROL,VK_LMENU,VK_RMENU,VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_OEM_102,VK_LWIN,
                       VK_OEM_4,VK_OEM_6,VK_OEM_1,VK_OEM_7,VK_OEM_5,VK_OEM_COMMA,VK_OEM_PERIOD,VK_OEM_2,VK_OEM_MINUS,VK_OEM_PLUS,VK_OEM_3,
                       VK_ESCAPE,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,VK_F11,VK_F12,
                       VK_SNAPSHOT,VK_DELETE,VK_HOME,VK_PRIOR,VK_NEXT,VK_END,
                       VK_NUMPAD0,VK_NUMPAD1,VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD6,VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD9,
                       VK_DECIMAL,VK_ADD,VK_SUBTRACT,VK_MULTIPLY,VK_DIVIDE};
    for (size_t s=0;s<sizeof(specials)/sizeof(specials[0]);s++) vkList[vkCount++] = specials[s];

    while (!g_stopFlag) {
        unsigned char fr=GetRValue(g_reactiveColor), fg=GetGValue(g_reactiveColor), fb=GetBValue(g_reactiveColor);
        for (int v = 0; v < vkCount; v++) {
            int idx = vk_to_keymap_index(vkList[v]);
            if (idx < 0) continue;
            int down = (GetAsyncKeyState(vkList[v]) & 0x8000) != 0;
            if (down) {
                if (!wasDown[idx]) {
                    if (g_reactiveRandomMode) { rr[idx]=rand()%256; gg[idx]=rand()%256; bb[idx]=rand()%256; }
                    else { rr[idx]=fr; gg[idx]=fg; bb[idx]=fb; }
                }
                wasDown[idx] = 1;
                fadeLife[idx] = (int)g_reactiveDuration;
            } else {
                wasDown[idx] = 0;
            }
        }

        unsigned char buf[BUF_SIZE] = {0};
        for (size_t i=0;i<KEYMAP_COUNT;i++) {
            if (wasDown[i]) {
                set_key(buf, KEYMAP[i].offset, rr[i], gg[i], bb[i]);
            } else if (fadeLife[i] > 0) {
                double fade = fadeLife[i] / (double)g_reactiveDuration;
                set_key(buf, KEYMAP[i].offset, (unsigned char)(rr[i]*fade),(unsigned char)(gg[i]*fade),(unsigned char)(bb[i]*fade));
                fadeLife[i]--;
            }
        }
        send_frame(buf);
        Sleep(FRAME_DELAY_MS);
    }
    free(wasDown); free(fadeLife); free(rr); free(gg); free(bb);
    return 0;
}

// ---------------------------------------------------------------
// THREAD MANAGEMENT
// ---------------------------------------------------------------
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
    g_activeMode = -1;
}

LPTHREAD_START_ROUTINE mode_to_fn(int mode) {
    switch (mode) {
        case MODE_BREATH: return effect_breathing;
        case MODE_WAVE: return effect_wave;
        case MODE_SPARKLE: return effect_sparkle;
        case MODE_REACTIVE: return effect_reactive;
        case MODE_WHEEL: return effect_wheel;
        case MODE_LIGHTNING: return effect_lightning;
        case MODE_FLAME: return effect_flame;
        case MODE_RAIN: return effect_rain;
        case MODE_MATRIX: return effect_matrix;
    }
    return NULL;
}

const wchar_t* mode_to_label(int mode) {
    switch (mode) {
        case MODE_BREATH: return L"Active Mode: Breathing";
        case MODE_WAVE: return L"Active Mode: Wave";
        case MODE_SPARKLE: return L"Active Mode: Sparkle";
        case MODE_REACTIVE: return L"Active Mode: Reactive";
        case MODE_WHEEL: return L"Active Mode: Wheel";
        case MODE_LIGHTNING: return L"Active Mode: Lightning";
        case MODE_FLAME: return L"Active Mode: Flame";
        case MODE_RAIN: return L"Active Mode: Rain";
        case MODE_MATRIX: return L"Active Mode: Matrix";
    }
    return L"Active Mode: None";
}

void start_mode(int mode) {
    stop_current_effect();
    InterlockedExchange(&g_stopFlag, 0);
    g_activeMode = mode;
    g_effectThread = CreateThread(NULL, 0, mode_to_fn(mode), NULL, 0, NULL);
    set_status(mode_to_label(mode));
}

// ---------------------------------------------------------------
// PANEL VISIBILITY
// ---------------------------------------------------------------
void show_only(HWND *arr, int count, BOOL show) {
    for (int i = 0; i < count; i++) ShowWindow(arr[i], show ? SW_SHOW : SW_HIDE);
}

void show_panel_for_mode(int mode) {
    show_only(g_panelBreath, g_panelBreathCount, mode == MODE_BREATH);
    show_only(g_panelWave, g_panelWaveCount, mode == MODE_WAVE);
    show_only(g_panelSparkle, g_panelSparkleCount, mode == MODE_SPARKLE);
    show_only(g_panelReactive, g_panelReactiveCount, mode == MODE_REACTIVE);
    show_only(g_panelWheel, g_panelWheelCount, mode == MODE_WHEEL);
    show_only(g_panelLightning, g_panelLightningCount, mode == MODE_LIGHTNING);
    show_only(g_panelFlame, g_panelFlameCount, mode == MODE_FLAME);
    show_only(g_panelRain, g_panelRainCount, mode == MODE_RAIN);
    show_only(g_panelMatrix, g_panelMatrixCount, mode == MODE_MATRIX);
    HWND top = GetAncestor(g_panelBreath[0], GA_ROOT);
    if (top) { InvalidateRect(top, NULL, TRUE); UpdateWindow(top); }
}

// ---------------------------------------------------------------
// COLOR SWATCH
// ---------------------------------------------------------------
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

// ---------------------------------------------------------------
// OWNER-DRAW BUTTON PAINTING
// ---------------------------------------------------------------
void draw_button_ex(LPDRAWITEMSTRUCT dis, BOOL useCustomColor, COLORREF customColor) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    wchar_t text[64];
    GetWindowText(dis->hwndItem, text, 64);

    BOOL pressed = (dis->itemState & ODS_SELECTED) != 0;
    COLORREF bg = useCustomColor ? customColor : (pressed ? CLR_PANEL_HOV : CLR_PANEL);

    HBRUSH br = CreateSolidBrush(bg);
    HPEN pen = CreatePen(PS_SOLID, 1, CLR_BORDER);
    HGDIOBJ oldBr = SelectObject(hdc, br);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 10, 10);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPen);
    DeleteObject(br);
    DeleteObject(pen);

    if (!useCustomColor) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, CLR_TEXT);
        DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void draw_selector_button(LPDRAWITEMSTRUCT dis, BOOL isActive) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    wchar_t text[8];
    GetWindowText(dis->hwndItem, text, 8);

    COLORREF bg = isActive ? CLR_ACCENT : CLR_PANEL;
    COLORREF border = isActive ? CLR_ACCENT_LT : CLR_BORDER;

    HBRUSH br = CreateSolidBrush(bg);
    HPEN pen = CreatePen(PS_SOLID, isActive ? 2 : 1, border);
    HGDIOBJ oldBr = SelectObject(hdc, br);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 8, 8);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPen);
    DeleteObject(br);
    DeleteObject(pen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, CLR_TEXT);
    DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void draw_button(LPDRAWITEMSTRUCT dis) {
    draw_button_ex(dis, FALSE, 0);
}

BOOL CALLBACK SetFontOnChild(HWND hwnd, LPARAM lFont) {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)lFont, TRUE);
    return TRUE;
}

// ---------------------------------------------------------------
// HELPERS TO BUILD UI
// ---------------------------------------------------------------
HWND mk_label(HWND parent, const wchar_t *text, int x, int y, int w, int h) {
    return CreateWindow(L"STATIC", text, WS_CHILD|WS_VISIBLE, x,y,w,h, parent, NULL, NULL, NULL);
}

HWND mk_slider(HWND parent, int id, int x, int y, int w, int h, int lo, int hi, int pos) {
    HWND s = CreateWindow(TRACKBAR_CLASS, L"", WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS, x,y,w,h, parent, (HMENU)(INT_PTR)id, NULL, NULL);
    SendMessage(s, TBM_SETRANGE, TRUE, MAKELONG(lo,hi));
    SendMessage(s, TBM_SETPOS, TRUE, pos);
    return s;
}

HWND mk_button(HWND parent, const wchar_t *text, int id, int x, int y, int w, int h) {
    return CreateWindow(L"BUTTON", text, WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, x,y,w,h, parent, (HMENU)(INT_PTR)id, NULL, NULL);
}

// ---------------------------------------------------------------
// MAIN WINDOW
// ---------------------------------------------------------------
void refresh_rain_color_visibility(void) {
    for (int i=0;i<5;i++) ShowWindow(g_rainColorBtn[i], (i < (int)g_rainColorCount) ? SW_SHOW : SW_HIDE);
    HWND top = GetAncestor(g_rainColorBtn[0], GA_ROOT);
    if (top) { InvalidateRect(top, NULL, TRUE); UpdateWindow(top); }
}
void refresh_matrix_color_visibility(void) {
    for (int i=0;i<3;i++) ShowWindow(g_matrixColorBtn[i], (i < (int)g_matrixColorCount) ? SW_SHOW : SW_HIDE);
    HWND top = GetAncestor(g_matrixColorBtn[0], GA_ROOT);
    if (top) { InvalidateRect(top, NULL, TRUE); UpdateWindow(top); }
}
void invalidate_group(HWND *arr, int count) {
    for (int i=0;i<count;i++) InvalidateRect(arr[i], NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HICON hAppIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
            if (!hAppIcon) hAppIcon = LoadIcon(NULL, IDI_APPLICATION);

            memset(&g_nid, 0, sizeof(g_nid));
            g_nid.cbSize = sizeof(g_nid);
            g_nid.hWnd = hwnd;
            g_nid.uID = 1;
            g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            g_nid.uCallbackMessage = WM_TRAYICON;
            g_nid.hIcon = hAppIcon;
            wcscpy(g_nid.szTip, L"Better RGB by TheYamo");
            Shell_NotifyIcon(NIM_ADD, &g_nid);

            HFONT hFont = CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");

            mk_label(hwnd, L"Select Mode:", 20, 20, 100, 20);
            g_hComboMode = CreateWindow(L"COMBOBOX", L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,
                130, 18, 230, 200, hwnd, (HMENU)IDC_COMBO_MODE, NULL, NULL);
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Breathing");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Wave");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Sparkle");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Reactive");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Wheel");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Lightning");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Flame");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Rain");
            SendMessage(g_hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Matrix");

            mk_button(hwnd, L"Stop (Turn Off Lights)", IDC_BTN_STOP, 20, 55, 340, 35);

            // --- Breathing panel (y starts at 105) ---
            g_panelBreath[g_panelBreathCount++] = mk_label(hwnd, L"Breathing Color:", 20, 105, 150, 20);
            g_hSwatchBreath = CreateWindow(L"SwatchClass", L"", WS_CHILD|WS_BORDER, 20,128,50,28, hwnd, (HMENU)(INT_PTR)999, NULL, NULL);
            g_panelBreath[g_panelBreathCount++] = g_hSwatchBreath;
            g_panelBreath[g_panelBreathCount++] = mk_button(hwnd, L"Pick Color", IDC_BTN_COLOR_BREATH, 80, 128, 120, 28);
            g_panelBreath[g_panelBreathCount++] = mk_label(hwnd, L"Speed:", 20, 165, 150, 20);
            g_panelBreath[g_panelBreathCount++] = mk_slider(hwnd, IDC_SLIDER_BSPEED, 20, 185, 340, 30, 1, 40, g_breathSpeed);
            g_panelBreath[g_panelBreathCount++] = mk_label(hwnd, L"Smoothness:", 20, 220, 150, 20);
            g_panelBreath[g_panelBreathCount++] = mk_slider(hwnd, IDC_SLIDER_BSMOOTH, 20, 240, 340, 30, 2, 150, g_breathSmooth);

            // --- Wave panel ---
            g_hLblAngle = mk_label(hwnd, L"Angle: 0 degrees", 20, 105, 300, 20);
            g_panelWave[g_panelWaveCount++] = g_hLblAngle;
            HWND sAngle = mk_slider(hwnd, IDC_SLIDER_ANGLE, 20, 125, 340, 30, 0, 359, g_waveAngle);
            SendMessage(sAngle, TBM_SETTICFREQ, 45, 0);
            SetWindowLongPtr(sAngle, GWL_STYLE, GetWindowLongPtr(sAngle, GWL_STYLE) | TBS_TOOLTIPS);
            SendMessage(sAngle, TBM_SETTIPSIDE, TBTS_TOP, 0);
            g_panelWave[g_panelWaveCount++] = sAngle;
            g_panelWave[g_panelWaveCount++] = mk_label(hwnd, L"Speed:", 20, 165, 150, 20);
            g_panelWave[g_panelWaveCount++] = mk_slider(hwnd, IDC_SLIDER_WSPEED, 20, 185, 340, 30, 1, 20, g_waveSpeed);

            // --- Sparkle panel ---
            g_panelSparkle[g_panelSparkleCount++] = mk_label(hwnd, L"Speed:", 20, 105, 150, 20);
            g_panelSparkle[g_panelSparkleCount++] = mk_slider(hwnd, IDC_SLIDER_SPSPEED, 20, 125, 340, 30, 1, 10, g_sparkleSpeed);
            g_panelSparkle[g_panelSparkleCount++] = mk_label(hwnd, L"Density:", 20, 165, 150, 20);
            g_panelSparkle[g_panelSparkleCount++] = mk_slider(hwnd, IDC_SLIDER_SPDENS, 20, 185, 340, 30, 3, 20, g_sparkleDensity);

            // --- Reactive panel ---
            g_panelReactive[g_panelReactiveCount++] = mk_label(hwnd, L"Reactive Color:", 20, 105, 200, 20);
            g_hSwatchReact = CreateWindow(L"SwatchClass", L"", WS_CHILD|WS_BORDER, 20,128,50,28, hwnd, (HMENU)(INT_PTR)998, NULL, NULL);
            g_panelReactive[g_panelReactiveCount++] = g_hSwatchReact;
            g_panelReactive[g_panelReactiveCount++] = mk_button(hwnd, L"Pick Color", IDC_BTN_COLOR_REACT, 80, 128, 120, 28);
            g_panelReactive[g_panelReactiveCount++] = mk_button(hwnd, L"Random", IDC_BTN_REACT_RANDOM, 210, 128, 90, 28);
            g_panelReactive[g_panelReactiveCount++] = mk_label(hwnd, L"Duration:", 20, 168, 150, 20);
            HWND rShort = CreateWindow(L"BUTTON", L"Short", WS_CHILD|BS_AUTORADIOBUTTON|WS_GROUP,
                20, 190, 100, 24, hwnd, (HMENU)IDC_RADIO_SHORT, NULL, NULL);
            HWND rMedium = CreateWindow(L"BUTTON", L"Medium", WS_CHILD|BS_AUTORADIOBUTTON,
                130, 190, 100, 24, hwnd, (HMENU)IDC_RADIO_MEDIUM, NULL, NULL);
            HWND rLong = CreateWindow(L"BUTTON", L"Long", WS_CHILD|BS_AUTORADIOBUTTON,
                240, 190, 100, 24, hwnd, (HMENU)IDC_RADIO_LONG, NULL, NULL);
            SendMessage(rMedium, BM_SETCHECK, BST_CHECKED, 0);
            g_panelReactive[g_panelReactiveCount++] = rShort;
            g_panelReactive[g_panelReactiveCount++] = rMedium;
            g_panelReactive[g_panelReactiveCount++] = rLong;

            // --- Wheel panel ---
            HWND chkRev = CreateWindow(L"BUTTON", L"Reverse Direction", WS_CHILD|BS_AUTOCHECKBOX,
                20, 105, 200, 20, hwnd, (HMENU)IDC_CHK_WHEEL_REVERSE, NULL, NULL);
            g_panelWheel[g_panelWheelCount++] = chkRev;
            g_panelWheel[g_panelWheelCount++] = mk_label(hwnd, L"Speed:", 20, 135, 150, 20);
            g_panelWheel[g_panelWheelCount++] = mk_slider(hwnd, IDC_SLIDER_WHSPEED, 20, 155, 340, 30, 1, 20, g_wheelSpeed);

            // --- Lightning panel ---
            g_panelLightning[g_panelLightningCount++] = mk_label(hwnd, L"Speed:", 20, 105, 200, 20);
            g_panelLightning[g_panelLightningCount++] = mk_slider(hwnd, IDC_SLIDER_LTSPEED, 20, 125, 340, 30, 1, 10, g_lightningSpeed);
            g_panelLightning[g_panelLightningCount++] = mk_label(hwnd, L"Smoothness:", 20, 165, 200, 20);
            g_panelLightning[g_panelLightningCount++] = mk_slider(hwnd, IDC_SLIDER_LTSMOOTH, 20, 185, 340, 30, 1, 20, g_lightningSmooth);
            g_panelLightning[g_panelLightningCount++] = mk_label(hwnd, L"Strike Width:", 20, 225, 200, 20);
            g_panelLightning[g_panelLightningCount++] = mk_slider(hwnd, IDC_SLIDER_LTWIDTH, 20, 245, 340, 30, 1, 4, g_lightningWidth);
            g_hLblConcurrent = mk_label(hwnd, L"Concurrent Strikes: 1", 20, 285, 250, 20);
            g_panelLightning[g_panelLightningCount++] = g_hLblConcurrent;
            g_panelLightning[g_panelLightningCount++] = mk_slider(hwnd, IDC_SLIDER_LTCONCUR, 20, 305, 340, 30, 1, 4, g_lightningConcurrent);

            // --- Flame panel ---
            g_panelFlame[g_panelFlameCount++] = mk_label(hwnd, L"Flicker Speed:", 20, 105, 200, 20);
            g_panelFlame[g_panelFlameCount++] = mk_slider(hwnd, IDC_SLIDER_FLSPEED, 20, 125, 340, 30, 1, 10, g_flameSpeed);
            g_panelFlame[g_panelFlameCount++] = mk_label(hwnd, L"Smoothness:", 20, 165, 200, 20);
            g_panelFlame[g_panelFlameCount++] = mk_slider(hwnd, IDC_SLIDER_FLSMOOTH, 20, 185, 340, 30, 1, 20, g_flameSmooth);

            // --- Rain panel ---
            g_panelRain[g_panelRainCount++] = mk_label(hwnd, L"Fall Speed:", 20, 105, 200, 20);
            g_panelRain[g_panelRainCount++] = mk_slider(hwnd, IDC_SLIDER_RNSPEED, 20, 125, 340, 30, 1, 10, g_rainSpeed);
            g_panelRain[g_panelRainCount++] = mk_label(hwnd, L"Density (simultaneous drops):", 20, 165, 300, 20);
            for (int c=0;c<5;c++) {
                g_rainDensityBtn[c] = mk_button(hwnd, (const wchar_t*[]){L"1",L"2",L"3",L"4",L"5"}[c], IDC_RAIN_DENSITY_BASE+c, 20+c*68, 185, 60, 30);
                g_panelRain[g_panelRainCount++] = g_rainDensityBtn[c];
            }
            g_panelRain[g_panelRainCount++] = mk_label(hwnd, L"Color Count:", 20, 225, 200, 20);
            for (int c=0;c<5;c++) {
                g_rainColorCountBtn[c] = mk_button(hwnd, (const wchar_t*[]){L"1",L"2",L"3",L"4",L"5"}[c], IDC_RAIN_COLORCNT_BASE+c, 20+c*68, 245, 60, 30);
                g_panelRain[g_panelRainCount++] = g_rainColorCountBtn[c];
            }
            for (int c=0;c<5;c++) {
                g_rainColorBtn[c] = mk_button(hwnd, L"", IDC_RAIN_COLOR_BASE+c, 20+c*68, 285, 60, 32);
                g_panelRain[g_panelRainCount++] = g_rainColorBtn[c];
            }

            // --- Matrix panel ---
            g_panelMatrix[g_panelMatrixCount++] = mk_label(hwnd, L"Streak Speed:", 20, 105, 200, 20);
            g_panelMatrix[g_panelMatrixCount++] = mk_slider(hwnd, IDC_SLIDER_MXSPEED, 20, 125, 340, 30, 1, 10, g_matrixSpeed);
            g_panelMatrix[g_panelMatrixCount++] = mk_label(hwnd, L"Style:", 20, 165, 200, 20);
            g_hComboMatrixStyle = CreateWindow(L"COMBOBOX", L"", WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL,
                20, 185, 340, 100, hwnd, (HMENU)IDC_COMBO_MXSTYLE, NULL, NULL);
            SendMessage(g_hComboMatrixStyle, CB_ADDSTRING, 0, (LPARAM)L"Diagonal Streaks");
            SendMessage(g_hComboMatrixStyle, CB_ADDSTRING, 0, (LPARAM)L"Vertical Columns");
            SendMessage(g_hComboMatrixStyle, CB_ADDSTRING, 0, (LPARAM)L"Laser Sweep");
            SendMessage(g_hComboMatrixStyle, CB_ADDSTRING, 0, (LPARAM)L"Ripple");
            SendMessage(g_hComboMatrixStyle, CB_SETCURSEL, 0, 0);
            g_panelMatrix[g_panelMatrixCount++] = g_hComboMatrixStyle;
            g_panelMatrix[g_panelMatrixCount++] = mk_label(hwnd, L"Density (Diagonal/Laser modes):", 20, 220, 300, 20);
            for (int c=0;c<5;c++) {
                g_matrixDensityBtn[c] = mk_button(hwnd, (const wchar_t*[]){L"1",L"2",L"3",L"4",L"5"}[c], IDC_MATRIX_DENSITY_BASE+c, 20+c*68, 240, 60, 30);
                g_panelMatrix[g_panelMatrixCount++] = g_matrixDensityBtn[c];
            }
            g_panelMatrix[g_panelMatrixCount++] = mk_label(hwnd, L"Color Count:", 20, 280, 200, 20);
            for (int c=0;c<3;c++) {
                g_matrixColorCountBtn[c] = mk_button(hwnd, (const wchar_t*[]){L"1",L"2",L"3"}[c], IDC_MATRIX_COLORCNT_BASE+c, 20+c*113, 300, 105, 30);
                g_panelMatrix[g_panelMatrixCount++] = g_matrixColorCountBtn[c];
            }
            for (int c=0;c<3;c++) {
                g_matrixColorBtn[c] = mk_button(hwnd, L"", IDC_MATRIX_COLOR_BASE+c, 20+c*113, 340, 105, 32);
                g_panelMatrix[g_panelMatrixCount++] = g_matrixColorBtn[c];
            }

            // --- Always-visible: brightness ---
            mk_label(hwnd, L"Brightness:", 20, 400, 150, 20);
            g_hSliderBright = mk_slider(hwnd, IDC_SLIDER_BRIGHT, 20, 420, 340, 30, 0, 100, g_brightness);

            g_hStatus = CreateWindow(L"STATIC", L"Active Mode: None", WS_CHILD|WS_VISIBLE|SS_LEFT,
                20, 465, 340, 25, hwnd, (HMENU)IDC_STATIC_STATUS, NULL, NULL);

            set_swatch_color(g_hSwatchBreath, g_breathColor);
            set_swatch_color(g_hSwatchReact, g_reactiveColor);

            refresh_rain_color_visibility();
            refresh_matrix_color_visibility();
            show_panel_for_mode(-1); // hide all initially

            EnumChildWindows(hwnd, SetFontOnChild, (LPARAM)hFont);
            return 0;
        }

        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
            int id = dis->CtlID;
            if (id >= IDC_RAIN_COLOR_BASE && id < IDC_RAIN_COLOR_BASE+5) {
                draw_button_ex(dis, TRUE, g_rainColors[id-IDC_RAIN_COLOR_BASE]);
            } else if (id >= IDC_MATRIX_COLOR_BASE && id < IDC_MATRIX_COLOR_BASE+3) {
                draw_button_ex(dis, TRUE, g_matrixColors[id-IDC_MATRIX_COLOR_BASE]);
            } else if (id >= IDC_RAIN_DENSITY_BASE && id < IDC_RAIN_DENSITY_BASE+5) {
                draw_selector_button(dis, (id-IDC_RAIN_DENSITY_BASE+1) == (int)g_rainDensityTarget);
            } else if (id >= IDC_RAIN_COLORCNT_BASE && id < IDC_RAIN_COLORCNT_BASE+5) {
                draw_selector_button(dis, (id-IDC_RAIN_COLORCNT_BASE+1) == (int)g_rainColorCount);
            } else if (id >= IDC_MATRIX_DENSITY_BASE && id < IDC_MATRIX_DENSITY_BASE+5) {
                draw_selector_button(dis, (id-IDC_MATRIX_DENSITY_BASE+1) == (int)g_matrixDensityTarget);
            } else if (id >= IDC_MATRIX_COLORCNT_BASE && id < IDC_MATRIX_COLORCNT_BASE+3) {
                draw_selector_button(dis, (id-IDC_MATRIX_COLORCNT_BASE+1) == (int)g_matrixColorCount);
            } else {
                draw_button(dis);
            }
            return TRUE;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, CLR_TEXT);
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)g_bgBrush;
        }

        case WM_HSCROLL: {
            HWND ctrl = (HWND)lp;
            if (ctrl == g_hSliderBright) g_brightness = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_BSPEED) g_breathSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_BSMOOTH) g_breathSmooth = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_ANGLE) {
                g_waveAngle = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
                wchar_t buf[64];
                wsprintf(buf, L"Angle: %d degrees", (int)g_waveAngle);
                SetWindowText(g_hLblAngle, buf);
            }
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_WSPEED) g_waveSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_SPSPEED) g_sparkleSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_SPDENS) g_sparkleDensity = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_WHSPEED) g_wheelSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_LTSPEED) g_lightningSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_LTSMOOTH) g_lightningSmooth = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_LTWIDTH) g_lightningWidth = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_LTCONCUR) {
                g_lightningConcurrent = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
                wchar_t buf[64];
                wsprintf(buf, L"Concurrent Strikes: %d", (int)g_lightningConcurrent);
                SetWindowText(g_hLblConcurrent, buf);
            }
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_FLSPEED) g_flameSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_FLSMOOTH) g_flameSmooth = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_RNSPEED) g_rainSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            else if (GetDlgCtrlID(ctrl) == IDC_SLIDER_MXSPEED) g_matrixSpeed = (LONG)SendMessage(ctrl, TBM_GETPOS, 0, 0);
            return 0;
        }

        case WM_COMMAND:
            if (HIWORD(wp) == CBN_SELCHANGE && (HWND)lp == g_hComboMode) {
                int sel = (int)SendMessage(g_hComboMode, CB_GETCURSEL, 0, 0);
                if (sel >= 0) {
                    show_panel_for_mode(sel);
                    if (sel == MODE_RAIN) refresh_rain_color_visibility();
                    if (sel == MODE_MATRIX) refresh_matrix_color_visibility();
                    start_mode(sel);
                }
                return 0;
            }
            if (HIWORD(wp) == CBN_SELCHANGE && (HWND)lp == g_hComboMatrixStyle) {
                g_matrixStyle = (LONG)SendMessage(g_hComboMatrixStyle, CB_GETCURSEL, 0, 0);
                return 0;
            }
            {
                int cid = LOWORD(wp);
                if (cid >= IDC_RAIN_COLOR_BASE && cid < IDC_RAIN_COLOR_BASE+5) {
                    int idx = cid - IDC_RAIN_COLOR_BASE;
                    g_rainColors[idx] = pick_color(hwnd, g_rainColors[idx]);
                    InvalidateRect(g_rainColorBtn[idx], NULL, TRUE);
                    return 0;
                }
                if (cid >= IDC_MATRIX_COLOR_BASE && cid < IDC_MATRIX_COLOR_BASE+3) {
                    int idx = cid - IDC_MATRIX_COLOR_BASE;
                    g_matrixColors[idx] = pick_color(hwnd, g_matrixColors[idx]);
                    InvalidateRect(g_matrixColorBtn[idx], NULL, TRUE);
                    return 0;
                }
                if (cid >= IDC_RAIN_DENSITY_BASE && cid < IDC_RAIN_DENSITY_BASE+5) {
                    g_rainDensityTarget = cid - IDC_RAIN_DENSITY_BASE + 1;
                    invalidate_group(g_rainDensityBtn, 5);
                    return 0;
                }
                if (cid >= IDC_RAIN_COLORCNT_BASE && cid < IDC_RAIN_COLORCNT_BASE+5) {
                    g_rainColorCount = cid - IDC_RAIN_COLORCNT_BASE + 1;
                    invalidate_group(g_rainColorCountBtn, 5);
                    refresh_rain_color_visibility();
                    return 0;
                }
                if (cid >= IDC_MATRIX_DENSITY_BASE && cid < IDC_MATRIX_DENSITY_BASE+5) {
                    g_matrixDensityTarget = cid - IDC_MATRIX_DENSITY_BASE + 1;
                    invalidate_group(g_matrixDensityBtn, 5);
                    return 0;
                }
                if (cid >= IDC_MATRIX_COLORCNT_BASE && cid < IDC_MATRIX_COLORCNT_BASE+3) {
                    g_matrixColorCount = cid - IDC_MATRIX_COLORCNT_BASE + 1;
                    invalidate_group(g_matrixColorCountBtn, 3);
                    refresh_matrix_color_visibility();
                    return 0;
                }
            }
            switch (LOWORD(wp)) {
                case IDC_BTN_STOP:
                    stop_current_effect();
                    set_status(L"Active Mode: None");
                    show_panel_for_mode(-1);
                    SendMessage(g_hComboMode, CB_SETCURSEL, (WPARAM)-1, 0);
                    break;
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
                case IDC_CHK_WHEEL_REVERSE:
                    g_wheelReverse = (IsDlgButtonChecked(hwnd, IDC_CHK_WHEEL_REVERSE) == BST_CHECKED) ? 1 : 0;
                    break;
                case IDC_RADIO_SHORT:  g_reactiveDuration = 10; break;
                case IDC_RADIO_MEDIUM: g_reactiveDuration = 20; break;
                case IDC_RADIO_LONG:   g_reactiveDuration = 45; break;
                case IDC_TRAY_RESTORE:
                    ShowWindow(hwnd, SW_SHOW);
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    break;
                case IDC_TRAY_EXIT:
                    DestroyWindow(hwnd);
                    break;
            }
            return 0;

        case WM_SYSCOMMAND:
            if ((wp & 0xFFF0) == SC_MINIMIZE) {
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            return DefWindowProc(hwnd, msg, wp, lp);

        case WM_TRAYICON:
            if (lp == WM_LBUTTONDBLCLK || lp == WM_LBUTTONUP) {
                ShowWindow(hwnd, SW_SHOW);
                ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
            } else if (lp == WM_RBUTTONUP) {
                POINT pt; GetCursorPos(&pt);
                HMENU menu = CreatePopupMenu();
                AppendMenu(menu, MF_STRING, IDC_TRAY_RESTORE, L"Open");
                AppendMenu(menu, MF_SEPARATOR, 0, NULL);
                AppendMenu(menu, MF_STRING, IDC_TRAY_EXIT, L"Exit");
                SetForegroundWindow(hwnd);
                TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(menu);
            }
            return 0;

        case WM_DESTROY:
            stop_current_effect();
            Shell_NotifyIcon(NIM_DELETE, &g_nid);
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
        MessageBox(NULL, L"Could not open device! Make sure GCUBridge service is stopped.", L"Error", MB_OK|MB_ICONERROR);
        return 1;
    }
    if (enter_custom_mode() < 0) {
        MessageBox(NULL, L"Could not enter per-key mode!", L"Error", MB_OK|MB_ICONERROR);
        hid_close(g_h); hid_exit();
        return 1;
    }

    INITCOMMONCONTROLSEX icc; icc.dwSize = sizeof(icc); icc.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    WNDCLASS swc = {0};
    swc.lpfnWndProc = SwatchProc;
    swc.hInstance = hInst;
    swc.lpszClassName = L"SwatchClass";
    swc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&swc);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"RGBMainWindowClass";
    g_bgBrush = CreateSolidBrush(CLR_BG);
    wc.hbrBackground = g_bgBrush;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPICON));
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"RegisterClass failed!", L"Error", MB_OK|MB_ICONERROR);
        hid_close(g_h); hid_exit();
        return 1;
    }

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Better RGB by TheYamo",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 570,
        NULL, NULL, hInst, NULL);
    if (!hwnd) {
        MessageBox(NULL, L"CreateWindow failed!", L"Error", MB_OK|MB_ICONERROR);
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