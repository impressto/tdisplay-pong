#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include "config.h"
#include "events.h"

#if USE_BACKGROUND
#include "background1.h"
#include "background2.h"
#include "background3.h"
#endif

// ===========================================
// Pong Game for TTGO T-Display
// Supports 1 Player (vs AI) and 2 Player modes
// ===========================================

TFT_eSPI tft = TFT_eSPI();
Preferences preferences;

int16_t screenW = SCREEN_WIDTH;
int16_t screenH = SCREEN_HEIGHT;

// Game state
enum GameState {
  STATE_MENU,
  STATE_PLAYING,
  STATE_GAME_OVER
};

GameState gameState = STATE_MENU;
int8_t gameMode = GAME_MODE_1P;  // Current game mode
int8_t menuSelection = 0;        // Current menu selection
int8_t menuScreen = 0;           // 0 = game mode, 1 = difficulty, 2 = theme
int8_t difficulty = 1;           // 0 = Easy, 1 = Medium, 2 = Hard
int8_t theme = 0;                // 0 = Default, 1 = Spaceport, 2 = Minecraft
int16_t ballSpeed = DIFFICULTY_MEDIUM;  // Current ball speed based on difficulty

#if USE_BACKGROUND
// Pointer to active background
const uint16_t* activeBackground = background1;
#endif

// Active theme colors
uint16_t activeColorBall = THEME0_BALL;
uint16_t activeColorPaddle1 = THEME0_PADDLE_P1;
uint16_t activeColorPaddle2 = THEME0_PADDLE_P2;

// Paddle positions (Y coordinates)
int16_t paddle1Y = 0;  // Left paddle (Player 1)
int16_t paddle2Y = 0;  // Right paddle (Player 2 / AI)

// Ball state
int16_t ballX = 0;
int16_t ballY = 0;
int16_t ballDX = BALL_START_DX;
int16_t ballDY = 1;
int16_t ballFrame = 0;

// Scores
int16_t score1 = 0;  // Player 1 score
int16_t score2 = 0;  // Player 2 / AI score
int16_t lastScore1 = -1;
int16_t lastScore2 = -1;

// AI state
int16_t aiReactionCounter = 0;
int16_t aiTargetY = 0;

// Visual effects
int16_t paddle1FlashFrames = 0;
int16_t paddle2FlashFrames = 0;
#define PADDLE_FLASH_DURATION 15
#define COLOR_PADDLE_FLASH 0xFFFF

// Debounce for menu
unsigned long lastButtonPress = 0;
#define BUTTON_DEBOUNCE_MS 200

// Forward declarations
void startGame();

// ===========================================
// Settings Storage (survives power off!)
// ===========================================

void loadSettings() {
  preferences.begin("pong", true);  // Read-only mode
  gameMode = preferences.getChar("mode", GAME_MODE_1P);
  difficulty = preferences.getChar("diff", 1);  // Default: Medium
  theme = preferences.getChar("theme", 0);      // Default: Default theme
  preferences.end();
  
  // Validate loaded values
  if (gameMode < 0 || gameMode > 1) gameMode = GAME_MODE_1P;
  if (difficulty < 0 || difficulty > 2) difficulty = 1;
  if (theme < 0 || theme > 2) theme = 0;
  
  // Apply difficulty
  switch (difficulty) {
    case 0: ballSpeed = DIFFICULTY_EASY; break;
    case 1: ballSpeed = DIFFICULTY_MEDIUM; break;
    case 2: ballSpeed = DIFFICULTY_HARD; break;
  }
  
  // Apply theme background
  #if USE_BACKGROUND
  switch (theme) {
    case 0: activeBackground = background1; break;
    case 1: activeBackground = background2; break;
    case 2: activeBackground = background3; break;
  }
  #endif
  
  // Apply theme colors
  switch (theme) {
    case 0:
      activeColorBall = THEME0_BALL;
      activeColorPaddle1 = THEME0_PADDLE_P1;
      activeColorPaddle2 = THEME0_PADDLE_P2;
      break;
    case 1:
      activeColorBall = THEME1_BALL;
      activeColorPaddle1 = THEME1_PADDLE_P1;
      activeColorPaddle2 = THEME1_PADDLE_P2;
      break;
    case 2:
      activeColorBall = THEME2_BALL;
      activeColorPaddle1 = THEME2_PADDLE_P1;
      activeColorPaddle2 = THEME2_PADDLE_P2;
      break;
  }
  
  Serial.printf("[Settings] Loaded: mode=%d, diff=%d, theme=%d\n", gameMode, difficulty, theme);
}

