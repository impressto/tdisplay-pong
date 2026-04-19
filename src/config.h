#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// Pong Game Configuration
// ===========================================

// --- Game Mode ---
#define GAME_MODE_1P       0    // Human vs AI
#define GAME_MODE_2P       1    // Human vs Human
#define GAME_MODE_DEMO     2    // AI vs AI (Demo mode)

// --- Display Settings (Landscape orientation) ---
#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  135

// --- Menu Navigation Buttons ---
// These buttons are used to navigate the menu and select options
// The 32-button matrix can be mapped here for menu navigation
#define MENU_UP_BUTTON      35   // Navigate up in menu
#define MENU_DOWN_BUTTON    0    // Navigate down in menu  
#define MENU_SELECT_BUTTON  35   // Select menu option (same as UP for simplicity)

// --- Player 1 Controls (Left paddle) ---
#define P1_UP_BUTTON       0    // GPIO 0 - Player 1 paddle up
#define P1_DOWN_BUTTON     35   // GPIO 35 - Player 1 paddle down

// --- Player 2 Controls (Right paddle - for 2P mode) ---
#define P2_UP_BUTTON       -1   // Player 2 paddle up (set to GPIO or -1 to disable)
#define P2_DOWN_BUTTON     -1   // Player 2 paddle down

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
#define EXT_P1_UP         13   // External button - Player 1 paddle up
#define EXT_P1_DOWN       25   // External button - Player 1 paddle down
#define EXT_P2_UP         -1   // External button - Player 2 paddle up
#define EXT_P2_DOWN       -1   // External button - Player 2 paddle down

// --- Paddle Settings ---
#define PADDLE_WIDTH   30   // Length of paddle (vertical)
#define PADDLE_HEIGHT  4    // Thickness of paddle (horizontal)
#define PADDLE_SPEED   3    // Pixels to move per frame when button held
#define PADDLE_OFFSET  10   // Pixels from edge of screen

// --- Ball Settings ---
#define BALL_RADIUS        6     // Ball radius in pixels (circle)

// --- Difficulty Levels ---
// Ball speed for each difficulty (1=fastest, 5=slowest)
// Lower number = faster ball = harder game!
#define DIFFICULTY_EASY    3     // Slow ball - good for beginners!
#define DIFFICULTY_MEDIUM  2     // Medium speed - a fun challenge!
#define DIFFICULTY_HARD    1     // Fast ball - for experts only!

// --- Ball Movement ---
#define BALL_MAX_DY        4     // Maximum vertical speed (higher = steeper angles)
#define BALL_START_DX      2     // Starting horizontal speed

// --- AI Settings ---
#define AI_SPEED           2     // AI paddle movement speed
#define AI_REACTION_DELAY  3     // Frames before AI reacts (higher = easier)
#define AI_ERROR_MARGIN    10    // Random error in AI positioning (higher = easier)

// --- Net Settings ---
#define NET_DASH_LENGTH    8     // Length of each dash
#define NET_DASH_GAP       6     // Gap between dashes
#define NET_WIDTH          2     // Width of net line
#define NET_COLOR          0xFFFF // White (RGB565)

// --- Game Speed ---
#define GAME_DELAY     5    // Delay between frames (ms) - lower = faster

// --- Background ---
#define USE_BACKGROUND    1    // 1 = use background image, 0 = solid color

