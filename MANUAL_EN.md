# Better RGB by TheYamo — User Manual (English)

A custom per-key RGB control application for Tongfang/ITE8291-based keyboards (VID_048D, PID_600B).

---

## 1. How this app coexists with the vendor software (v2.2 device lock)

This application talks directly to the keyboard's USB RGB interface. The vendor's own service ("GCUBridge" / Control Center) uses the same interface, and — as discovered the hard way — it periodically re-applies its own selected lighting state, silently overriding anything another program has set.

Since v2.2 the app solves this at the driver level: it opens the keyboard's RGB interface **exclusively**. While the app is running, no other process (including the vendor service) can open that interface at all, so nothing can override your lighting.

What this means in practice:

- **You no longer need to change any lighting setting in the Control Center.** Older versions asked you to turn keyboard lighting off there first — that step is gone.
- **The vendor service keeps running normally.** Fan curves, power limits, and performance modes are completely unaffected; they don't use this interface.
- **The Control Center's keyboard-lighting page will not load while this app is running.** That is intentional — it's the lock working. Close this app and the vendor's lighting controls become available again.

**First run:** the vendor service usually grabs the interface at boot, before you can launch anything. If the app starts and detects it doesn't hold the lock, it shows a dialog offering to fix it: the vendor service is restarted for a few seconds (its other functions are untouched) and the app relaunches holding the lock. Windows will ask for administrator approval once.

**To make this fully automatic**, use the **Auto-Start at Login** button (see Section 4). It installs a Scheduled Task that launches the app with administrator rights at every login — no UAC prompt, no dialogs; the app silently takes the lock every time you log in.

---

## 2. Building from source

You need:
- `mingw-w64` (gcc for Windows) with `windres`

The repository bundles its own copy of [hidapi](https://github.com/libusb/hidapi) in the `hidapi/` folder. **Use the bundled copy** — its `windows/hid.c` carries a small but essential patch: `open_device` first tries to open the device with `dwShareMode = 0` (exclusive) and only falls back to the stock shared mode if that fails. The exclusive device lock described in Section 1 does not work with an unpatched hidapi.

Files required:
- `better_rgb.c` (main source)
- `resource.rc`, `resource.h`, `app.manifest`, `app.ico`
- the bundled `hidapi/` folder

Build (two steps, from the project folder):
```
windres resource.rc -O coff -o resource.o
gcc better_rgb.c resource.o hidapi\windows\hid.c -Ihidapi\hidapi -o "Better RGB by TheYamo.exe" -lsetupapi -lhid -lm -mwindows -lcomdlg32 -lcomctl32 -lshell32 -ladvapi32 -O2
```

**Note:** if any folder in your path contains spaces, wrap that whole path in double quotes.

---

## 3. Running the application

Double-click the exe (or run it from a terminal). No console window will appear — this is a normal windowed application.

- If the app already holds the device lock (e.g. it was started via the Auto-Start task, or the vendor service released the interface), it starts silently.
- If the vendor service is holding the interface, you'll get the fix dialog described in Section 1. Choosing **Yes** triggers one UAC prompt, a brief vendor-service restart, and the app relaunches locked. Choosing **No** lets the app run anyway, but the vendor software may override your lighting at random moments.
- If the app is launched with administrator rights and finds the interface taken, it fixes the situation automatically with no dialog at all — this is exactly what the Auto-Start task does at every login.

All user-facing dialogs are shown in both English and Turkish.

---

## 4. The interface

- **Select Mode** — a dropdown at the top. Choosing an item immediately starts that lighting effect and shows only the settings relevant to it below.
- **Stop (Turn Off Lights)** — stops the current effect and turns the keyboard off.
- **Brightness** — a slider at the bottom, always visible regardless of which mode is active. Scales the overall output brightness (0–100%).
- **Active Mode** label — shows which effect is currently running.
- **Auto-Start at Login** button — shows the current state (ON/OFF) and toggles it. Turning it ON installs a Windows Scheduled Task ("Better RGB by TheYamo") that launches the app with administrator rights at every login, so the device lock is taken silently with no UAC prompts. Turning it OFF removes the task. Creating or removing the task itself requires one administrator approval.

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

### Static
A motionless layout: the keyboard is split into colored zones of your choice.
- **Horizontal / Vertical** — pick the split direction. Horizontal stacks up to **6** bands from top to bottom; Vertical places up to **10** bands side by side.
- **Zones** — slider selecting how many zones the keyboard is divided into (the maximum depends on the chosen direction).
- **Zone selector + Pick Color** — choose a zone from the dropdown, then assign its color. Repeat for each zone.

---

## 6. Troubleshooting

**"Could not open the keyboard device" on startup.**
The supported RGB interface (VID_048D / PID_600B, ITE8291) wasn't found on this machine. This app targets that specific controller; on a different keyboard/controller it cannot run.

**The Control Center's keyboard-lighting page doesn't load.**
Expected while this app is running — the device lock (Section 1) prevents the vendor software from opening the RGB interface. Close this app and the vendor's lighting page works again.

**Lighting randomly turned off / got replaced by a vendor color (older versions).**
This was the signature of the vendor service overriding the interface, and it's what the v2.2 exclusive lock eliminates. If you still see it on v2.2, the app is probably running without the lock (you declined the fix dialog, or the service grabbed the interface first). Restart the app and accept the fix, or enable Auto-Start at Login for a permanent silent fix.

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