void saveSettings() {
  preferences.begin("pong", false);  // Read-write mode
  preferences.putChar("mode", gameMode);
  preferences.putChar("diff", difficulty);
  preferences.putChar("theme", theme);
  preferences.end();
  Serial.printf("[Settings] Saved: mode=%d, diff=%d, theme=%d\n", gameMode, difficulty, theme);
}

// ===========================================
// Helper Functions
// ===========================================

// Redraw a rectangle from the background image
void clearWithBackground(int16_t rx, int16_t ry, int16_t rw, int16_t rh) {
#if USE_BACKGROUND
  if (rx < 0) { rw += rx; rx = 0; }
  if (ry < 0) { rh += ry; ry = 0; }
  if (rx + rw > screenW) rw = screenW - rx;
  if (ry + rh > screenH) rh = screenH - ry;
  if (rw <= 0 || rh <= 0) return;
  
  tft.setSwapBytes(true);
  for (int16_t row = 0; row < rh; row++) {
    int32_t offset = (ry + row) * SCREEN_WIDTH + rx;
    tft.pushImage(rx, ry + row, rw, 1, &activeBackground[offset]);
  }
  tft.setSwapBytes(false);
#else
  tft.fillRect(rx, ry, rw, rh, COLOR_BG);
#endif
}

// Check if a button is pressed (handles disabled pins)
bool isButtonPressed(int pin) {
  if (pin < 0) return false;
  return digitalRead(pin) == LOW;
}

// ===========================================
// Drawing Functions
// ===========================================

// Draw the center net (dashed line)
void drawNet() {
  int16_t centerX = screenW / 2 - NET_WIDTH / 2;
  for (int16_t y = 0; y < screenH; y += NET_DASH_LENGTH + NET_DASH_GAP) {
    int16_t dashLen = NET_DASH_LENGTH;
    if (y + dashLen > screenH) dashLen = screenH - y;
    tft.fillRect(centerX, y, NET_WIDTH, dashLen, NET_COLOR);
  }
}

// Draw the background and net
void drawBackground() {
#if USE_BACKGROUND
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, activeBackground);
  tft.setSwapBytes(false);
#else
  tft.fillScreen(COLOR_BG);
#endif
  drawNet();
}

// Draw scores at top of screen
void drawScores() {
  if (score1 != lastScore1 || score2 != lastScore2) {
    // Clear score areas
    clearWithBackground(screenW/4 - 20, 2, 40, 18);
    clearWithBackground(3*screenW/4 - 20, 2, 40, 18);
    
    // Draw scores
    tft.setTextColor(COLOR_MENU_TEXT, COLOR_BG);
    tft.setTextSize(2);
    
    // Player 1 score (left side)
    tft.setCursor(screenW/4 - 10, 5);
    tft.print(score1);
    
    // Player 2 / AI score (right side)
    tft.setCursor(3*screenW/4 - 10, 5);
    tft.print(score2);
    
    lastScore1 = score1;
    lastScore2 = score2;
  }
}

// Draw a paddle
void drawPaddle(int16_t x, int16_t y, uint16_t color) {
  tft.fillRect(x, y, PADDLE_HEIGHT, PADDLE_WIDTH, color);
}

// Draw the ball (filled circle)
void drawBall(int16_t x, int16_t y) {
  tft.fillCircle(x, y, BALL_RADIUS, activeColorBall);
}

// Clear ball area
void clearBall(int16_t x, int16_t y) {
  clearWithBackground(x - BALL_RADIUS - 1, y - BALL_RADIUS - 1, 
                      BALL_RADIUS * 2 + 2, BALL_RADIUS * 2 + 2);
}

// ===========================================
// Menu System
// ===========================================

