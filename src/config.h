#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// Squash/Pong Game Configuration
// ===========================================

// --- Display Settings ---
#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  135
#define SCORE_AREA_WIDTH 30  // Left margin reserved for score display

// --- Button Pins ---
#define LEFT_BUTTON    0    // GPIO 0 - moves paddle down
#define RIGHT_BUTTON   35   // GPIO 35 - moves paddle up

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

#endif
