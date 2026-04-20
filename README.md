# 🎮 ESP32 Pong Game - Learn Electronics!

A fun pong game for the TTGO T-Display with sound effects. Choose your sound module:
- **DFPlayer Mini** — full MP3 playback from SD card, one track per event
- **ISD1820** — simple record-and-play chip, one recorded sound triggered on paddle hit or score lost


<img src="https://github.com/impressto/tdisplay-pong/blob/master/readme-banner.jpg" />


### The board

<img src="https://github.com/impressto/tdisplay-pong/blob/master/esp-pong.jpg" />


## What You'll Learn

- How to upload code to an ESP32
- How to wire a sound module (DFPlayer Mini or ISD1820) and trigger it from code
- How software controls hardware through GPIO pins
- Serial communication between devices (DFPlayer)
- How edge-triggered signals work (ISD1820)

<img src="https://github.com/impressto/tdisplay-pong/blob/master/pong-workshop.jpg" />

## What You Need

### Hardware
- **TTGO T-Display** (ESP32 with built-in screen)
- **USB-C cable** to connect to your computer
- **Small speaker** (3W 8ohm recommended)
- **Jumper wires** to connect things

**Choose one sound module:**

| Module | Pros | Cons |
|--------|------|------|
| **DFPlayer Mini** | Different sound per event, SD card holds many tracks | Needs SD card and MP3 files |
| **ISD1820** | No SD card needed, record your own voice/sound | Only one sound, no volume control |

### Software
- [VS Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)

## How to Play

### Onboard Buttons
1. **Left Button (GPIO 0)** - Move paddle UP
2. **Right Button (GPIO 35)** - Move paddle DOWN

### External Buttons for 2-Player Mode

To play **Human vs Human**, you need 4 external buttons — 2 for each player!

#### Wiring Diagram

All 4 buttons connect to the **right side** of the board, close to GND:

```
        T-Display (clone) - Right Side Pins
        ┌────────────────────────────────┐
        │            [USB-C]              │
        │                                 │
        │                          3V  ○ │
        │                       GPIO36 ○ │
        │                       GPIO37 ○ │
        │                       GPIO38 ○ │
        │                       GPIO39 ○ │
        │   P1 UP  [BTN]─────── GPIO32 ● │
        │   P1 DOWN[BTN]─────── GPIO33 ● │
        │   P2 UP  [BTN]─────── GPIO25 ● │
        │   P2 DOWN[BTN]─────── GPIO26 ● │
        │                       GPIO27 ○ │
        │   All buttons ───────── GND ● │
        │                           5V ○ │
        │           [SCREEN]             │
        └────────────────────────────────┘
        ● = used    ○ = not used
```

#### Pin Assignments

| Player | Action | GPIO Pin | Position (top to bottom) |
|--------|--------|----------|-------------------------|
| **Player 1** (Left paddle) | UP | GPIO 32 | 1st button |
| **Player 1** (Left paddle) | DOWN | GPIO 33 | 2nd button |
| **Player 2** (Right paddle) | UP | GPIO 25 | 3rd button |
| **Player 2** (Right paddle) | DOWN | GPIO 26 | 4th button |
| All buttons | Ground | GND | Right below GPIO27 |

#### How to Wire Each Button

```
Button          T-Display
┌─────┐
│     │
│ ┌─┐ │ Leg 1 ────────► GPIO pin (32, 33, 25, or 26)
│ └─┘ │
│     │ Leg 2 ────────► GND (any GND pin)
└─────┘
```

**Tips:**
- No resistors needed! The ESP32 has internal pull-up resistors.
- All 4 buttons share the same GND — you can daisy-chain them.
- Test each button before gluing anything down!

#### Enable 2-Player Mode in config.h

Make sure the Player 2 pins are set (they should be by default):

```c
#define EXT_P1_UP         32   // Player 1 UP
#define EXT_P1_DOWN       33   // Player 1 DOWN
#define EXT_P2_UP         25   // Player 2 UP
#define EXT_P2_DOWN       26   // Player 2 DOWN
```

### Gameplay
- Hit the ball with your paddle to score points!
- The ball appears to **roll** as it moves - watch it spin!
- If you miss, you lose a point

## Getting Started

### Step 1: Upload the Code

1. Connect your T-Display to your computer with a USB cable
2. Open this folder in VS Code
3. Click the **PlatformIO: Upload** button (→ arrow at the bottom)
4. Wait for it to finish - the game will start automatically!

### Step 2: Play the Game First!

Before wiring anything, play the game to understand when events happen:
- Ball bounces off walls
- Ball hits your paddle
- You score a point
- You miss the ball

## Wiring Up Sound!

<img src="https://github.com/impressto/tdisplay-pong/blob/master/pong-board.jpg" />



Pick **one** sound module and follow its wiring guide below. Then set the matching option in `config.h`.

---

### Option A: DFPlayer Mini