void drawMenu() {
  tft.fillScreen(COLOR_MENU_BG);
  
  // Title
  tft.setTextColor(COLOR_MENU_TEXT);
  tft.setTextSize(3);
  tft.setCursor(70, 20);
  tft.print("PONG");
  
  tft.setTextSize(2);
  
  if (menuScreen == 0) {
    // Game Mode Selection
    // Quick Play option (uses saved settings)
    if (menuSelection == 0) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(40, 50);
      tft.print("> Quick Play");
    } else {
      tft.setTextColor(0x07FF);  // Cyan
      tft.setCursor(50, 50);
      tft.print("Quick Play");
    }
    
    // 1 Player option
    if (menuSelection == 1) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(40, 75);
      tft.print("> 1 Player");
    } else {
      tft.setTextColor(COLOR_MENU_TEXT);
      tft.setCursor(50, 75);
      tft.print("1 Player");
    }
    
    // 2 Players option
    if (menuSelection == 2) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(40, 100);
      tft.print("> 2 Players");
    } else {
      tft.setTextColor(COLOR_MENU_TEXT);
      tft.setCursor(50, 100);
      tft.print("2 Players");
    }
  } else if (menuScreen == 1) {
    // Difficulty Selection
    tft.setTextSize(1);
    tft.setTextColor(0x7BEF);
    tft.setCursor(65, 45);
    tft.print("Select Difficulty");
    tft.setTextSize(2);
    
    // Easy
    if (menuSelection == 0) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 60);
      tft.print("> Easy");
    } else {
      tft.setTextColor(0x07E0);  // Green
      tft.setCursor(60, 60);
      tft.print("Easy");
    }
    
    // Medium
    if (menuSelection == 1) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 80);
      tft.print("> Medium");
    } else {
      tft.setTextColor(0xFFE0);  // Yellow
      tft.setCursor(60, 80);
      tft.print("Medium");
    }
    
    // Hard
    if (menuSelection == 2) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 100);
      tft.print("> Hard");
    } else {
      tft.setTextColor(0xF800);  // Red
      tft.setCursor(60, 100);
      tft.print("Hard");
    }
  } else {
    // Theme Selection
    tft.setTextSize(1);
    tft.setTextColor(0x7BEF);
    tft.setCursor(70, 45);
    tft.print("Select Theme");
    tft.setTextSize(2);
    
    // Default
    if (menuSelection == 0) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 60);
      tft.print("> Default");
    } else {
      tft.setTextColor(0xBEDF);  // Light blue
      tft.setCursor(60, 60);
      tft.print("Default");
    }
    
    // Spaceport
    if (menuSelection == 1) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 80);
      tft.print("> Spaceport");
    } else {
      tft.setTextColor(0x4A69);  // Dark blue
      tft.setCursor(60, 80);
      tft.print("Spaceport");
    }
    
    // Minecraft
    if (menuSelection == 2) {
      tft.setTextColor(COLOR_MENU_SELECT);
      tft.setCursor(50, 100);
      tft.print("> Minecraft");
    } else {
      tft.setTextColor(0x07E0);  // Green
      tft.setCursor(60, 100);
      tft.print("Minecraft");
    }
  }
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(0x7BEF);  // Gray
  tft.setCursor(45, 120);
  tft.print("Press button to select");
}

