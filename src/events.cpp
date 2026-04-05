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
  
  // Helper: check if DFPlayer is currently playing
  static bool isDFPlayerBusy() {
    #if DFPLAYER_BUSY_PIN >= 0
      return digitalRead(DFPLAYER_BUSY_PIN) == LOW;  // BUSY pin is LOW when playing
    #else
      return false;  // No BUSY pin, assume not busy
    #endif
  }
#endif

// Track when to turn off event pins (0 = already off)
static unsigned long wallBounceOffTime = 0;
static unsigned long paddleHitOffTime = 0;
static unsigned long scoreGainedOffTime = 0;
static unsigned long scoreLostOffTime = 0;

#if ISD1820_ENABLED
  static unsigned long isd1820OffTime = 0;  // Tracks when to release the P-E trigger pulse

  // Pulse the ISD1820 P-E pin to trigger playback (edge-triggered)
  static void triggerISD1820() {
    if (isd1820OffTime > 0) return;  // Already playing — don't overlap pulses
    digitalWrite(ISD1820_PLAY_PIN, HIGH);
    isd1820OffTime = millis() + ISD1820_PULSE_MS;
    Serial.println("[ISD1820] Triggered playback");
  }
#endif

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

  // Initialize ISD1820 Voice Record Playback Module
  #if ISD1820_ENABLED
    pinMode(ISD1820_PLAY_PIN, OUTPUT);
    digitalWrite(ISD1820_PLAY_PIN, LOW);
    Serial.println("[ISD1820] Ready on GPIO" + String(ISD1820_PLAY_PIN));
    #if ISD1820_TRIGGER_PADDLE_HIT
      Serial.println("[ISD1820] Triggers on: paddle hit");
    #endif
    #if ISD1820_TRIGGER_SCORE_LOST
      Serial.println("[ISD1820] Triggers on: score lost");
    #endif
  #else
    Serial.println("[ISD1820] Disabled in config.h");
  #endif

  // Initialize DFPlayer Mini
  #if DFPLAYER_ENABLED
    // Initialize BUSY pin if configured
    #if DFPLAYER_BUSY_PIN >= 0
      pinMode(DFPLAYER_BUSY_PIN, INPUT);
      Serial.println("[DFPlayer] BUSY pin: GPIO" + String(DFPLAYER_BUSY_PIN));
    #endif
    
    Serial.println("[DFPlayer] Initializing on TX:" + String(DFPLAYER_TX_PIN) + " RX:" + String(DFPLAYER_RX_PIN));
    dfPlayerSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(500);  // Give DFPlayer more time to initialize
    
    if (dfPlayer.begin(dfPlayerSerial)) {
      dfPlayerReady = true;
      dfPlayer.volume(DFPLAYER_VOLUME);
      Serial.println("[DFPlayer] OK! Volume set to " + String(DFPLAYER_VOLUME));
      
      // Report SD card file count
      delay(100);
      int fileCount = dfPlayer.readFileCounts();
      Serial.println("[DFPlayer] Files on SD card: " + String(fileCount));
    } else {
      dfPlayerReady = false;
      Serial.println("[DFPlayer] FAILED! Check wiring:");
      Serial.println("  - TX (ESP32 GPIO" + String(DFPLAYER_TX_PIN) + ") -> DFPlayer RX");
      Serial.println("  - RX (ESP32 GPIO" + String(DFPLAYER_RX_PIN) + ") <- DFPlayer TX");
      Serial.println("  - VCC -> 5V, GND -> GND");
    }
  #else
    Serial.println("[DFPlayer] Disabled in config.h");
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

  // Release ISD1820 P-E trigger pin after pulse
  #if ISD1820_ENABLED
    if (isd1820OffTime > 0 && now >= isd1820OffTime) {
      digitalWrite(ISD1820_PLAY_PIN, LOW);
      isd1820OffTime = 0;
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
}

void onPaddleHit() {
  #if PIN_PADDLE_HIT >= 0
    digitalWrite(PIN_PADDLE_HIT, HIGH);
    paddleHitOffTime = millis() + EVENT_PULSE_MS;
  #endif

  #if ISD1820_ENABLED && ISD1820_TRIGGER_PADDLE_HIT
    triggerISD1820();
  #endif

  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      // Paddle hit always plays (high priority - interrupts bounce sounds)
      if (isDFPlayerBusy()) {
        Serial.println("[DFPlayer] Interrupting for track " + String(TRACK_PADDLE_HIT) + " (paddle hit)");
      } else {
        Serial.println("[DFPlayer] Playing track " + String(TRACK_PADDLE_HIT) + " (paddle hit)");
      }
      dfPlayer.playMp3Folder(TRACK_PADDLE_HIT);
    }
  #endif
}

void onScoreGained(int16_t newScore) {
  #if PIN_SCORE_GAINED >= 0
    digitalWrite(PIN_SCORE_GAINED, HIGH);
    scoreGainedOffTime = millis() + EVENT_PULSE_MS;
  #endif

  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      // Play award fanfare at score milestones, regular hit sound otherwise
      bool isMilestone = (newScore > 0 && newScore % SCORE_MILESTONE_INTERVAL == 0);
      int trackToPlay = isMilestone ? TRACK_AWARD : TRACK_PADDLE_HIT;
      const char* label = isMilestone ? "milestone award" : "score gained";

      #if DFPLAYER_WAIT_FOR_FINISH
        if (isDFPlayerBusy() && !isMilestone) {
          Serial.println("[DFPlayer] Skipped track " + String(trackToPlay) + " (still playing)");
        } else {
          Serial.println("[DFPlayer] Playing track " + String(trackToPlay) + " (" + label + ")");
          dfPlayer.playMp3Folder(trackToPlay);
        }
      #else
        Serial.println("[DFPlayer] Playing track " + String(trackToPlay) + " (" + label + ")");
        dfPlayer.playMp3Folder(trackToPlay);
      #endif
    }
  #endif
}

void onScoreLost() {
  #if PIN_SCORE_LOST >= 0
    digitalWrite(PIN_SCORE_LOST, HIGH);
    scoreLostOffTime = millis() + EVENT_PULSE_MS;
  #endif

  #if ISD1820_ENABLED && ISD1820_TRIGGER_SCORE_LOST
    triggerISD1820();
  #endif

  #if DFPLAYER_ENABLED
    if (dfPlayerReady) {
      // Score lost always plays (high priority - interrupts other sounds)
      if (isDFPlayerBusy()) {
        Serial.println("[DFPlayer] Interrupting for track " + String(TRACK_SCORE_LOST) + " (score lost)");
      } else {
        Serial.println("[DFPlayer] Playing track " + String(TRACK_SCORE_LOST) + " (score lost)");
      }
      dfPlayer.playMp3Folder(TRACK_SCORE_LOST);
    }
  #endif
}
