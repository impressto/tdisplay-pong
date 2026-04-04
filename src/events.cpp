#include "events.h"

// ===========================================
// Event Hook System Implementation
// ===========================================
// This file handles triggering GPIO pins when
// game events happen. Modify this file to add
// your own custom behaviors!

// ===========================================
// DFPlayer Mini Setup
// ===========================================
#if DFPLAYER_ENABLED
  #include <HardwareSerial.h>
  static HardwareSerial dfPlayerSerial(1);  // Use UART1
  static DFRobotDFPlayerMini dfPlayer;
  static bool dfPlayerReady = false;
#endif

// Track when to turn off event pins (0 = already off)
static unsigned long wallBounceOffTime = 0;
static unsigned long paddleHitOffTime = 0;
static unsigned long scoreGainedOffTime = 0;
static unsigned long scoreLostOffTime = 0;

// Initialize all event pins as outputs
void setupEventPins() {
  #if PIN_WALL_BOUNCE >= 0
    pinMode(PIN_WALL_BOUNCE, OUTPUT);
    digitalWrite(PIN_WALL_BOUNCE, LOW);
  #endif
  
  #if PIN_PADDLE_HIT >= 0
    pinMode(PIN_PADDLE_HIT, OUTPUT);
    digitalWrite(PIN_PADDLE_HIT, LOW);
  #endif
  
  #if PIN_SCORE_GAINED >= 0
    pinMode(PIN_SCORE_GAINED, OUTPUT);
    digitalWrite(PIN_SCORE_GAINED, LOW);
  #endif
  
  #if PIN_SCORE_LOST >= 0
    pinMode(PIN_SCORE_LOST, OUTPUT);
    digitalWrite(PIN_SCORE_LOST, LOW);
  #endif

  // Initialize DFPlayer Mini
  #if DFPLAYER_ENABLED
    dfPlayerSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(100);  // Give DFPlayer time to initialize
    
    if (dfPlayer.begin(dfPlayerSerial)) {
      dfPlayerReady = true;
      dfPlayer.volume(DFPLAYER_VOLUME);
      Serial.println("DFPlayer Mini initialized!");
    } else {
      dfPlayerReady = false;
      Serial.println("DFPlayer Mini not found - check wiring!");
    }
  #endif
}

// Turn off pins after their pulse duration has passed
void updateEventPins() {
  unsigned long now = millis();
  
  #if PIN_WALL_BOUNCE >= 0
    if (wallBounceOffTime > 0 && now >= wallBounceOffTime) {
      digitalWrite(PIN_WALL_BOUNCE, LOW);
      wallBounceOffTime = 0;
    }
  #endif
  
  #if PIN_PADDLE_HIT >= 0
    if (paddleHitOffTime > 0 && now >= paddleHitOffTime) {
      digitalWrite(PIN_PADDLE_HIT, LOW);
      paddleHitOffTime = 0;
    }
  #endif
  
  #if PIN_SCORE_GAINED >= 0
    if (scoreGainedOffTime > 0 && now >= scoreGainedOffTime) {
      digitalWrite(PIN_SCORE_GAINED, LOW);
      scoreGainedOffTime = 0;
    }
  #endif
  
  #if PIN_SCORE_LOST >= 0
    if (scoreLostOffTime > 0 && now >= scoreLostOffTime) {
      digitalWrite(PIN_SCORE_LOST, LOW);
      scoreLostOffTime = 0;
    }
  #endif
}

// ===========================================
// Event Trigger Functions
// ===========================================
// These turn on the GPIO pin for a short pulse.
// The pin automatically turns off after EVENT_PULSE_MS.

void onWallBounce() {
  #if PIN_WALL_BOUNCE >= 0
    digitalWrite(PIN_WALL_BOUNCE, HIGH);
    wallBounceOffTime = millis() + EVENT_PULSE_MS;
  #endif
  
  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      dfPlayer.play(TRACK_WALL_BOUNCE);  // Play track 1
    }
  #endif
}

void onPaddleHit() {
  #if PIN_PADDLE_HIT >= 0
    digitalWrite(PIN_PADDLE_HIT, HIGH);
    paddleHitOffTime = millis() + EVENT_PULSE_MS;
  #endif
  
  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      dfPlayer.play(TRACK_PADDLE_HIT);  // Play track 2
    }
  #endif
}

void onScoreGained() {
  #if PIN_SCORE_GAINED >= 0
    digitalWrite(PIN_SCORE_GAINED, HIGH);
    scoreGainedOffTime = millis() + EVENT_PULSE_MS;
  #endif
  
  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      dfPlayer.play(TRACK_PADDLE_HIT);  // Play track 2 (same as paddle hit)
    }
  #endif
}

void onScoreLost() {
  #if PIN_SCORE_LOST >= 0
    digitalWrite(PIN_SCORE_LOST, HIGH);
    scoreLostOffTime = millis() + EVENT_PULSE_MS;
  #endif
  
  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      dfPlayer.play(TRACK_SCORE_LOST);  // Play track 3
    }
  #endif
}
