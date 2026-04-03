#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// Squash/Pong Game Configuration
// ===========================================

// --- Display Settings ---
#define SCREEN_WIDTH   235
#define SCREEN_HEIGHT  130

// --- Button Pins ---
#define LEFT_BUTTON    0    // GPIO 0 - moves paddle down
#define RIGHT_BUTTON   35   // GPIO 35 - moves paddle up

// --- Paddle Settings ---
#define PADDLE_HEIGHT  32   // Height in pixels (matched to ball size)
#define PADDLE_WIDTH   4    // Width in pixels
#define PADDLE_INERTIA 130  // Higher = more glide after button release

// --- Ball Settings ---
#define BALL_SIZE      32   // Ball size in pixels (matches sprite)
#define BALL_SPEED_START  3    // Initial delay between ball moves (higher = slower)
#define BALL_SPEED_INC    1    // Speed increase per paddle hit (decrease delay)
#define BALL_SPEED_MIN    1    // Minimum delay (max speed)
#define USE_BALL_SPRITE   1    // 1 = use sprite image, 0 = use simple square

// --- Game Speed ---
#define GAME_DELAY     5    // Delay between frames (ms) - lower = faster

// --- Background ---
#define USE_BACKGROUND    1    // 1 = use background image, 0 = solid color

// --- Colors (RGB565 format) ---
#define COLOR_BG       0x0000  // Black (used when USE_BACKGROUND=0)
#define COLOR_BALL     0xFFFF  // White
#define COLOR_PADDLE   0xFFFF  // White

#endif