void handleMenu() {
  unsigned long now = millis();
  if (now - lastButtonPress < BUTTON_DEBOUNCE_MS) return;
  
  // Navigate menu
  if (isButtonPressed(P1_UP_BUTTON) || isButtonPressed(EXT_P1_UP)) {
    if (menuScreen == 0) {
      menuSelection = (menuSelection + 1) % 3;  // 3 options: Quick Play, 1P, 2P
    } else {
      menuSelection = (menuSelection + 1) % 3;  // 3 options for difficulty/theme
    }
    lastButtonPress = now;
    drawMenu();
  }
  
  if (isButtonPressed(P1_DOWN_BUTTON) || isButtonPressed(EXT_P1_DOWN)) {
    lastButtonPress = now;
    
    if (menuScreen == 0) {
      if (menuSelection == 0) {
        // Quick Play - use saved settings and start immediately
        gameState = STATE_PLAYING;
        startGame();
      } else {
        // Game mode selected (1=1P, 2=2P), go to difficulty screen
        gameMode = menuSelection - 1;  // Adjust for Quick Play offset
        menuScreen = 1;
        menuSelection = difficulty;  // Use saved difficulty as default
        drawMenu();
      }
    } else if (menuScreen == 1) {
      // Difficulty selected, go to theme screen
      difficulty = menuSelection;
      
      // Set ball speed based on difficulty
      switch (difficulty) {
        case 0: ballSpeed = DIFFICULTY_EASY; break;
        case 1: ballSpeed = DIFFICULTY_MEDIUM; break;
        case 2: ballSpeed = DIFFICULTY_HARD; break;
      }
      
      menuScreen = 2;
      menuSelection = theme;  // Use saved theme as default
      drawMenu();
    } else {
      // Theme selected, start game
      theme = menuSelection;
      
      // Set active background based on theme
      #if USE_BACKGROUND
      switch (theme) {
        case 0: activeBackground = background1; break;
        case 1: activeBackground = background2; break;
        case 2: activeBackground = background3; break;
      }
      #endif
      
      // Set colors based on theme
      switch (theme) {
        case 0:  // Default
          activeColorBall = THEME0_BALL;
          activeColorPaddle1 = THEME0_PADDLE_P1;
          activeColorPaddle2 = THEME0_PADDLE_P2;
          break;
        case 1:  // Spaceport
          activeColorBall = THEME1_BALL;
          activeColorPaddle1 = THEME1_PADDLE_P1;
          activeColorPaddle2 = THEME1_PADDLE_P2;
          break;
        case 2:  // Minecraft
          activeColorBall = THEME2_BALL;
          activeColorPaddle1 = THEME2_PADDLE_P1;
          activeColorPaddle2 = THEME2_PADDLE_P2;
          break;
      }
      
      gameState = STATE_PLAYING;
      menuScreen = 0;  // Reset for next time
      menuSelection = 0;
      saveSettings();  // Save selections for next time
      startGame();
    }
  }
}

// ===========================================
// Game Logic
// ===========================================

void resetBall() {
  ballX = screenW / 2;
  ballY = screenH / 2;
  
  // Alternate direction based on who scored last
  ballDX = (random(2) == 0) ? BALL_START_DX : -BALL_START_DX;
  ballDY = random(-BALL_MAX_DY, BALL_MAX_DY + 1);
  if (ballDY == 0) ballDY = 1;
  
  ballFrame = 0;
}

void startGame() {
  // Reset scores
  score1 = 0;
  score2 = 0;
  lastScore1 = -1;
  lastScore2 = -1;
  
  // Center paddles
  paddle1Y = (screenH - PADDLE_WIDTH) / 2;
  paddle2Y = (screenH - PADDLE_WIDTH) / 2;
  
  // Reset AI
  aiReactionCounter = 0;
  aiTargetY = screenH / 2;
  
  // Draw initial state
  drawBackground();
  drawScores();
  
  // Draw paddles
  drawPaddle(PADDLE_OFFSET, paddle1Y, activeColorPaddle1);
  drawPaddle(screenW - PADDLE_OFFSET - PADDLE_HEIGHT, paddle2Y, activeColorPaddle2);
  
  resetBall();
}

void updatePaddle1() {
  int16_t oldY = paddle1Y;
  
  // Move paddle based on input
  if (isButtonPressed(P1_UP_BUTTON) || isButtonPressed(EXT_P1_UP)) {
    paddle1Y -= PADDLE_SPEED;
  }
  if (isButtonPressed(P1_DOWN_BUTTON) || isButtonPressed(EXT_P1_DOWN)) {
    paddle1Y += PADDLE_SPEED;
  }
  
  // Clamp to screen bounds
  if (paddle1Y < 0) paddle1Y = 0;
  if (paddle1Y + PADDLE_WIDTH > screenH) paddle1Y = screenH - PADDLE_WIDTH;
  
  // Redraw if moved
  if (oldY != paddle1Y) {
    clearWithBackground(PADDLE_OFFSET, oldY, PADDLE_HEIGHT, PADDLE_WIDTH);
    
    uint16_t color = activeColorPaddle1;
    if (paddle1FlashFrames > 0) {
      color = COLOR_PADDLE_FLASH;
      paddle1FlashFrames--;
    }
    drawPaddle(PADDLE_OFFSET, paddle1Y, color);
  } else if (paddle1FlashFrames > 0) {
    paddle1FlashFrames--;
    drawPaddle(PADDLE_OFFSET, paddle1Y, 
               paddle1FlashFrames > 0 ? COLOR_PADDLE_FLASH : activeColorPaddle1);
  }
}