// ===========================================
// Colors (RGB565 format)
// ===========================================
// Pick your favorite colors! Here are some options:
//
//   COLOR NAME     VALUE      WHAT IT LOOKS LIKE
//   ----------     ------     ------------------
//   Black          0x0000     ■ (like space!)
//   White          0xFFFF     □ (like snow!)
//   Red            0xF800     ■ (like an apple!)
//   Green          0x07E0     ■ (like grass!)
//   Blue           0x001F     ■ (like the sky!)
//   Yellow         0xFFE0     ■ (like the sun!)
//   Cyan           0x07FF     ■ (like pool water!)
//   Magenta        0xF81F     ■ (like a flower!)
//   Orange         0xFD20     ■ (like an orange!)
//   Purple         0x780F     ■ (like grapes!)
//   Pink           0xFE19     ■ (like bubblegum!)
//   Light Blue     0x867D     ■ (like a cloudy sky!)
//   Lime           0x87E0     ■ (like a lime!)
//
//   DARKER COLORS:
//   Dark Red       0x8000     ■ (like a cherry!)
//   Dark Green     0x03E0     ■ (like a forest!)
//   Dark Blue      0x0010     ■ (like the deep ocean!)
//   Dark Orange    0xC280     ■ (like rust!)
//   Dark Purple    0x4008     ■ (like a plum!)
//   Dark Cyan      0x0410     ■ (like dark water!)
//   Dark Yellow    0x8400     ■ (like mustard!)
//   Brown          0x8200     ■ (like chocolate!)
//   Maroon         0x8000     ■ (like a brick!)
//   Navy           0x0010     ■ (like a sailor's uniform!)
//   Gray           0x8410     ■ (like a rainy cloud!)
//   Dark Gray      0x4208     ■ (like a shadow!)
//
// TRY IT: Change the color values below and upload
//         to see your paddle or ball change color!
// ===========================================

#define COLOR_BG           0x0000  // Black (used when USE_BACKGROUND=0)
#define COLOR_MENU_BG      0x0000  // Menu background color
#define COLOR_MENU_TEXT    0xFFFF  // Menu text color
#define COLOR_MENU_SELECT  0xFFE0  // Menu selected item color (yellow)

// ===========================================
// Theme Colors
// ===========================================
// Each theme has its own paddle and ball colors
// to make sure they're visible against the background!
//
// Theme 0: Default (background1)
// Theme 1: Spaceport (background2)
// Theme 2: Minecraft (background3)
// ===========================================

// --- Default Theme Colors ---
#define THEME0_BALL        0xF800  // Red ball
#define THEME0_PADDLE_P1   0x07E0  // Green paddle
#define THEME0_PADDLE_P2   0x001F  // Blue paddle

// --- Spaceport Theme Colors ---
#define THEME1_BALL        0xFFE0  // Yellow ball (visible in space!)
#define THEME1_PADDLE_P1   0x07FF  // Cyan paddle
#define THEME1_PADDLE_P2   0xFE19  // Pink paddle

// --- Minecraft Theme Colors ---
#define THEME2_BALL        0xFFFF  // White ball
#define THEME2_PADDLE_P1   0xFD20  // Orange paddle
#define THEME2_PADDLE_P2   0xF81F  // Magenta paddle

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
// Put MP3 files on SD card in /mp3/ folder as: 0001.mp3, 0002.mp3, 0003.mp3
// (Using /mp3/ folder is faster than root folder!)
//
// Wiring Guide:
// - DFPlayer RX -> ESP32 TX pin (through 1K resistor recommended)
// - DFPlayer TX -> ESP32 RX pin
// - DFPlayer BUSY -> ESP32 GPIO (reads LOW when playing)
// - DFPlayer VCC -> 5V
// - DFPlayer GND -> GND
// - Speaker connected to DFPlayer SPK_1 and SPK_2
//
// Tips for low latency:
// - Use short sound files (< 1 second)
// - Use low bitrate (64-128 kbps)
// - Use fast SD card (Class 10+)
//
// Set DFPLAYER_ENABLED to 0 to disable
// ===========================================
#define DFPLAYER_ENABLED   0     // 1 = enable DFPlayer, 0 = disable (using buzzer)
#define DFPLAYER_TX_PIN    17    // ESP32 TX -> DFPlayer RX
#define DFPLAYER_RX_PIN    21    // ESP32 RX <- DFPlayer TX
#define DFPLAYER_BUSY_PIN  22    // ESP32 <- DFPlayer BUSY (LOW = playing)
#define DFPLAYER_VOLUME    10    // Volume 0-30

// Track assignments (file numbers on SD card, 0 = disabled)
#define TRACK_AWARD        1     // 0001.mp3 - played when player hits a score milestone
#define TRACK_PADDLE_HIT   2     // 0002.mp3 - played on paddle hit & regular score gained
#define TRACK_SCORE_LOST   3     // 0003.mp3 - played on score lost

