#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string.h>
#include "shared.h"
#include "anim_editor.h"

#define ANIM_CANVAS_MARGIN 20
#define ANIM_KEY_UNIT      50
#define ANIM_KEY_GAP       2

static HWND g_hAnimEditorWindow = NULL;
static HWND g_hChkShowLabels = NULL;
static int g_showLabels = 1;
#define CHECKBOX_AREA_HEIGHT 36
#define IDC_ANIM_CHK_LABELS 700

const char* get_key_display_label(const char *name) {
    struct { const char *name; const char *label; } table[] = {
        {"BracketL","["},{"BracketR","]"},{"Semicolon",";"},{"Quote","'"},
        {"Backslash","\\"},{"ISO_Backslash","\\"},{"Comma",","},{"Period","."},
        {"Slash","/"},{"Grave","`"},{"Minus","-"},{"Equals","="},
        {"FnLockCam","FnLk"},{"CopilotKey","Copilot"},{"CapsLock","Caps"},
        {"ShiftL","Shift"},{"ShiftR","Shift"},{"CtrlL","Ctrl"},{"AltL","Alt"},
        {"NumpadDiv","/"},{"NumpadMul","*"},{"NumpadMinus","-"},{"NumpadPlus","+"},
        {"NumpadDecimal","."},{"NumpadEnter","Enter"},
        {"Numpad0","0"},{"Numpad1","1"},{"Numpad2","2"},{"Numpad3","3"},{"Numpad4","4"},
        {"Numpad5","5"},{"Numpad6","6"},{"Numpad7","7"},{"Numpad8","8"},{"Numpad9","9"},
        {"ArrowLeft","<"},{"ArrowRight",">"},{"ArrowUp","^"},{"ArrowDown","v"},
    };
    for (size_t i=0;i<sizeof(table)/sizeof(table[0]);i++)
        if (strcmp(table[i].name, name) == 0) return table[i].label;
    return name;
}

typedef struct { const char *name; float left; float width; } KeyVisualLayout;

static const KeyVisualLayout VISUAL_LAYOUT[] = {
    {"Esc", 0.00f, 0.93f},{"F1", 1.20f, 0.95f},{"F2", 2.42f, 0.95f},{"F3", 3.64f, 0.93f},{"F4", 4.84f, 0.95f},
    {"F5", 6.04f, 0.95f},{"F6", 7.27f, 0.93f},{"F7", 8.47f, 0.95f},{"F8", 9.67f, 0.95f},{"F9", 10.91f, 0.93f},
    {"F10", 12.09f, 0.95f},{"F11", 13.35f, 0.93f},{"F12", 14.56f, 0.91f},{"FnLockCam", 15.76f, 0.93f},
    {"PrtSc", 16.96f, 0.93f},{"Del", 18.20f, 0.91f},{"Home", 19.38f, 0.93f},{"PgUp", 20.62f, 0.93f},
    {"PgDn", 21.84f, 0.91f},{"End", 23.04f, 0.93f},
    {"Grave", 0.00f, 1.00f},{"1", 1.25f, 1.02f},{"2", 2.55f, 1.00f},{"3", 3.84f, 1.00f},{"4", 5.09f, 1.00f},
    {"5", 6.40f, 1.00f},{"6", 7.67f, 1.00f},{"7", 8.95f, 1.00f},{"8", 10.22f, 1.00f},{"9", 11.51f, 1.00f},
    {"0", 12.78f, 1.00f},{"Minus", 14.04f, 1.00f},{"Equals", 15.35f, 1.00f},{"Backspace", 16.60f, 2.27f},
    {"NumLock", 19.18f, 1.00f},{"NumpadDiv", 20.44f, 1.00f},{"NumpadMul", 21.73f, 1.00f},{"NumpadMinus", 23.02f, 1.00f},
    {"Tab", 0.00f, 1.65f},{"Q", 1.93f, 1.00f},{"W", 3.18f, 1.02f},{"E", 4.49f, 1.00f},{"R", 5.75f, 1.00f},
    {"T", 7.04f, 1.00f},{"Y", 8.33f, 1.00f},{"U", 9.60f, 1.00f},{"I", 10.85f, 1.00f},{"O", 12.16f, 1.00f},
    {"P", 13.44f, 1.00f},{"BracketL", 14.69f, 1.00f},{"BracketR", 16.00f, 0.98f},{"Enter", 17.25f, 1.62f},
    {"Numpad7", 19.18f, 1.00f},{"Numpad8", 20.44f, 1.00f},{"Numpad9", 21.73f, 1.00f},{"NumpadPlus", 23.02f, 1.00f},
    {"CapsLock", 0.00f, 2.29f},{"A", 2.55f, 1.00f},{"S", 3.84f, 1.00f},{"D", 5.09f, 1.00f},{"F", 6.40f, 1.00f},
    {"G", 7.67f, 1.00f},{"H", 8.95f, 1.00f},{"J", 10.22f, 1.00f},{"K", 11.51f, 1.00f},{"L", 12.78f, 1.00f},
    {"Semicolon", 14.04f, 1.00f},{"Quote", 15.35f, 1.00f},{"Backslash", 16.60f, 1.00f},
    {"Numpad4", 19.16f, 1.00f},{"Numpad5", 20.44f, 1.00f},{"Numpad6", 21.71f, 1.00f},
    {"ShiftL", 0.00f, 1.65f},{"ISO_Backslash", 1.93f, 1.00f},{"Z", 3.18f, 1.02f},{"X", 4.49f, 1.00f},
    {"C", 5.75f, 1.00f},{"V", 7.04f, 1.02f},{"B", 8.33f, 1.00f},{"N", 9.60f, 1.00f},{"M", 10.85f, 1.00f},
    {"Comma", 12.16f, 1.00f},{"Period", 13.44f, 1.00f},{"Slash", 14.69f, 1.00f},{"ShiftR", 15.98f, 2.93f},
    {"Numpad1", 19.20f, 1.00f},{"Numpad2", 20.47f, 1.00f},{"Numpad3", 21.73f, 1.00f},{"NumpadEnter", 23.02f, 1.00f},
    {"CtrlL", 0.00f, 1.65f},{"Fn", 1.93f, 1.00f},{"Windows", 3.18f, 1.00f},{"AltL", 4.49f, 1.00f},
    {"Space", 5.75f, 6.07f},{"AltGr", 12.09f, 1.00f},{"CopilotKey", 13.40f, 2.18f},{"ArrowUp", 17.13f, 1.00f},
    {"Numpad0", 19.67f, 1.71f},{"NumpadDecimal", 21.67f, 1.00f},
    {"ArrowLeft", 15.84f, 1.00f},{"ArrowDown", 17.07f, 1.13f},{"ArrowRight", 18.42f, 0.98f},
};
#define VISUAL_LAYOUT_COUNT 102

