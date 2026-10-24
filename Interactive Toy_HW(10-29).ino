const int R_PIN      = 9;
const int G_PIN      = 10;
const int B_PIN      = 11;
const int BUTTON_PIN = 2;
const int LIGHT_PIN  = A0;

const bool COMMON_ANODE    = true;  
const bool DARK_IS_HIGH    = true; 
const int  LIGHT_THRESHOLD = 250;  

const int MIN_MOOD     = 0;
const int NEUTRAL_MOOD = 10;
const int MAX_MOOD     = 20;

const unsigned long UNTOUCH_MS     = 5000;  // ignored this long -> start getting angry
const unsigned long DECAY_STEP_MS  = 1000;  // time per red step
const unsigned long BLINK_MS       = 300;
const unsigned long DARK_TO_SLEEP  = 5000;
const unsigned long SLEEP_FADE_MS  = 3000;  // red -> yellow
const unsigned long YELLOW_HOLD_MS = 3000;
const unsigned long SLEEP_OFF_MIN_MS = 3000;  // LED stays off at least this long before waking
const unsigned long WAKE_FADE_MS   = 5000;  // off -> green (normal speed)
const unsigned long WAKE_SPEEDUP_MS = 500;  // after a press, the rest of the fade takes this long
const int YELLOW_GREEN_LEVEL = 160;         // lower if yellow looks too green

// ---------- NEW (sunbath) settings ----------
const int  SUN_THRESHOLD = 100;                  // sensor value BELOW this = flashlight on it (yours reads ~30-60)
const unsigned long SUN_ENTER_MS    = 300;      // light must stay this long before the sunbath starts
const unsigned long SUN_LEAVE_MS    = 500;      // light must be gone this long before the sunbath ends
const unsigned long SUN_TO_BROWN_MS = 5000;     // orange -> dark brown
const int SUN_STEPS = 10;                       // number of darker steps 
const unsigned long WAVE_MS         = 1200;     // length of one brightness wave (100% -> 50% -> 100%)
const unsigned long HOT_FLASH_MS    = 500;      // red flicker when you press during the sunbath
const unsigned long HOT_BLINK_MS    = 125;      // red on / off speed of that flicker
const int ORANGE_R = 150, ORANGE_G = 100, ORANGE_B = 0;   // start color
const int BROWN_R  = 80,  BROWN_G  = 20,  BROWN_B  = 0;   // end color (dark brown)


enum State { AWAKE, DARK_BLINK, SLEEP_FADE, SLEEP_HOLD, SLEEP_OFF, WAKE_FADE, SUNBATH };   // NEW: SUNBATH
State state = AWAKE;
unsigned long stateStart = 0;

int mood = NEUTRAL_MOOD;
bool buttonWasPressed = false;
unsigned long lastTouch = 0;
unsigned long lastDecay = 0;

// wake fade progress (so a press can speed it up smoothly)
float wakeStartLevel = 0;                 // 0..1 brightness when the current speed began
unsigned long wakeDuration = WAKE_FADE_MS;
bool wakeSpedUp = false;

// sunbath variables
bool sunnyNow = false;                    // is the flashlight on the sensor right now?
unsigned long sunnyChangedAt = 0;         // when sunnyNow last changed
bool grillPrinted = false;                // "grill duck" message printed yet?
bool hotActive = false;                   // red flicker running?
unsigned long hotStart = 0;


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  delay(200);                                              // let the pin settle
  buttonWasPressed = (digitalRead(BUTTON_PIN) == LOW);     // a button already "down" at power-up is not a click
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  wakeToNeutral();
}

void loop() {
  bool dark = isDark();
  bool buttonClicked = readButtonClick();
  trackSun();                                              // NEW

  switch (state) {
    case AWAKE:      updateAwake(dark, buttonClicked);      break;
    case DARK_BLINK: updateDarkBlink(dark, buttonClicked);  break;
    case SLEEP_FADE: updateSleepFade();                     break;
    case SLEEP_HOLD: updateSleepHold();                     break;
    case SLEEP_OFF:  updateSleepOff(dark, buttonClicked);   break;
    case WAKE_FADE:  updateWakeFade(dark, buttonClicked);   break;
    case SUNBATH:    updateSunbath(buttonClicked);          break;   // NEW
  }
}


