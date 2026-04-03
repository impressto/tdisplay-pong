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

int16_t ipady = 0;
int16_t pady = 0;
int16_t padh = PADDLE_HEIGHT;

int dly = GAME_DELAY;
int16_t s = BALL_SIZE;

int16_t x = 0;
int16_t y = 0;
int16_t dx = 1;
int16_t dy = 1;

int16_t ballSpeed = BALL_SPEED_START;  // Current ball speed (frame skip counter)
int16_t ballFrame = 0;                  // Frame counter for ball movement

int16_t inertia = 0;
int16_t ispeed = PADDLE_INERTIA;

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
    clearWithBackground(2, 2, 50, 12);
    tft.setTextColor(COLOR_PADDLE);
    tft.setTextSize(1);
    tft.setCursor(2, 2);
    tft.printf("Score:%d", score);
    lastScore = score;
  }
}

void resetBall() {
  x = 0;
  y = random(0, h);
  dx = 1;
  dy = random(1, 3) - 1;
  ballSpeed = BALL_SPEED_START;
  ballFrame = 0;
  score = 0;
}

void ball() {
  if (x + s > w + 200) {
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

  if (y + s == h || y == 0) {
    dy = -dy;
  }

  if (x == 0) {
    dx = -dx;
  }

  if (x + s == w) {
    if (y >= pady && y <= pady + padh) {
      dx = -dx;
      // Increase ball speed on paddle hit
      if (ballSpeed > BALL_SPEED_MIN) {
        ballSpeed -= BALL_SPEED_INC;
      }
      score++;
    }
    if (y <= pady + padh && y >= (pady + padh / 2) && dy <= 0) {
      dy = 1;
    }
    if (y >= pady && y <= (pady + padh / 2) && dy >= 0) {
      dy = -1;
    }
  }
}

void paddle() {
  clearWithBackground(w - PADDLE_WIDTH, pady, PADDLE_WIDTH, padh);
  
  if (digitalRead(LEFT_BUTTON) == 0) {
    inertia = ispeed;
  }
  if (digitalRead(RIGHT_BUTTON) == 0) {
    inertia = -ispeed;
  }
  if (digitalRead(LEFT_BUTTON) == 0 && digitalRead(RIGHT_BUTTON) == 0) {
    inertia = 0;
  }
  
  if (inertia > 0 && pady + padh < h) {
    ipady = ipady + inertia;
    pady = round(ipady / ispeed);
    inertia = inertia - 1;
  }
  if (inertia < 0 && pady > 0) {
    ipady = ipady + inertia;
    pady = round(ipady / ispeed);
    inertia = inertia + 1;
  }
  
  tft.drawRect(w - PADDLE_WIDTH, pady, PADDLE_WIDTH, padh, COLOR_PADDLE);
}

void setup() {
  // Turn on backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  tft.init();
  tft.setRotation(1);
  
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
  ballSprite.pushImage(0, 0, BALL_SIZE, BALL_SIZE, kim_jong_un2);
#endif
  
  resetBall();
}

void loop() {
  delay(dly);
  paddle();
  ball();
  drawScore();
}