static const float ROW_TOP[7]    = {0.00f, 0.91f, 2.20f, 3.47f, 4.75f, 6.02f, 7.22f};
static const float ROW_HEIGHT[7] = {0.66f, 1.02f, 1.00f, 1.00f, 1.00f, 1.02f, 0.98f};

void get_key_visual_rect(const char *name, float *outLeft, float *outWidth) {
    for (int i = 0; i < VISUAL_LAYOUT_COUNT; i++) {
        if (strcmp(VISUAL_LAYOUT[i].name, name) == 0) {
            *outLeft = VISUAL_LAYOUT[i].left;
            *outWidth = VISUAL_LAYOUT[i].width;
            return;
        }
    }
    *outLeft = 0.0f;
    *outWidth = 1.0f;
}

void key_to_canvas_rect(const KeyEntry *k, int *outX, int *outY, int *outW, int *outH) {
    float left, width;
    get_key_visual_rect(k->name, &left, &width);

    *outX = ANIM_CANVAS_MARGIN + (int)(left * ANIM_KEY_UNIT);
    *outY = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT + (int)(ROW_TOP[k->row] * ANIM_KEY_UNIT);
    *outW = (int)(width * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
    *outH = (int)(ROW_HEIGHT[k->row] * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
}

int is_tall_key(const char *name) {
    return strcmp(name, "NumpadPlus") == 0 || strcmp(name, "NumpadEnter") == 0;
}

void draw_tall_key(HDC hdc, const KeyEntry *k, HBRUSH brush, HPEN pen) {
    float left, width;
    get_key_visual_rect(k->name, &left, &width);
    int x = ANIM_CANVAS_MARGIN + (int)(left * ANIM_KEY_UNIT);
    int y = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT + (int)(ROW_TOP[k->row] * ANIM_KEY_UNIT);
    int w = (int)(width * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
    int bottomRow = k->row + 1;
    int yBottom = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT
                + (int)((ROW_TOP[bottomRow] + ROW_HEIGHT[bottomRow]) * ANIM_KEY_UNIT) - ANIM_KEY_GAP;

    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, x, y, x + w, yBottom, 6, 6);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
}

void draw_enter_key(HDC hdc, HBRUSH brush, HPEN pen) {
    const float xA = 17.87f, xB = 18.87f;
    const float xC = 17.25f;

    int xAi = ANIM_CANVAS_MARGIN + (int)(xA * ANIM_KEY_UNIT);
    int xBi = ANIM_CANVAS_MARGIN + (int)(xB * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
    int xCi = ANIM_CANVAS_MARGIN + (int)(xC * ANIM_KEY_UNIT);

    int y0 = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT + (int)(ROW_TOP[2] * ANIM_KEY_UNIT);
    int y1 = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT + (int)(ROW_TOP[3] * ANIM_KEY_UNIT);
    int y2 = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT
           + (int)((ROW_TOP[3] + ROW_HEIGHT[3]) * ANIM_KEY_UNIT) - ANIM_KEY_GAP;

    POINT pts[6] = {
        {xAi, y0}, {xBi, y0}, {xBi, y2}, {xCi, y2}, {xCi, y1}, {xAi, y1}
    };

    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    Polygon(hdc, pts, 6);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
}

LRESULT CALLBACK AnimCanvasProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);


            RECT client;
            GetClientRect(hwnd, &client);
            HBRUSH bgBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &client, bgBrush);
            DeleteObject(bgBrush);

            HFONT labelFont = CreateFont(-13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
            HGDIOBJ oldFont = SelectObject(hdc, labelFont);




            for (size_t i = 0; i < KEYMAP_COUNT; i++) {
                const char *name = KEYMAP[i].name;
                int px, py, pw, ph;

                HBRUSH keyBrush = CreateSolidBrush(CLR_PANEL);
                HPEN keyPen = CreatePen(PS_SOLID, 1, CLR_BORDER);

                if (strcmp(name, "Enter") == 0) {
                    draw_enter_key(hdc, keyBrush, keyPen);
                    px = ANIM_CANVAS_MARGIN + (int)(17.25f * ANIM_KEY_UNIT);
                    py = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT + (int)(ROW_TOP[3] * ANIM_KEY_UNIT);
                    pw = (int)(1.62f * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
                    ph = (int)(ROW_HEIGHT[3] * ANIM_KEY_UNIT) - ANIM_KEY_GAP;
                } else if (is_tall_key(name)) {
                    draw_tall_key(hdc, &KEYMAP[i], keyBrush, keyPen);
                    key_to_canvas_rect(&KEYMAP[i], &px, &py, &pw, &ph);
                    int bottomRow = KEYMAP[i].row + 1;
                    ph = ANIM_CANVAS_MARGIN + CHECKBOX_AREA_HEIGHT
                       + (int)((ROW_TOP[bottomRow] + ROW_HEIGHT[bottomRow]) * ANIM_KEY_UNIT) - ANIM_KEY_GAP - py;
                } else {
                    key_to_canvas_rect(&KEYMAP[i], &px, &py, &pw, &ph);
                    HGDIOBJ oldBrush = SelectObject(hdc, keyBrush);
                    HGDIOBJ oldPen = SelectObject(hdc, keyPen);
                    RoundRect(hdc, px, py, px + pw, py + ph, 6, 6);
                    SelectObject(hdc, oldBrush);
                    SelectObject(hdc, oldPen);
                }

                DeleteObject(keyBrush);
                DeleteObject(keyPen);

                if (g_showLabels) {
                    RECT labelRc = { px, py, px + pw, py + ph };
                    const char *label = get_key_display_label(name);
                    wchar_t wlabel[32];
                    MultiByteToWideChar(CP_UTF8, 0, label, -1, wlabel, 32);
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, CLR_TEXT);
                    DrawTextW(hdc, wlabel, -1, &labelRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }

            SelectObject(hdc, oldFont);
            DeleteObject(labelFont);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wp) == IDC_ANIM_CHK_LABELS) {
                g_showLabels = (IsDlgButtonChecked(hwnd, IDC_ANIM_CHK_LABELS) == BST_CHECKED) ? 1 : 0;
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            }
            return 0;

        case WM_CLOSE:

            ShowWindow(hwnd, SW_HIDE);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void open_animation_editor(HINSTANCE hInst) {
    if (g_hAnimEditorWindow) {
        ShowWindow(g_hAnimEditorWindow, SW_SHOW);
        SetForegroundWindow(g_hAnimEditorWindow);
        return;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = AnimCanvasProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"AnimEditorCanvasClass";
    wc.hbrBackground = CreateSolidBrush(CLR_BG);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    int canvasWidth  = ANIM_CANVAS_MARGIN*2 + (int)(24 * ANIM_KEY_UNIT);
    int canvasHeight = ANIM_CANVAS_MARGIN*2 + CHECKBOX_AREA_HEIGHT + (int)(8.3 * ANIM_KEY_UNIT);

    g_hAnimEditorWindow = CreateWindow(wc.lpszClassName, L"Animation Editor - Keyboard Layout",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, canvasWidth + 16, canvasHeight + 40,
        NULL, NULL, hInst, NULL);

    g_hChkShowLabels = CreateWindow(L"BUTTON", L"Show Key Labels", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
        ANIM_CANVAS_MARGIN, 8, 200, 24, g_hAnimEditorWindow, (HMENU)IDC_ANIM_CHK_LABELS, hInst, NULL);
    SendMessage(g_hChkShowLabels, BM_SETCHECK, g_showLabels ? BST_CHECKED : BST_UNCHECKED, 0);

    ShowWindow(g_hAnimEditorWindow, SW_SHOW);
}