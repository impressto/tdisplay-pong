#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// Squash/Pong Game Configuration
// ===========================================

// --- Display Settings ---
#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  135
#define SCORE_AREA_WIDTH 30  // Left margin reserved for score display

// --- Button Pins (Onboard) ---
#define LEFT_BUTTON    0    // GPIO 0 - moves paddle left
#define RIGHT_BUTTON   35   // GPIO 35 - moves paddle right

// ===========================================
// External Button Pins - For Student Projects!
// ===========================================
// Connect external buttons for a better gaming experience!
// Buttons should connect the pin to GND when pressed.
//
// Wiring Guide:
// - Button one leg -> GPIO pin
// - Button other leg -> GND
// (Internal pull-up resistors are enabled, no external resistor needed)
//
// Set to -1 to disable external buttons
// ===========================================
#define EXT_BUTTON_DOWN   25   // External button - moves paddle left
#define EXT_BUTTON_UP     13   // External button - moves paddle right

// --- Paddle Settings (as seen in portrait orientation) ---
#define PADDLE_WIDTH   15   // Visual width of paddle (horizontal in portrait view)
#define PADDLE_HEIGHT  4    // Visual height/thickness of paddle
#define PADDLE_SPEED   2    // Pixels to move per frame when button held
#define PADDLE_OFFSET  5    // Pixels from right edge of screen

// --- Ball Settings ---
#define BALL_SIZE      32   // Ball size in pixels (matches sprite)
#define BALL_SPEED_START  3    // Initial delay between ball moves (higher = slower)
#define BALL_SPEED_INC    1    // Speed increase per paddle hit (decrease delay)
#define BALL_SPEED_MIN    1    // Minimum delay (max speed)
#define USE_BALL_SPRITE   1    // 1 = use sprite image, 0 = use simple square

// --- Ball Collision Box (relative to ball x,y position) ---
#define BALL_COLLISION_X      8    // X offset of collision box within sprite
#define BALL_COLLISION_Y      8    // Y offset of collision box within sprite
#define BALL_COLLISION_W      16   // Width of collision box
#define BALL_COLLISION_H      16   // Height of collision box

// --- Ball Movement ---
#define BALL_MAX_DY           3    // Maximum vertical speed (higher = steeper angles)

// --- Game Speed ---
#define GAME_DELAY     5    // Delay between frames (ms) - lower = faster

// --- Background ---
#define USE_BACKGROUND    1    // 1 = use background image, 0 = solid color

// --- Colors (RGB565 format) ---
#define COLOR_BG       0x0000  // Black (used when USE_BACKGROUND=0)
#define COLOR_BALL     0xFFFF  // White
#define COLOR_PADDLE   0xF800  // Red

// ===========================================
// Event Hook Pins - For Teaching Electronics!
// ===========================================
// These pins trigger when game events happen.
// Connect LEDs, buzzers, or motors to learn
// how software controls hardware!
//
// Wiring Guide:
// - LED: Pin -> 220Ω resistor -> LED (+) -> GND
// - Buzzer: Pin -> Buzzer (+) -> GND
// - Motor: Pin -> Transistor base (with resistor)
//
// Set to -1 to disable a pin
// ===========================================

#define PIN_WALL_BOUNCE    26   // Fires when ball bounces off any wall
#define PIN_PADDLE_HIT     27   // Fires when ball hits the paddle
#define PIN_SCORE_GAINED   32   // Fires when player scores a point
#define PIN_SCORE_LOST     33   // Fires when player misses the ball

// --- Event Pulse Duration ---
#define EVENT_PULSE_MS     50   // How long pins stay HIGH (milliseconds)

// ===========================================
// DFPlayer Mini MP3 Module Configuration
// ===========================================
// Connect a DFPlayer Mini to play sounds on events!
// Put MP3 files on SD card as: 0001.mp3, 0002.mp3, 0003.mp3
//
// Wiring Guide:
// - DFPlayer RX -> ESP32 TX pin (through 1K resistor recommended)
// - DFPlayer TX -> ESP32 RX pin
// - DFPlayer VCC -> 5V
// - DFPlayer GND -> GND
// - Speaker connected to DFPlayer SPK_1 and SPK_2
//
// Set DFPLAYER_ENABLED to 0 to disable
// ===========================================
#define DFPLAYER_ENABLED   1     // 1 = enable DFPlayer, 0 = disable
#define DFPLAYER_TX_PIN    17    // ESP32 TX -> DFPlayer RX
#define DFPLAYER_RX_PIN    21    // ESP32 RX <- DFPlayer TX
#define DFPLAYER_VOLUME    20    // Volume 0-30

// Track assignments (file numbers on SD card)
#define TRACK_WALL_BOUNCE  1     // 0001.mp3 - played on wall bounce
#define TRACK_PADDLE_HIT   2     // 0002.mp3 - played on paddle hit & score gained
#define TRACK_SCORE_LOST   3     // 0003.mp3 - played on score lost

#endif
