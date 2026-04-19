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

#if BUZZER_ENABLED
  // ===========================================
  // Musical Note Frequencies (Hz)
  // ===========================================
  #define NOTE_C4  262
  #define NOTE_CS4 277
  #define NOTE_D4  294
  #define NOTE_DS4 311
  #define NOTE_E4  330
  #define NOTE_F4  349
  #define NOTE_FS4 370
  #define NOTE_G4  392
  #define NOTE_GS4 415
  #define NOTE_A4  440
  #define NOTE_AS4 466
  #define NOTE_B4  494
  #define NOTE_C5  523
  #define NOTE_CS5 554
  #define NOTE_D5  587
  #define NOTE_DS5 622
  #define NOTE_E5  659
  #define NOTE_F5  698
  #define NOTE_FS5 740
  #define NOTE_G5  784
  #define NOTE_GS5 831
  #define NOTE_A5  880
  #define NOTE_B5  988
  #define NOTE_C6  1047
  #define NOTE_REST 0

  // Melody queue for non-blocking playback
  #define MAX_MELODY_NOTES 8
  static int melodyNotes[MAX_MELODY_NOTES];
  static int melodyDurations[MAX_MELODY_NOTES];
  static int melodyLength = 0;
  static int melodyIndex = 0;
  static unsigned long nextNoteTime = 0;
  static bool melodyPlaying = false;

  // Sound priority (higher = more important, won't be interrupted)
  #define PRIORITY_WALL    1
  #define PRIORITY_PADDLE  2
  #define PRIORITY_SCORE   3
  static int currentPriority = 0;

  // Start playing a melody (non-blocking)
  static void playMelody(const int* notes, const int* durations, int length) {
    // Stop any current tone immediately
    ledcWriteTone(BUZZER_CHANNEL, 0);
    
    if (length > MAX_MELODY_NOTES) length = MAX_MELODY_NOTES;
    for (int i = 0; i < length; i++) {
      melodyNotes[i] = notes[i];
      melodyDurations[i] = durations[i];
    }
    melodyLength = length;
    melodyIndex = 0;
    melodyPlaying = true;
    nextNoteTime = millis();  // Start immediately
  }

  // Check if a melody is currently playing
  static bool isMelodyPlaying() {
    return melodyPlaying && (melodyIndex < melodyLength);
  }

  // Update melody playback - call this in loop
  static void updateMelody() {
    if (!melodyPlaying || melodyIndex >= melodyLength) {
      if (melodyPlaying) {
        ledcWriteTone(BUZZER_CHANNEL, 0);  // Ensure silence
        melodyPlaying = false;
        currentPriority = 0;  // Reset priority when melody ends
      }
      return;
    }
    
    unsigned long now = millis();
    if (now >= nextNoteTime) {
      int freq = melodyNotes[melodyIndex];
      int dur = melodyDurations[melodyIndex];
      
      if (freq > 0) {
        ledcWriteTone(BUZZER_CHANNEL, freq);
      } else {
        ledcWriteTone(BUZZER_CHANNEL, 0);  // Rest
      }
      
      nextNoteTime = now + dur;
      melodyIndex++;
    }
  }

  // Pre-defined melodies for game events
  static void playWallBounceSound() {
    // Don't interrupt higher priority sounds
    if (isMelodyPlaying() && currentPriority > PRIORITY_WALL) return;
    currentPriority = PRIORITY_WALL;
    
    // Quick single note click
    static const int notes[] = {NOTE_E5};
    static const int durs[] = {30};
    playMelody(notes, durs, 1);
  }

  static void playPaddleHitSound() {
    // Don't interrupt equal or higher priority sounds
    if (isMelodyPlaying() && currentPriority >= PRIORITY_PADDLE) return;
    currentPriority = PRIORITY_PADDLE;
    
    // Quick ascending ping: C5 -> G5
    static const int notes[] = {NOTE_C5, NOTE_G5};
    static const int durs[] = {40, 50};
    playMelody(notes, durs, 2);
  }

  static void playScoreGainedSound() {
    currentPriority = PRIORITY_SCORE;
    
    // Happy victory arpeggio: C4 -> E4 -> G4 -> C5 (major chord)
    static const int notes[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
    static const int durs[] = {60, 60, 60, 120};
    playMelody(notes, durs, 4);
  }

  static void playScoreLostSound() {
    currentPriority = PRIORITY_SCORE;
    
    // Sad descending: G4 -> E4 -> C4
    static const int notes[] = {NOTE_G4, NOTE_E4, NOTE_C4};
    static const int durs[] = {100, 100, 150};
    playMelody(notes, durs, 3);
  }

  // ===========================================
  // Startup Melodies (Public Domain / Open Source)
  // ===========================================
  // Change STARTUP_MELODY in config.h to select (0-5)
  
  // 0: Simple C Major arpeggio (default)
  static void playStartupMelody0() {
    static const int notes[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
    static const int durs[] = {80, 80, 80, 150};
    playMelody(notes, durs, 4);
  }

  // 1: "Charge!" fanfare (public domain - sports cheer)
  static void playStartupMelody1() {
    static const int notes[] = {NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_G5};
    static const int durs[] = {100, 100, 100, 150, 100, 250};
    playMelody(notes, durs, 6);
  }

  // 2: "Shave and a Haircut" (public domain)
  static void playStartupMelody2() {
    static const int notes[] = {NOTE_C5, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_REST, NOTE_B4, NOTE_C5};
    static const int durs[] = {100, 100, 100, 100, 100, 100, 100, 200};
    playMelody(notes, durs, 8);
  }

  // 3: Westminster Chimes intro (public domain - Big Ben)
  static void playStartupMelody3() {
    static const int notes[] = {NOTE_E4, NOTE_D4, NOTE_C4, NOTE_G4};
    static const int durs[] = {250, 250, 250, 500};
    playMelody(notes, durs, 4);
  }

  // 4: Retro "Power Up" (ascending chromatic flourish)
  static void playStartupMelody4() {
    static const int notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
    static const int durs[] = {50, 50, 50, 50, 50, 50, 50, 150};
    playMelody(notes, durs, 8);
  }

  // 5: Classic arcade "Ready!" jingle
  static void playStartupMelody5() {
    static const int notes[] = {NOTE_E5, NOTE_E5, NOTE_REST, NOTE_E5, NOTE_REST, NOTE_C5, NOTE_E5, NOTE_G5};
    static const int durs[] = {80, 80, 40, 80, 40, 80, 80, 200};
    playMelody(notes, durs, 8);
  }

  // Play the selected startup melody
  static void playStartupMelody() {
    #if STARTUP_MELODY == 1
      playStartupMelody1();
    #elif STARTUP_MELODY == 2
      playStartupMelody2();
    #elif STARTUP_MELODY == 3
      playStartupMelody3();
    #elif STARTUP_MELODY == 4
      playStartupMelody4();
    #elif STARTUP_MELODY == 5
      playStartupMelody5();
    #else
      playStartupMelody0();  // Default
    #endif
  }
#endif

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

  // Initialize passive buzzer
  #if BUZZER_ENABLED
    ledcSetup(BUZZER_CHANNEL, 2000, 8);  // 2kHz initial freq, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWriteTone(BUZZER_CHANNEL, 0);  // Start silent
    Serial.println("[Buzzer] Musical mode ready on GPIO" + String(BUZZER_PIN));
    
    // Play startup melody
    playStartupMelody();
  #else
    Serial.println("[Buzzer] Disabled in config.h");
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

  // Update buzzer melody playback
  #if BUZZER_ENABLED
    updateMelody();
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

  #if BUZZER_ENABLED
    playWallBounceSound();
  #endif
}

void onPaddleHit() {
  #if PIN_PADDLE_HIT >= 0
    digitalWrite(PIN_PADDLE_HIT, HIGH);
    paddleHitOffTime = millis() + EVENT_PULSE_MS;
  #endif

  #if BUZZER_ENABLED
    playPaddleHitSound();
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

  #if BUZZER_ENABLED
    playScoreGainedSound();
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

  #if BUZZER_ENABLED
    playScoreLostSound();
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