The DFPlayer Mini plays MP3 files from a micro SD card. You can assign a different sound to each game event.

#### Wiring

```
T-Display          DFPlayer Mini
---------          -------------
GPIO 17 (TX) ----> RX  (through 1K resistor recommended)
GPIO 21 (RX) <---- TX
GPIO 22      <---- BUSY  (optional — detects when playing)
GND          ----> GND
5V           ----> VCC
                   SPK1 ----> Speaker (+)
                   SPK2 ----> Speaker (-)
```

**Note:** Use 5V for louder output. The 1K resistor on TX protects the DFPlayer's RX pin.

#### Setting Up Sound Files

1. Format a micro SD card as FAT32
2. Create a folder named `mp3` in the root
3. Name your sound files: `0001.mp3`, `0002.mp3`, `0003.mp3`, etc.
4. Insert the SD card into the DFPlayer

**Recommended sound files:**
| File | Event |
|------|-------|
| `0001.mp3` | Score milestone award (every 5 points by default) |
| `0002.mp3` | Paddle hit (regular) |
| `0003.mp3` | Score lost |

#### Enable in config.h

```c
#define DFPLAYER_ENABLED   1    // Enable DFPlayer
#define ISD1820_ENABLED    0    // Disable ISD1820
```

---

### Option B: ISD1820 Voice Record Playback Module

The ISD1820 is the simplest way to add sound. It stores **one recorded sound** directly on the chip — no SD card, no MP3 files. Just record your voice or a sound effect by holding the REC button, and the ESP32 will trigger it during the game.

#### Recording Your Sound

1. Power the ISD1820 (3.3V or 5V)
2. **Hold the REC button** on the module and speak or make a sound into the microphone
3. **Release REC** when done — the sound is saved permanently, even without power
4. Press the PLAY button on the module to hear it back before wiring to the ESP32

#### Wiring

```
T-Display          ISD1820
---------          -------
GPIO 15      ----> P-E  (Play Edge-triggered)
GND          ----> GND
3.3V or 5V   ----> VCC
                   SP+  ----> Speaker (+)
                   SP-  ----> Speaker (-)
```

**Important:** Connect to the **P-E** pin (Play Edge), NOT the P-L pin. P-E plays the sound once when the pin goes HIGH, which is what we want.

#### Enable in config.h

```c
#define DFPLAYER_ENABLED            0    // Disable DFPlayer
#define ISD1820_ENABLED             1    // Enable ISD1820
#define ISD1820_TRIGGER_PADDLE_HIT  1    // Play on paddle hit (1=yes, 0=no)
#define ISD1820_TRIGGER_SCORE_LOST  1    // Play on score lost (1=yes, 0=no)
```

---

### No Sound Module?

That's fine too — just disable both in `config.h`:

```c
#define DFPLAYER_ENABLED   0
#define ISD1820_ENABLED    0
```

The game runs perfectly without any sound hardware attached.

---

## Game Events

| Event | When Does It Trigger? | DFPlayer | ISD1820 |
|-------|----------------------|----------|---------|
| Wall Bounce | Ball bounces off any wall | ❌ | ❌ |
| Paddle Hit | Ball hits your paddle | ✅ regular sound | ✅ (configurable) |
| Score Milestone | Score reaches a multiple of 5 | ✅ award sound | ❌ |
| Score Lost | You miss the ball | ✅ | ✅ (configurable) |

## Customizing the Game

Open `src/config.h` to change settings:

```c
// ---- DFPlayer ----
#define DFPLAYER_ENABLED   1     // 1 = enable, 0 = disable
#define DFPLAYER_TX_PIN    17
#define DFPLAYER_RX_PIN    21
#define DFPLAYER_VOLUME    10    // 0-30
#define TRACK_AWARD        1     // 0001.mp3 - plays at score milestones
#define TRACK_PADDLE_HIT   2     // 0002.mp3 - plays on regular paddle hit
#define TRACK_SCORE_LOST   3     // 0003.mp3 - plays on score lost
#define SCORE_MILESTONE_INTERVAL  5   // Award sound fires at 5, 10, 15, 20...

// ---- ISD1820 ----
#define ISD1820_ENABLED             0    // 1 = enable, 0 = disable
#define ISD1820_PLAY_PIN            15   // GPIO connected to P-E pin
#define ISD1820_TRIGGER_PADDLE_HIT  1    // 1 = trigger on paddle hit
#define ISD1820_TRIGGER_SCORE_LOST  1    // 1 = trigger on score lost
```

### Other Fun Settings to Try

```c
#define PADDLE_WIDTH     15    // Make paddle bigger = easier game
#define BALL_SPEED_START  3    // Higher number = slower ball
#define PADDLE_SPEED      2    // How fast paddle moves

// External button pins (set to -1 to disable)
#define EXT_BUTTON_DOWN  25    // External button - paddle left
#define EXT_BUTTON_UP    13    // External button - paddle right
```