void changeState(State next) {
  state = next;
  stateStart = millis();
}

void enterAwake() {                 // keeps the current mood
  lastTouch = millis();
  lastDecay = millis();
  changeState(AWAKE);
}

void wakeToNeutral() {              // fade finished: fully green + announce
  mood = NEUTRAL_MOOD;
  Serial.println("Quack! Duck is here!");
  enterAwake();
}

void startSleep() {
  Serial.println("I'm so sleepy Quack Quack...");
  changeState(SLEEP_FADE);
}

void startWakeFade() {              // always begins from LED off, normal speed
  wakeStartLevel = 0;
  wakeDuration = WAKE_FADE_MS;
  wakeSpedUp = false;
  changeState(WAKE_FADE);
}


void updateAwake(bool dark, bool buttonClicked) {
  if (dark) {
    Serial.println("QUACK?  Why it is so dark??");
    changeState(DARK_BLINK);
    return;
  }
  if (sunReady()) {                  // NEW: flashlight detected -> sunbath
    startSunbath();
    return;
  }
  if (buttonClicked) handleButtonClick();
  handleDecay();
  showMood(mood);
}

void updateDarkBlink(bool dark, bool buttonClicked) {
  if (!dark) {                       // dark was short: restore previous mood
    enterAwake();
    return;
  }
  if (buttonClicked) {               // press = skip straight to sleep (yellow hold)
    Serial.println("'m so sleepy Quack Quack...");
    changeState(SLEEP_HOLD);
    return;
  }
  bool on = ((millis() - stateStart) / BLINK_MS) % 2 == 0;
  setColor(on ? 255 : 0, 0, 0);

  if (millis() - stateStart > DARK_TO_SLEEP) startSleep();
}

// sensor and button are ignored in SLEEP_FADE and SLEEP_HOLD
void updateSleepFade() {
  float t = progress(SLEEP_FADE_MS);
  setColor(255, YELLOW_GREEN_LEVEL * t, 0);      // red -> yellow
  if (t >= 1.0) changeState(SLEEP_HOLD);
}

void updateSleepHold() {
  setColor(255, YELLOW_GREEN_LEVEL, 0);
  if (millis() - stateStart >= YELLOW_HOLD_MS) changeState(SLEEP_OFF);
}

void updateSleepOff(bool dark, bool buttonClicked) {
  setColor(0, 0, 0);

  // stay off for a minimum time, ignoring light and button
  if (millis() - stateStart < SLEEP_OFF_MIN_MS) return;

  if (buttonClicked || !dark) startWakeFade();   // light or press -> start fading in
}

void updateWakeFade(bool dark, bool buttonClicked) {
  if (dark) {                        // dark again: yellow, then off
    Serial.println("Let me sleep quack quack...");
    changeState(SLEEP_HOLD);
  }

  float t = wakeLevel();

  if (buttonClicked && !wakeSpedUp) {            // press = finish the fade in 0.5s
    wakeStartLevel = t;
    wakeDuration = WAKE_SPEEDUP_MS;
    wakeSpedUp = true;
    stateStart = millis();
  }

  t = wakeLevel();
  setColor(0, 255 * t, 0);                       // off -> green
  if (t >= 1.0) wakeToNeutral();
}

// current wake fade brightness 0..1 (continues from wakeStartLevel)
float wakeLevel() {
  float f = (millis() - stateStart) / (float)wakeDuration;
  if (f > 1.0) f = 1.0;
  return wakeStartLevel + (1.0 - wakeStartLevel) * f;
}


void handleButtonClick() {
  lastTouch = millis();
  if (mood >= MAX_MOOD) return;      

  mood++;
  printMoodMessage("Yipee!");
}

void handleDecay() {
  unsigned long now = millis();
  if (mood > MIN_MOOD && now - lastTouch > UNTOUCH_MS && now - lastDecay > DECAY_STEP_MS) {
    mood--;
    lastDecay = now;
    printMoodMessage("Quack!");
  }
}


void printMoodMessage(const char* stepMessage) {
  if (mood == MAX_MOOD)      Serial.println("QUACK QUACK! The duck is very happy-yipee!");
  else if (mood == MIN_MOOD) Serial.println("The duck is ANGRY :(");
  else                       Serial.println(stepMessage);
}


