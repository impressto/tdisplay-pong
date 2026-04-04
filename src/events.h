#ifndef EVENTS_H
#define EVENTS_H

// ===========================================
// Event Hook System - For Teaching Electronics!
// ===========================================
// These functions trigger GPIO pins when game
// events happen. Kids can connect LEDs, buzzers,
// or motors to see code controlling hardware!
//
// How to use:
// 1. Call setupEventPins() in your setup() function
// 2. Call updateEventPins() in your loop() function
// 3. Call the event functions when things happen:
//    - onWallBounce()  -> ball hits a wall
//    - onPaddleHit()   -> ball hits the paddle
//    - onScoreGained() -> player scores a point
//    - onScoreLost()   -> player misses the ball
//
// Configure which pins to use in config.h!
// ===========================================

#include <Arduino.h>
#include "config.h"

#if DFPLAYER_ENABLED
  #include <DFRobotDFPlayerMini.h>
#endif

// Initialize all event pins - call this in setup()
void setupEventPins();

// Update event pins (turns them off after pulse) - call this in loop()
void updateEventPins();

// Event trigger functions - call these when events happen!
void onWallBounce();   // Ball bounced off a wall
void onPaddleHit();    // Ball hit the paddle
void onScoreGained();  // Player scored a point
void onScoreLost();    // Player missed the ball

#endif