// Milestone: play TRACK_AWARD instead of TRACK_PADDLE_HIT every X points
#define SCORE_MILESTONE_INTERVAL  5    // Award sound plays at score 5, 10, 15, 20 ...

// Sound playback behavior
#define DFPLAYER_WAIT_FOR_FINISH  1    // 1 = wait for track to finish, 0 = allow interrupts

// ===========================================
// ISD1820 Voice Record Playback Module
// ===========================================
// A simpler alternative to the DFPlayer Mini.
// The ISD1820 stores ONE recorded sound and plays
// it back when triggered — no SD card needed!
//
// How to record your sound:
// 1. Hold the REC button on the module and speak
//    (or play a sound into the microphone)
// 2. Release REC when done — the sound is saved
//    permanently even when power is removed
//
// Wiring Guide:
// - ISD1820 VCC  -> 3.3V or 5V
// - ISD1820 GND  -> GND
// - ISD1820 P-E  -> ESP32 GPIO pin below
//   (P-E = Play Edge-triggered: brief HIGH pulse plays the sound once)
// - ISD1820 SP+  -> Speaker (+)
// - ISD1820 SP-  -> Speaker (-)
//
// Choose which game events trigger the sound:
//   ISD1820_TRIGGER_PADDLE_HIT  1 = play when ball hits paddle
//   ISD1820_TRIGGER_SCORE_LOST  1 = play when player misses ball
//
// NOTE: Only ONE sound module can be active at a time.
//       Set DFPLAYER_ENABLED 0 when using ISD1820.
// ===========================================

// Safety check — compiler will error if both modules are enabled
#if DFPLAYER_ENABLED && defined(ISD1820_ENABLED) && ISD1820_ENABLED
  #error "Only one sound module can be enabled at a time. Set DFPLAYER_ENABLED or ISD1820_ENABLED to 0."
#endif

#define ISD1820_ENABLED             0    // 1 = enable ISD1820, 0 = disable
#define ISD1820_PLAY_PIN            15   // ESP32 GPIO -> ISD1820 P-E (play edge) pin
#define ISD1820_PULSE_MS            120  // Pulse duration (ms) — long enough to trigger reliably

// Choose which events trigger the ISD1820 (1 = yes, 0 = no)
#define ISD1820_TRIGGER_PADDLE_HIT  1    // Play sound on paddle hit
#define ISD1820_TRIGGER_SCORE_LOST  1    // Play sound when player misses the ball

// ===========================================
// Passive Buzzer (9042) Configuration
// ===========================================
// A simple passive buzzer that uses PWM to generate tones.
// The 9042 is a popular passive buzzer for ESP32 projects.
//
// Wiring Guide:
// - Buzzer (+) -> ESP32 GPIO pin
// - Buzzer (-) -> GND
//
// NOTE: Only ONE sound module should be active at a time!
//       Set DFPLAYER_ENABLED and ISD1820_ENABLED to 0 when using buzzer.
// ===========================================

#define BUZZER_ENABLED         1     // 1 = enable passive buzzer, 0 = disable
#define BUZZER_PIN             15    // GPIO pin connected to buzzer (+)
#define BUZZER_CHANNEL         0     // LEDC channel for PWM (0-15)

// Tone frequencies (Hz) for game events
#define BUZZER_FREQ_WALL       800   // Wall bounce - short beep
#define BUZZER_FREQ_PADDLE     1200  // Paddle hit - higher ping
#define BUZZER_FREQ_SCORE      1500  // Score gained - victory tone
#define BUZZER_FREQ_LOST       300   // Score lost - low sad tone

// Tone durations (ms)
#define BUZZER_DUR_WALL        30    // Short click
#define BUZZER_DUR_PADDLE      50    // Quick ping
#define BUZZER_DUR_SCORE       100   // Slightly longer
#define BUZZER_DUR_LOST        200   // Longer sad sound

// Startup melody selection (0-5):
//   0 = C Major arpeggio (simple)
//   1 = "Charge!" fanfare (sports cheer)
//   2 = "Shave and a Haircut" 
//   3 = Westminster Chimes (Big Ben)
//   4 = Retro "Power Up" scale
//   5 = Classic arcade "Ready!" jingle
#define STARTUP_MELODY         3     // Try different values 0-5!

#endif