## Adding Your Own Event Code! 🛠️

Want to customize what happens when game events occur? You can add your own custom code!

### Where to Put Your Code

Open `src/events.cpp` - this is where all the event magic happens!

**The file has 4 event functions you can modify:**

```c
void onWallBounce() {
  // This runs when the ball hits a wall
}

void onPaddleHit() {
  // This runs when you hit the ball
}

void onScoreGained() {
  // This runs when you score a point
}

void onScoreLost() {
  // This runs when you miss the ball
}
```

### Project Files Overview

| File | What's Inside |
|------|---------------|
| `src/main.cpp` | The game code (paddle, ball, score) |
| `src/config.h` | Settings you can change (pins, speeds, sizes) |
| `src/events.h` | List of event functions (don't change this) |
| `src/events.cpp` | **Your code goes here!** Event implementations |

## T-Display Pinout

Here are the GPIO pins used in this project:

### Right Side (Buttons)

| Pin | Usage | Notes |
|-----|-------|-------|
| 32 | P1 UP button | Player 1 paddle up |
| 33 | P1 DOWN button | Player 1 paddle down |
| 25 | P2 UP button | Player 2 paddle up |
| 26 | P2 DOWN button | Player 2 paddle down |
| GND | Ground | Shared by all buttons |

### Left Side (LEDs, Buzzer, Sound Modules)

| Pin | Usage | Notes |
|-----|-------|-------|
| 2 | Event LED | Score gained |
| 12 | Event LED | Wall bounce |
| 13 | Event LED | Paddle hit |
| 15 | Buzzer / ISD1820 | PWM buzzer or ISD1820 P-E pin |
| 17 | Event LED / DFPlayer TX | Score lost (or DFPlayer serial) |
| 21 | DFPlayer RX | Serial RX from DFPlayer TX |
| 22 | DFPlayer BUSY | Optional: detect when playing |

### Onboard Buttons

| Pin | Usage | Notes |
|-----|-------|-------|
| 0 | Left button | Menu / P1 up (onboard) |
| 35 | Right button | Menu / P1 down (input only) |

### Wiring Overview

```
         T-Display (clone) - Both Sides

    LEFT SIDE                      RIGHT SIDE
    (LEDs/Buzzer)                  (Buttons)
    ┌───────────────────────────────────┐
    │           [USB-C]              │
    │                                │
    │ ○ GND                   3V  ○ │
    │ ○ GND                GPIO36 ○ │
    │ ○ 21                 GPIO37 ○ │
    │ ○ 22                 GPIO38 ○ │
    │ ● 17 (Score Lost)    GPIO39 ○ │
    │ ● 2  (Score Gained)  GPIO32 ● │ P1 UP
    │ ● 15 (Buzzer)        GPIO33 ● │ P1 DOWN
    │ ● 13 (Paddle Hit)    GPIO25 ● │ P2 UP
    │ ● 12 (Wall Bounce)   GPIO26 ● │ P2 DOWN
    │ ○ GND                GPIO27 ○ │
    │ ○ GND                  GND ● │ All buttons
    │ ○ 3V                   5V  ○ │
    │           [SCREEN]            │
    └───────────────────────────────────┘
    ● = used    ○ = available
```

## Troubleshooting

### No sound from DFPlayer
- Check wiring: TX to RX, RX to TX (they cross over!)
- Make sure the SD card is formatted as FAT32
- Verify sound files are in an `mp3` folder and named `0001.mp3`, `0002.mp3`, etc.
- Try a different speaker
- Check the DFPlayer is getting power (5V recommended for reliable operation)
- Open the Serial Monitor — the game prints `[DFPlayer] OK!` or a wiring error on startup

### No sound from ISD1820
- Make sure you have recorded a sound first (hold REC button, speak, release)
- Check you are connected to the **P-E** pin, not P-L
- Test the sound by pressing the PLAY button on the module itself
- Check `ISD1820_ENABLED 1` is set in `config.h`
- Verify the GPIO pin number matches `ISD1820_PLAY_PIN` in `config.h`

### Code won't upload
- Try pressing the RESET button on the T-Display
- Make sure the USB cable supports data (some only charge)
- Try a different USB port

### Game runs but no sound
- Open the Serial Monitor — startup messages show which module is active and whether it initialised successfully

## Want to Learn More?

- Change the ball to a different image! (Look at `kim-jong-un.h`)
- Add your own background image! (Look at `background.h`)
- Create new events for different game situations
- Adjust the ball rotation speed in `main.cpp` (`BALL_ROTATION_SPEED`)

## Features

- 🎨 Custom ball sprite with rolling animation
- 🖼️ Background image support
- 🎮 Onboard + external button control
- 🔊 DFPlayer Mini sound effects (multiple tracks, SD card)
- 🔊 ISD1820 voice module support (record your own sound, no SD card needed)
- 📈 Increasing difficulty as you score



---

**Have fun learning electronics! 🚀**