void updatePaddle2() {
  int16_t oldY = paddle2Y;
  
  if (gameMode == GAME_MODE_2P) {
    // Human player 2
    if (isButtonPressed(P2_UP_BUTTON) || isButtonPressed(EXT_P2_UP)) {
      paddle2Y -= PADDLE_SPEED;
    }
    if (isButtonPressed(P2_DOWN_BUTTON) || isButtonPressed(EXT_P2_DOWN)) {
      paddle2Y += PADDLE_SPEED;
    }
  } else {
    // AI control
    aiReactionCounter++;
    if (aiReactionCounter >= AI_REACTION_DELAY) {
      aiReactionCounter = 0;
      
      // AI tracks the ball when it's moving toward the AI paddle
      if (ballDX > 0) {
        aiTargetY = ballY + random(-AI_ERROR_MARGIN, AI_ERROR_MARGIN + 1);
      }
    }
    
    // Move toward target
    int16_t paddleCenter = paddle2Y + PADDLE_WIDTH / 2;
    if (paddleCenter < aiTargetY - 2) {
      paddle2Y += AI_SPEED;
    } else if (paddleCenter > aiTargetY + 2) {
      paddle2Y -= AI_SPEED;
    }
  }
  
  // Clamp to screen bounds
  if (paddle2Y < 0) paddle2Y = 0;
  if (paddle2Y + PADDLE_WIDTH > screenH) paddle2Y = screenH - PADDLE_WIDTH;
  
  // Redraw if moved
  if (oldY != paddle2Y) {
    clearWithBackground(screenW - PADDLE_OFFSET - PADDLE_HEIGHT, oldY, 
                        PADDLE_HEIGHT, PADDLE_WIDTH);
    
    uint16_t color = activeColorPaddle2;
    if (paddle2FlashFrames > 0) {
      color = COLOR_PADDLE_FLASH;
      paddle2FlashFrames--;
    }
    drawPaddle(screenW - PADDLE_OFFSET - PADDLE_HEIGHT, paddle2Y, color);
  } else if (paddle2FlashFrames > 0) {
    paddle2FlashFrames--;
    drawPaddle(screenW - PADDLE_OFFSET - PADDLE_HEIGHT, paddle2Y,
               paddle2FlashFrames > 0 ? COLOR_PADDLE_FLASH : activeColorPaddle2);
  }
}

