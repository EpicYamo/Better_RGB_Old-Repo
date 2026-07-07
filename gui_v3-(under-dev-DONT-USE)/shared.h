// shared.h
// Data and constants shared between rgb_gui.c and any new modular files
// (like anim_editor.c). Anything another .c file needs to see must be
// declared here with "extern" (for data) or a plain prototype (for
// functions) - the actual definition still lives in exactly one .c file.

#ifndef SHARED_H
#define SHARED_H

#include <windows.h>

// ---------------------------------------------------------------
// KEYMAP - the reverse-engineered key name -> hardware offset table,
// plus each key's logical (row, col) position used for spatial effects.
// The real data is defined once in better_rgb_gui_v3.c; this is just the "shape"
// of it so other files can read it.
// ---------------------------------------------------------------
typedef struct { const char *name; int offset; int row; int col; } KeyEntry;

#define KEYMAP_COUNT 102
extern const KeyEntry KEYMAP[KEYMAP_COUNT];

// Converts a key's logical (row, col) into its real physical x position
// (accounting for wide keys, the numpad gap, and hand-tuned overrides).
// Defined in better_rgb_gui_v3.c.
double get_key_x(const KeyEntry *k);

// THEME COLORS - shared so new windows (like the animation editor)
// visually match the main application.
#define CLR_BG        RGB(24,24,28)
#define CLR_PANEL     RGB(45,45,52)
#define CLR_PANEL_HOV RGB(55,55,64)
#define CLR_ACCENT    RGB(127,90,240)
#define CLR_ACCENT_LT RGB(170,140,255)
#define CLR_TEXT      RGB(235,235,240)
#define CLR_BORDER    RGB(70,70,80)

#endif