// anim_editor.h
// Public interface to the Animation Editor window, for use by rgb_gui.c.

#ifndef ANIM_EDITOR_H
#define ANIM_EDITOR_H

#include <windows.h>

// Opens the Animation Editor window, creating it the first time it's
// called and just bringing it to the front on later calls.
void open_animation_editor(HINSTANCE hInst);

#endif