bool readButtonClick() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);
  bool clicked = pressed && !buttonWasPressed;
  buttonWasPressed = pressed;
  return clicked;
}

bool isDark() {
  int value = analogRead(LIGHT_PIN);
  return DARK_IS_HIGH ? (value > LIGHT_THRESHOLD) : (value < LIGHT_THRESHOLD);
}


void setColor(int r, int g, int b) {
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);
  if (COMMON_ANODE) { r = 255 - r; g = 255 - g; b = 255 - b; }
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}

// mood 0 = red, 10 = green, 20 = blue
void showMood(int m) {
  m = constrain(m, MIN_MOOD, MAX_MOOD);
  if (m >= NEUTRAL_MOOD) {
    float t = (m - NEUTRAL_MOOD) / (float)NEUTRAL_MOOD;   // 0 = green, 1 = blue
    setColor(0, 255 * (1 - t), 255 * t);
  } else {
    float t = (NEUTRAL_MOOD - m) / (float)NEUTRAL_MOOD;   // 0 = green, 1 = red
    setColor(255 * t, 255 * (1 - t), 0);
  }
}

float progress(unsigned long duration) {
  float t = (millis() - stateStart) / (float)duration;
  return t > 1.0 ? 1.0 : t;
}


// true when the sensor is lit by a flashlight (reads below SUN_THRESHOLD)
bool isSunny() {
  int value = analogRead(LIGHT_PIN);
  return DARK_IS_HIGH ? (value < SUN_THRESHOLD) : (value > 1023 - SUN_THRESHOLD);
}

// called once per loop: remembers when the flashlight appeared / disappeared
void trackSun() {
  bool sunny = isSunny();
  if (sunny != sunnyNow) {
    sunnyNow = sunny;
    sunnyChangedAt = millis();
  }
}

// the flashlight has been on the sensor for a moment
bool sunReady() {
  return sunnyNow && millis() - sunnyChangedAt >= SUN_ENTER_MS;
}

// the flashlight has been gone for a moment
bool sunGone() {
  return !sunnyNow && millis() - sunnyChangedAt >= SUN_LEAVE_MS;
}

void startSunbath() {
  Serial.println("Yipee! Sunbathing time!");
  grillPrinted = false;
  hotActive = false;
  changeState(SUNBATH);
}

void updateSunbath(bool buttonClicked) {
  // light source is out -> back to the mood the duck had before
  if (sunGone()) {
    enterAwake();
    return;
  }

  // button pressed -> red flicker + message (mood does not change)
  if (buttonClicked) {
    hotActive = true;
    hotStart = millis();
    Serial.println("Too hot to play, QUACK!");
  }

  unsigned long step = (millis() - stateStart) / (SUN_TO_BROWN_MS / SUN_STEPS);
  if (step > SUN_STEPS) step = SUN_STEPS;
  float t = step / (float)SUN_STEPS;             // 0 = orange ... 1 = dark brown

  // fully brown: say it once
  if (t >= 1.0 && !grillPrinted) {
    grillPrinted = true;
    Serial.println("Quack..., I have become a grill duck.");
  }

  // red flicker has priority over the sunbath color
  if (hotActive) {
    unsigned long hotTime = millis() - hotStart;
    if (hotTime >= HOT_FLASH_MS) {
      hotActive = false;
    } else if ((hotTime / HOT_BLINK_MS) % 2 == 0) {
      setColor(255, 0, 0);
      return;
    }
  }

  // color: orange -> brown
  float r = ORANGE_R + (BROWN_R - ORANGE_R) * t;
  float g = ORANGE_G + (BROWN_G - ORANGE_G) * t;
  float b = ORANGE_B + (BROWN_B - ORANGE_B) * t;

  // wave: brightness goes 100% -> 50% -> 100% ... until fully brown, then steady
  float brightness = 1.0;
  if (t < 1.0) {
    float phase = (millis() - stateStart) % WAVE_MS / (float)WAVE_MS;
    brightness = 0.75 + 0.25 * sin(TWO_PI * phase);
  }

  setColor(r * brightness, g * brightness, b * brightness);
}
