// anim_editor.c
// The Animation Editor - a separate window where the user will design
// custom per-key RGB animations frame by frame.
//
// This file only needs to know about:
//   - the KEYMAP data and get_key_x() (from shared.h)
//   - the dark theme colors (also from shared.h)
//   - standard Windows GDI functions (from windows.h)
// It does NOT need to know about hidapi, the USB protocol, or any of the
// live effect threads in rgb_gui.c - that separation is the whole point
// of splitting this into its own file.

#define UNICODE
#define _UNICODE
#include <windows.h>
#include "shared.h"
#include "anim_editor.h"

// ================================================================
// ANIMATION EDITOR - Stage 1: visual keyboard canvas (read-only)
// ================================================================
// This section builds a SEPARATE top-level window that shows a visual
// representation of the physical keyboard, using the same (row, x)
// position data we already computed for the Wave/Wheel/Static effects.
// For now it's read-only - just proving the layout renders correctly
// before we add click-to-paint and animation logic in later stages.

#define ANIM_CANVAS_MARGIN 20   // empty border around the whole drawing
#define ANIM_KEY_UNIT      40   // pixels per one "x-unit" of physical key spacing
#define ANIM_KEY_SIZE      34   // size of each drawn key square (leaves a ~6px gap between keys)

static HWND g_hAnimEditorWindow = NULL;

// Converts a key's logical (row, x) position - the same values get_key_x()
// and KEYMAP[i].row already give us - into a pixel position inside the canvas.
// This is the ONLY place that knows how "1 unit of keyboard space" maps to
// "40 pixels on screen" - if we ever want a bigger or smaller editor window,
// this is the one function to change.
void key_to_canvas_pixel(const KeyEntry *k, int *outX, int *outY) {
    double x = get_key_x(k);
    *outX = ANIM_CANVAS_MARGIN + (int)(x * ANIM_KEY_UNIT);
    *outY = ANIM_CANVAS_MARGIN + k->row * ANIM_KEY_UNIT;
}

// The window procedure for the canvas itself. Right now it only responds
// to WM_PAINT (Windows asking "please redraw yourself"). Later stages will
// add WM_LBUTTONDOWN / WM_MOUSEMOVE / WM_LBUTTONUP here for click-painting.
LRESULT CALLBACK AnimCanvasProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill the whole canvas with our dark theme background first,
            // otherwise Windows would leave whatever was there before.
            RECT client;
            GetClientRect(hwnd, &client);
            HBRUSH bgBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &client, bgBrush);
            DeleteObject(bgBrush);

            // Draw every physical key as a small rounded square at its
            // mapped pixel position. We don't have per-key animation colors
            // yet (that's Stage 2), so every key is drawn the same neutral
            // "panel" color for now - this step is purely about confirming
            // the shape/spacing of the keyboard looks right.
            for (size_t i = 0; i < KEYMAP_COUNT; i++) {
                int px, py;
                key_to_canvas_pixel(&KEYMAP[i], &px, &py);

                HBRUSH keyBrush = CreateSolidBrush(CLR_PANEL);
                HPEN keyPen = CreatePen(PS_SOLID, 1, CLR_BORDER);
                HGDIOBJ oldBrush = SelectObject(hdc, keyBrush);
                HGDIOBJ oldPen = SelectObject(hdc, keyPen);
                RoundRect(hdc, px, py, px + ANIM_KEY_SIZE, py + ANIM_KEY_SIZE, 6, 6);
                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(keyBrush);
                DeleteObject(keyPen);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLOSE:
            // Hide instead of destroy, so re-opening the editor doesn't
            // need to recreate everything from scratch.
            ShowWindow(hwnd, SW_HIDE);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// Called when the user clicks "Animation Editor..." in the main window.
// Creates the editor window the first time, or just brings it back to
// the front if it already exists (so we don't register the window class
// or leak a second window if the button is clicked repeatedly).
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

    // The widest key position is around x=20.5 (the End key, etc.), so we
    // reserve room for about 21 units of width, plus margins on both sides.
    int canvasWidth  = ANIM_CANVAS_MARGIN*2 + (int)(21 * ANIM_KEY_UNIT);
    int canvasHeight = ANIM_CANVAS_MARGIN*2 + 7 * ANIM_KEY_UNIT;

    g_hAnimEditorWindow = CreateWindow(wc.lpszClassName, L"Animation Editor - Keyboard Layout",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, canvasWidth + 16, canvasHeight + 40,
        NULL, NULL, hInst, NULL);

    ShowWindow(g_hAnimEditorWindow, SW_SHOW);
}
