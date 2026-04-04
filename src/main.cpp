#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

#if USE_BALL_SPRITE
#include "kim-jong-un.h"
#endif

#if USE_BACKGROUND
#include "background.h"
#endif

// Squash/Pong game for TTGO T-Display
// Original by kickiss2: https://github.com/kickiss2/TTGO_games
// Controls: Left button = paddle down, Right button = paddle up

TFT_eSPI tft = TFT_eSPI();

#if USE_BALL_SPRITE
TFT_eSprite ballSprite = TFT_eSprite(&tft);
#define TRANSPARENT_COLOR 0x0000
#endif

int16_t h = SCREEN_HEIGHT;
int16_t w = SCREEN_WIDTH;

int16_t pady = 0;
int16_t padh = PADDLE_WIDTH;

int dly = GAME_DELAY;
int16_t s = BALL_SIZE;

int16_t x = 0;
int16_t y = 0;
int16_t dx = 1;
int16_t dy = 1;

int16_t ballSpeed = BALL_SPEED_START;  // Current ball speed (frame skip counter)
int16_t ballFrame = 0;                  // Frame counter for ball movement

int16_t score = 0;
int16_t lastScore = -1;

// Redraw a rectangle from the background image
void clearWithBackground(int16_t rx, int16_t ry, int16_t rw, int16_t rh) {
#if USE_BACKGROUND
  if (rx < 0) { rw += rx; rx = 0; }
  if (ry < 0) { rh += ry; ry = 0; }
  if (rx + rw > w) rw = w - rx;
  if (ry + rh > h) rh = h - ry;
  if (rw <= 0 || rh <= 0) return;
  
  tft.setSwapBytes(true);
  for (int16_t row = 0; row < rh; row++) {
    int32_t offset = (ry + row) * SCREEN_WIDTH + rx;
    tft.pushImage(rx, ry + row, rw, 1, &background[offset]);
  }
  tft.setSwapBytes(false);
#else
  tft.fillRect(rx, ry, rw, rh, COLOR_BG);
#endif
}

void drawScore() {
  if (score != lastScore) {
    // Clear previous score area (at left edge, rotated text area)
    clearWithBackground(0, 0, SCORE_AREA_WIDTH, 100);
    
    // Temporarily switch to portrait rotation for text
    tft.setRotation(0);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(5, 5);
    tft.printf("Score:%d", score);
    tft.setRotation(1);  // Back to landscape for game
    
    lastScore = score;
  }
}

void resetBall() {
  x = SCORE_AREA_WIDTH;  // Start after score area
  y = random(0, h - s);
  dx = 1;
  // Random starting angle between -MAX_DY and +MAX_DY, but not zero
  dy = random(1, BALL_MAX_DY + 1);
  if (random(0, 2) == 0) dy = -dy;
  ballSpeed = BALL_SPEED_START;
  ballFrame = 0;
}

void ball() {
  // Ball went past paddle - reset at far left
  if (x > w + s) {
    // Player missed - deduct a point (minimum 0)
    if (score > 0) score--;
    resetBall();
  }

  // Only move ball every 'ballSpeed' frames (lower = faster)
  ballFrame++;
  if (ballFrame < ballSpeed) {
    return;
  }
  ballFrame = 0;

  clearWithBackground(x, y, s, s);
  x = x + dx;
  y = y + dy;
  
#if USE_BALL_SPRITE
  ballSprite.pushSprite(x, y, TRANSPARENT_COLOR);
#else
  tft.fillRect(x, y, s, s, COLOR_BALL);
#endif

  // Collision box coordinates (relative to screen)
  int16_t cx = x + BALL_COLLISION_X;
  int16_t cy = y + BALL_COLLISION_Y;
  int16_t cw = BALL_COLLISION_W;
  int16_t ch = BALL_COLLISION_H;

  // Top/bottom wall collision
  if (cy + ch >= h) {
    y = h - BALL_COLLISION_Y - ch;
    dy = -dy;
  }
  if (cy <= 0) {
    y = -BALL_COLLISION_Y;
    dy = -dy;
  }

  // Left wall collision (respect score area)
  if (cx <= SCORE_AREA_WIDTH) {
    x = SCORE_AREA_WIDTH - BALL_COLLISION_X;
    dx = -dx;
  }

  // Paddle collision - only count hits on the front face (ball moving towards paddle)
  // Ball center must be within paddle range (no side clips)
  if (dx > 0 && cx + cw >= w - PADDLE_OFFSET - PADDLE_HEIGHT) {
    int ballCenter = cy + ch / 2;
    // Only score if ball center is within paddle's vertical range
    if (ballCenter >= pady && ballCenter <= pady + padh) {
      x = w - PADDLE_OFFSET - PADDLE_HEIGHT - BALL_COLLISION_X - cw;  // Push ball back
      dx = -dx;
      // Increase ball speed on paddle hit
      if (ballSpeed > BALL_SPEED_MIN) {
        ballSpeed -= BALL_SPEED_INC;
      }
      score++;
      
      // Add spin based on where ball center hits paddle
      int paddleCenter = pady + padh / 2;
      if (ballCenter > paddleCenter) {
        dy = random(1, BALL_MAX_DY + 1);  // Hit bottom half - bounce down
      } else {
        dy = -random(1, BALL_MAX_DY + 1); // Hit top half - bounce up
      }
    }
  }
}

void paddle() {
  // Clear wider area to ensure old paddle pixels are removed
  clearWithBackground(w - PADDLE_OFFSET - 6, pady, 6, padh);
  
  // Move paddle only while button is pressed (no inertia)
  if (digitalRead(LEFT_BUTTON) == 0 && pady + padh < h) {
    pady += PADDLE_SPEED;
  }
  if (digitalRead(RIGHT_BUTTON) == 0 && pady > 0) {
    pady -= PADDLE_SPEED;
  }
  
  // Clamp paddle to screen bounds
  if (pady < 0) pady = 0;
  if (pady + padh > h) pady = h - padh;
  
  tft.fillRect(w - PADDLE_OFFSET - PADDLE_HEIGHT, pady, PADDLE_HEIGHT, padh, COLOR_PADDLE);
}

void setup() {
  // Turn on backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);  // Clear entire display first
  
#if USE_BACKGROUND
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, background);
  tft.setSwapBytes(false);
#else
  tft.fillScreen(COLOR_BG);
#endif
  tft.setTextColor(COLOR_PADDLE, COLOR_BG);
  
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  
#if USE_BALL_SPRITE
  ballSprite.createSprite(BALL_SIZE, BALL_SIZE);
  ballSprite.setSwapBytes(true);
  ballSprite.pushImage(0, 0, BALL_SIZE, BALL_SIZE, forever_alone);
#endif
  
  resetBall();
}

void loop() {
  delay(dly);
  paddle();
  ball();
  drawScore();
}