# 🎮 ESP32 Pong Game - Learn Electronics!

A fun pong game for the TTGO T-Display with sound effects powered by a DFPlayer Mini MP3 module!


<img src="https://github.com/impressto/tdisplay-pong/blob/master/esp-pong.jpg" />


## What You'll Learn

- How to upload code to an ESP32
- How to wire and use a DFPlayer Mini for sound
- How software controls hardware through GPIO pins
- Serial communication between devices

## What You Need

### Hardware
- **TTGO T-Display** (ESP32 with built-in screen)
- **USB-C cable** to connect to your computer
- **DFPlayer Mini** MP3 module for sound effects
- **Micro SD card** with MP3 files for game sounds
- **Small speaker** (3W 8ohm recommended)
- **Jumper wires** to connect things

### Software
- [VS Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)

## How to Play

### Onboard Buttons
1. **Left Button (GPIO 0)** - Move paddle LEFT
2. **Right Button (GPIO 35)** - Move paddle RIGHT

### External Buttons (Optional)
You can add your own buttons for a better gaming experience!
- **GPIO 25** - External button to move paddle LEFT
- **GPIO 13** - External button to move paddle RIGHT

**Wiring external buttons:**
```
Button leg 1 -----> GPIO pin (25 or 13)
Button leg 2 -----> GND
```
No resistor needed - the ESP32 has internal pull-up resistors!

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

## Wiring Up Electronics!

The game triggers events when things happen in the game. These events play sounds through the DFPlayer!

### DFPlayer Mini Wiring

Connect the DFPlayer Mini to your T-Display:

```
T-Display          DFPlayer Mini
---------          -------------
GPIO 17 (TX) ----> RX
GPIO 21 (RX) ----> TX
GND          ----> GND
3.3V/5V      ----> VCC
                   SPK1 ----> Speaker (+)
                   SPK2 ----> Speaker (-)
```

**Note:** The DFPlayer can run on 3.3V or 5V. Use 5V for louder sound output.

### Setting Up Sound Files

1. Format a micro SD card as FAT32
2. Create a folder named `mp3` in the root
3. Name your sound files with numbers: `0001.mp3`, `0002.mp3`, etc.
4. Insert the SD card into the DFPlayer

**Recommended sound files:**
| File | Event |
|------|-------|
| `0001.mp3` | Wall bounce sound |
| `0002.mp3` | Paddle hit sound |
| `0003.mp3` | Score gained sound |
| `0004.mp3` | Score lost sound |

### Game Events

| Event | When Does It Trigger? |
|-------|----------------------|
| Wall Bounce | Ball bounces off any wall |
| Paddle Hit | Ball hits your paddle |
| Score Gained | You score a point! |
| Score Lost | You miss the ball |

## Customizing the Game

Open `src/config.h` to change settings:

```c
// DFPlayer pins
#define DFPLAYER_TX_PIN    17   // TX to DFPlayer RX
#define DFPLAYER_RX_PIN    21   // RX from DFPlayer TX

// Sound file numbers (on SD card)
#define SOUND_WALL_BOUNCE  1    // Plays 0001.mp3
#define SOUND_PADDLE_HIT   2    // Plays 0002.mp3
#define SOUND_SCORE_GAINED 3    // Plays 0003.mp3
#define SOUND_SCORE_LOST   4    // Plays 0004.mp3
```

### Other Fun Settings to Try

```c
#define PADDLE_WIDTH   15    // Make paddle bigger = easier game
#define BALL_SPEED_START 3   // Higher number = slower ball
#define PADDLE_SPEED   2     // How fast paddle moves

// External button pins (set to -1 to disable)
#define EXT_BUTTON_DOWN  25  // External button - paddle left
#define EXT_BUTTON_UP    13  // External button - paddle right
```

## Adding Your Own Event Code! 🛠️

Want to customize what happens when game events occur? You can add your own custom code!

### Where to Put Your Code

Open `src/events.cpp` - this is where all the event magic happens!

**The file has 4 event functions you can modify:**

```c
void onWallBounce() {
  // This runs when the ball hits a wall
  // Add your code here!
}

void onPaddleHit() {
  // This runs when you hit the ball
  // Add your code here!
}

void onScoreGained() {
  // This runs when you score a point
  // Add your code here!
}

void onScoreLost() {
  // This runs when you miss the ball
  // Add your code here!
}
```

### Example: Play Different Sounds

You can adjust volume or play different tracks based on game state:

```c
void onScoreGained() {
  // Play a victory sound at higher volume!
  dfPlayer.volume(25);
  dfPlayer.play(SOUND_SCORE_GAINED);
}

void onScoreLost() {
  // Play a sad sound 
  dfPlayer.play(SOUND_SCORE_LOST);
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

| Pin | Usage | Notes |
|-----|-------|-------|
| 0 | Left button | Onboard button |
| 13 | External button | Default: External LEFT button |
| 17 | DFPlayer TX | Serial TX to DFPlayer RX |
| 21 | DFPlayer RX | Serial RX from DFPlayer TX |
| 25 | External button | Default: External RIGHT button |
| 35 | Right button | Onboard button (input only) |

## Troubleshooting

### No sound from DFPlayer
- Check wiring: TX to RX, RX to TX (they cross over!)
- Make sure the SD card is formatted as FAT32
- Verify sound files are in an `mp3` folder and named `0001.mp3`, `0002.mp3`, etc.
- Try a different speaker
- Check the DFPlayer is getting power (3.3V or 5V)

### Code won't upload
- Try pressing the RESET button on the T-Display
- Make sure the USB cable supports data (some only charge)
- Try a different USB port

### Game runs but no sound on events
- Check your wiring connections
- Verify the SD card is inserted properly
- Try playing a sound manually to test the DFPlayer

## Want to Learn More?

- Change the ball to a different image! (Look at `kim-jong-un.h`)
- Add your own background image! (Look at `background.h`)
- Create new events for different game situations
- Adjust the ball rotation speed in `main.cpp` (`BALL_ROTATION_SPEED`)

## Features

- 🎨 Custom ball sprite with rolling animation
- 🖼️ Background image support
- 🎮 Onboard + external button control
- 🔊 DFPlayer Mini sound effects for game events
- 📈 Increasing difficulty as you score



---

**Have fun learning electronics! 🚀**
