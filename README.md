# 🎮 ESP32 Pong Game - Learn Electronics!

A fun pong game for the TTGO T-Display that teaches you how to wire up LEDs, buzzers, and motors that react to what happens in the game!

## What You'll Learn

- How to upload code to an ESP32
- How to wire LEDs, buzzers, and motors
- How software controls hardware through GPIO pins
- Basic electronics with resistors and circuits

## What You Need

### Hardware
- **TTGO T-Display** (ESP32 with built-in screen)
- **USB-C cable** to connect to your computer
- **Breadboard** for building circuits
- **Jumper wires** to connect things

### Optional Components (pick what you have!)
- LEDs (any color)
- 220Ω resistors (for LEDs)
- Piezo buzzer
- Small vibration motor
- NPN transistor (like 2N2222) for motors

### Software
- [VS Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)

## How to Play

1. **Left Button (GPIO 0)** - Move paddle DOWN
2. **Right Button (GPIO 35)** - Move paddle UP
3. Hit the ball with your paddle to score points!
4. If you miss, you lose a point

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

The game triggers GPIO pins when events happen. You can connect things to these pins!

### Event Pins

| Event | GPIO Pin | When Does It Trigger? |
|-------|----------|----------------------|
| Wall Bounce | **26** | Ball bounces off any wall |
| Paddle Hit | **27** | Ball hits your paddle |
| Score Gained | **32** | You score a point! |
| Score Lost | **33** | You miss the ball |

### Project 1: LED Light Show 💡

Make an LED light up when you hit the ball!

**What you need:**
- 1 LED
- 1 resistor (220Ω - has red-red-brown stripes)
- 2 jumper wires

**Wiring:**
```
GPIO 27 -----> 220Ω Resistor -----> LED (long leg +) -----> LED (short leg -) -----> GND
```

**Step by step:**
1. Connect a jumper wire from **GPIO 27** to one end of the resistor
2. Connect the other end of the resistor to the **long leg** of the LED (that's the + side)
3. Connect the **short leg** of the LED to **GND** on the T-Display

Now when you hit the ball, the LED lights up!

### Project 2: Multi-Color Light Show 🌈

Use 4 different colored LEDs for each event!

**Wiring:**
```
GPIO 26 (Wall)   -----> 220Ω -----> Blue LED   -----> GND
GPIO 27 (Paddle) -----> 220Ω -----> Green LED  -----> GND  
GPIO 32 (Score+) -----> 220Ω -----> Yellow LED -----> GND
GPIO 33 (Score-) -----> 220Ω -----> Red LED    -----> GND
```

### Project 3: Sound Effects 🔊

Add a buzzer that beeps when you score!

**What you need:**
- 1 piezo buzzer

**Wiring:**
```
GPIO 32 -----> Buzzer (+) 
GND     -----> Buzzer (-)
```

**Note:** Most small piezo buzzers can connect directly. If yours doesn't make sound, you might need a transistor circuit (ask an adult!).

### Project 4: Vibration Motor 📳

Make something shake when you miss!

**What you need:**
- 1 small vibration motor
- 1 NPN transistor (2N2222 or similar)
- 1 resistor (1kΩ)
- 1 diode (1N4001 - optional but recommended)

**Wiring (with transistor):**
```
GPIO 33 -----> 1kΩ Resistor -----> Transistor Base (middle pin)
3.3V    -----> Motor (+) -----> Motor (-) -----> Transistor Collector (right pin)
GND     -----> Transistor Emitter (left pin)
```

⚠️ **Important:** Motors need more power than an LED. The transistor acts like a switch that the ESP32 controls!

## Customizing the Game

Open `src/config.h` to change settings:

```c
// Change which pins the events use
#define PIN_WALL_BOUNCE    26   // Change to any free GPIO
#define PIN_PADDLE_HIT     27
#define PIN_SCORE_GAINED   32
#define PIN_SCORE_LOST     33

// How long the pin stays ON (in milliseconds)
#define EVENT_PULSE_MS     50   // Try 100 for longer flashes!

// Disable a pin by setting it to -1
#define PIN_WALL_BOUNCE    -1   // This turns off wall bounce events
```

### Other Fun Settings to Try

```c
#define PADDLE_WIDTH   15    // Make paddle bigger = easier game
#define BALL_SPEED_START 3   // Higher number = slower ball
#define PADDLE_SPEED   2     // How fast paddle moves
```

## Adding Your Own Event Code! 🛠️

Want to do more than just turn a pin on and off? You can add your own custom code!

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

### Example: Play Different Tones

Instead of just turning a pin on/off, make a buzzer play different sounds:

```c
void onPaddleHit() {
  // Play a happy beep!
  tone(PIN_PADDLE_HIT, 1000, 50);  // 1000 Hz for 50ms
}

void onScoreLost() {
  // Play a sad sound
  tone(PIN_SCORE_LOST, 200, 200);  // Low 200 Hz for 200ms
}
```

### Example: Flash an LED Multiple Times

```c
void onScoreGained() {
  // Flash 3 times to celebrate!
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_SCORE_GAINED, HIGH);
    delay(50);
    digitalWrite(PIN_SCORE_GAINED, LOW);
    delay(50);
  }
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

Here are the GPIO pins you can use:

| Pin | Safe to Use? | Notes |
|-----|--------------|-------|
| 0 | ⚠️ | Used for left button |
| 2 | ✅ | |
| 12 | ✅ | |
| 13 | ✅ | |
| 15 | ✅ | |
| 17 | ✅ | |
| 21 | ✅ | |
| 22 | ✅ | |
| 25 | ✅ | |
| 26 | ✅ | Default: Wall bounce |
| 27 | ✅ | Default: Paddle hit |
| 32 | ✅ | Default: Score gained |
| 33 | ✅ | Default: Score lost |
| 35 | ⚠️ | Used for right button (input only) |

## Troubleshooting

### LED doesn't light up
- Check the LED is the right way around (long leg to +, short to -)
- Make sure you have a resistor - LEDs need them!
- Try a different GPIO pin

### Code won't upload
- Try pressing the RESET button on the T-Display
- Make sure the USB cable supports data (some only charge)
- Try a different USB port

### Game runs but nothing happens on pins
- Check your wiring connections
- Make sure you're using the right GPIO pins
- Try the `onWallBounce` event first - it happens most often!

## Want to Learn More?

- Change the ball to a different image! (Look at `kim-jong-un.h`)
- Add your own background image! (Look at `background.h`)
- Create new events for different game situations

## Credits

- Original game by [kickiss2](https://github.com/kickiss2/TTGO_games)
- Event hooks system for learning electronics

---

**Have fun learning electronics! 🚀**
