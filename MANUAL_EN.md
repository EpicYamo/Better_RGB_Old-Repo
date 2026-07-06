# Better RGB by TheYamo — User Manual (English)

A custom per-key RGB control application for Tongfang/ITE8291-based keyboards (VID_048D, PID_600B).

---

## 1. Before you start — important setup step

This application talks directly to the keyboard's USB interface. The vendor's own RGB service ("GCUBridge") also talks to the same interface, and if both try to control the lighting at the same time, you'll get flickering, conflicts, or in rare cases the keyboard's embedded controller can lock up entirely.

**You do not need to disable the GCUBridge Windows service.** If you do, you'll lose the *other* features of the vendor Control Center software (fan curves, performance modes, etc. — whatever else it manages on your system).

**What you must do instead:**

1. Open the vendor's **Control Center** application.
2. Find the keyboard lighting / RGB section.
3. **Turn keyboard lighting OFF** inside that app.

As long as lighting is switched off there, the GCUBridge service can keep running normally in the background for its other functions, and it will not touch the RGB interface — leaving it free for this application to control safely.

⚠️ **Do not run this application while lighting is enabled in the Control Center at the same time.** Always disable it there first.

---

## 2. Building from source

If you received only the source files, you'll need to compile it yourself. You need:
- `mingw-w64` (gcc for Windows) with `windres`
- The [hidapi](https://github.com/libusb/hidapi) source (only `hidapi/hidapi.h` and `windows/hid.c` are needed)

Files required in the same folder:
- `rgb_gui.c` (main source)
- `resource.rc`
- `resource.h`
- `app.manifest`
- `app.ico`

Build (two steps, from that folder):
```
windres resource.rc -O coff -o resource.o
gcc rgb_gui.c resource.o PATH_TO_HIDAPI\windows\hid.c -IPATH_TO_HIDAPI\hidapi -o rgb_gui.exe -lsetupapi -lhid -lm -mwindows -lcomdlg32 -lcomctl32 -lshell32
```
Replace `PATH_TO_HIDAPI` with wherever you placed the hidapi source folder.

**Note:** if any folder in your path contains spaces, wrap that whole path in double quotes.

---

## 3. Running the application

Double-click `rgb_gui.exe` (or run it from a terminal). No console window will appear — this is a normal windowed application.

If the keyboard can't be found (a "Could not open device" error appears), the most common cause is that lighting is still enabled in the vendor Control Center — go back to Section 1.

---

## 4. The interface

- **Select Mode** — a dropdown at the top. Choosing an item immediately starts that lighting effect and shows only the settings relevant to it below.
- **Stop (Turn Off Lights)** — stops the current effect and turns the keyboard off.
- **Brightness** — a slider at the bottom, always visible regardless of which mode is active. Scales the overall output brightness (0–100%).
- **Active Mode** label — shows which effect is currently running.

Minimizing the window sends it to the system tray (the hidden icons area near the clock) instead of the taskbar — the app keeps running in the background. Double-click the tray icon to bring the window back, or right-click it for an Open/Exit menu.

---

## 5. Modes and their settings

### Breathing
The whole keyboard pulses in a single color.
- **Color** — click "Pick Color" to choose it.
- **Speed** — how fast the pulse cycles.
- **Smoothness** — how gradual the brightness transition looks. Higher = silkier, no visible steps.

### Wave
A moving color gradient sweeps across the keyboard.
- **Angle** — 0–359°. 0° sweeps left-to-right, 90° top-to-bottom, other values sweep diagonally. A live tooltip shows the exact degree while dragging.
- **Speed** — how fast the gradient moves.

### Sparkle
Random keys flash briefly.
- **Speed** — how long each flash lasts before fading (higher speed = shorter-lived, snappier flashes).
- **Density** — how many keys are lit at any given moment (3–20), independent of speed.

### Reactive
Keys light up as you type and fade out after release.
- **Color** — pick a fixed color, or click "Random" for a different random color on every keypress.
- **Duration** — Short / Medium / Long: how long a key stays lit (fading) after you release it. Note: while a key is held down, it stays at full brightness regardless of this setting — Duration only affects the fade-out after release.

### Wheel
A rainbow rotates around the center of the keyboard.
- **Reverse Direction** — check to spin counter-clockwise instead of clockwise.
- **Speed** — rotation speed.

### Lightning
A glowing "bolt" falls from the top of the keyboard to the bottom.
- **Speed** — how fast the bolt travels downward.
- **Smoothness** — how long and soft the trailing glow behind the bolt is. Higher values create a longer, softer-fading tail (and take correspondingly longer to fully disappear).
- **Strike Width** — 1 to 4 key-widths wide.
- **Concurrent Strikes** — how many independent bolts can be falling at the same time (1–4). The exact count is shown live next to the slider.

### Flame
A drifting fire simulation, warmer toward the bottom.
- **Flicker Speed** — how quickly the heat pattern shifts and re-randomizes.
- **Smoothness** — how gradually the color blends from one state to the next, rather than jumping.

### Rain
Colored drops fall down the keyboard with a short trailing glow.
- **Fall Speed** — how fast drops fall and complete their animation.
- **Density** — 1 to 5 steps; each step adds 3 simultaneous drops (step 1 = 3 drops, step 5 = 15 drops).
- **Color Count** — 1 to 5. Click a number to set how many colors are in the palette, then click each newly-appeared color box to assign it a color. Every drop is a single solid color for its whole lifetime, randomly chosen from the palette when it spawns.

### Matrix
Four different animation styles, selectable from the Style dropdown:
- **Diagonal Streaks** — colored bands sweep across the keyboard at random diagonal angles.
- **Vertical Columns** — the classic "digital rain" look, falling straight down each column.
- **Laser Sweep** — 2–3 sharp, fast horizontal/vertical lines sweep across.
- **Ripple** — expanding rings, like drops in water, spreading out from random points.

Settings:
- **Streak Speed** — overall animation speed.
- **Density** — affects the Diagonal Streaks and Laser Sweep styles: how many lines are active at once.
- **Color Count** — 1 to 3, same click-to-assign system as Rain.

---

## 6. Troubleshooting

**"Could not open device" on startup.**
Lighting is still enabled in the vendor Control Center — disable it there (Section 1) and try again.

**Keyboard lighting stops responding entirely, including to the vendor software.**
The keyboard's embedded controller has locked up — this is a known hardware quirk, not data corruption or permanent damage. To recover:
1. Shut the laptop down completely.
2. Unplug the charging cable.
3. Hold the power button for about 30 seconds.
4. Reconnect power and turn the laptop back on.

**Colored settings boxes from one mode appear while a different mode is selected.**
This was a bug in earlier versions and has been fixed; if you still see it, make sure you're running the latest build.

**Numpad Enter and the main Enter key both light up together in Reactive mode.**
This is a known limitation — Windows reports both keys with the same virtual-key code, so the application can't currently tell them apart.

**The Fn key or the Copilot key never lights up in Reactive mode.**
Also expected — these keys don't send a standard signal Windows can detect.