void updateBall() {
  // Only move ball every 'ballSpeed' frames
  ballFrame++;
  if (ballFrame < ballSpeed) return;
  ballFrame = 0;
  
  // Clear old ball position
  clearBall(ballX, ballY);
  
  // Redraw net section if ball was near center
  if (abs(ballX - screenW/2) < BALL_RADIUS + NET_WIDTH) {
    int16_t netX = screenW / 2 - NET_WIDTH / 2;
    for (int16_t y = ballY - BALL_RADIUS - 2; y < ballY + BALL_RADIUS + 2; y += NET_DASH_LENGTH + NET_DASH_GAP) {
      if (y >= 0 && y < screenH) {
        int16_t dashY = (y / (NET_DASH_LENGTH + NET_DASH_GAP)) * (NET_DASH_LENGTH + NET_DASH_GAP);
        tft.fillRect(netX, dashY, NET_WIDTH, NET_DASH_LENGTH, NET_COLOR);
      }
    }
  }
  
  // Move ball
  ballX += ballDX;
  ballY += ballDY;
  
  // Top/bottom wall collision
  if (ballY - BALL_RADIUS <= 0) {
    ballY = BALL_RADIUS;
    ballDY = -ballDY;
    onWallBounce();
  }
  if (ballY + BALL_RADIUS >= screenH) {
    ballY = screenH - BALL_RADIUS;
    ballDY = -ballDY;
    onWallBounce();
  }
  
  // Left paddle (Player 1) collision
  if (ballDX < 0 && 
      ballX - BALL_RADIUS <= PADDLE_OFFSET + PADDLE_HEIGHT &&
      ballX + BALL_RADIUS >= PADDLE_OFFSET) {
    if (ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_WIDTH) {
      ballX = PADDLE_OFFSET + PADDLE_HEIGHT + BALL_RADIUS;
      ballDX = -ballDX;
      
      // Add spin based on hit location
      int16_t hitPos = ballY - (paddle1Y + PADDLE_WIDTH / 2);
      ballDY = hitPos / 4;
      if (ballDY == 0) ballDY = (random(2) == 0) ? 1 : -1;
      if (ballDY > BALL_MAX_DY) ballDY = BALL_MAX_DY;
      if (ballDY < -BALL_MAX_DY) ballDY = -BALL_MAX_DY;
      
      paddle1FlashFrames = PADDLE_FLASH_DURATION;
      onPaddleHit();
    }
  }
  
  // Right paddle (Player 2 / AI) collision
  if (ballDX > 0 && 
      ballX + BALL_RADIUS >= screenW - PADDLE_OFFSET - PADDLE_HEIGHT &&
      ballX - BALL_RADIUS <= screenW - PADDLE_OFFSET) {
    if (ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_WIDTH) {
      ballX = screenW - PADDLE_OFFSET - PADDLE_HEIGHT - BALL_RADIUS;
      ballDX = -ballDX;
      
      // Add spin based on hit location
      int16_t hitPos = ballY - (paddle2Y + PADDLE_WIDTH / 2);
      ballDY = hitPos / 4;
      if (ballDY == 0) ballDY = (random(2) == 0) ? 1 : -1;
      if (ballDY > BALL_MAX_DY) ballDY = BALL_MAX_DY;
      if (ballDY < -BALL_MAX_DY) ballDY = -BALL_MAX_DY;
      
      paddle2FlashFrames = PADDLE_FLASH_DURATION;
      onPaddleHit();
    }
  }
  
  // Scoring - ball goes past left paddle
  if (ballX < -BALL_RADIUS) {
    score2++;
    onScoreLost();
    drawScores();
    delay(500);
    resetBall();
  }
  
  // Scoring - ball goes past right paddle
  if (ballX > screenW + BALL_RADIUS) {
    score1++;
    onScoreGained(score1);
    drawScores();
    delay(500);
    resetBall();
  }
  
  // Draw ball at new position
  drawBall(ballX, ballY);
}

// ===========================================
// Setup and Loop
// ===========================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n[Setup] Pong Game Starting...");
  
  // Load saved settings
  loadSettings();
  
  // Turn on backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  // Initialize display (landscape orientation)
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  // Initialize buttons
  pinMode(P1_UP_BUTTON, INPUT_PULLUP);
  pinMode(P1_DOWN_BUTTON, INPUT_PULLUP);
  
  #if EXT_P1_UP >= 0
    pinMode(EXT_P1_UP, INPUT_PULLUP);
  #endif
  #if EXT_P1_DOWN >= 0
    pinMode(EXT_P1_DOWN, INPUT_PULLUP);
  #endif
  #if P2_UP_BUTTON >= 0
    pinMode(P2_UP_BUTTON, INPUT_PULLUP);
  #endif
  #if P2_DOWN_BUTTON >= 0
    pinMode(P2_DOWN_BUTTON, INPUT_PULLUP);
  #endif
  #if EXT_P2_UP >= 0
    pinMode(EXT_P2_UP, INPUT_PULLUP);
  #endif
  #if EXT_P2_DOWN >= 0
    pinMode(EXT_P2_DOWN, INPUT_PULLUP);
  #endif
  
  setupEventPins();
  
  // Show menu with Quick Play as default option
  menuSelection = 0;  // Start with Quick Play highlighted
  drawMenu();
  Serial.println("[Setup] Ready - showing menu");
}

void loop() {
  delay(GAME_DELAY);
  updateEventPins();
  
  switch (gameState) {
    case STATE_MENU:
      handleMenu();
      break;
      
    case STATE_PLAYING:
      updatePaddle1();
      updatePaddle2();
      updateBall();
      break;
      
    case STATE_GAME_OVER:
      // Could add game over screen here
      // For now, return to menu on button press
      if (isButtonPressed(P1_UP_BUTTON) || isButtonPressed(P1_DOWN_BUTTON)) {
        gameState = STATE_MENU;
        drawMenu();
      }
      break;
  }
}