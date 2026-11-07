const int buttonPin = 2;

const int RLedPin = 9;
const int GLedPin = 10;
const int BLedPin = 11;

int buttonState = 0;
bool buttonPressed = false;
unsigned long pressingTime = 0;
const int longPressInterval = 1500;

int lastReading = LOW;
unsigned long lastChangeTime = 0;
const int debounceDelay = 50;

int clickCount = 0;
unsigned long lastClickTime = 0;
const int doubleClickWindow = 350;

const int colorCount = 8;
const int colorTable[colorCount][3] = {
  {255, 255, 255},
  {255,   0,   0},
  {  0, 255,   0},
  {  0,   0, 255},
  {255,  40, 120},
  {255, 180,   0},
  {150,   0, 255},
  {255,  60,   0}
};

int lightNum = 0;
int RLightColor = 255;
int GLightColor = 255;
int BLightColor = 255;
int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

int currentMode = 0;
const int lastRequiredMode = 2;
const int firstSpecialMode = 3;
const int lastSpecialMode = 5;

unsigned long blinkTimer = 0;
const int blinkInterval = 500;
bool blinkOn = true;

int breathLevel = 255;
const int fadeAmount = 2;
int fadeDirection = -1;

int rainbowIndex = 1;
unsigned long rainbowTimer = 0;
const int rainbowStepTime = 300;

int fireworkStage = 0;
unsigned long fireworkStageStart = 0;
int fireworkHue = 0;
unsigned long fireworkPause = 0;
unsigned long sparkleTimer = 0;
bool sparkleOn = true;
const int launchTime = 350;
const int flashTime = 70;
const int bloomTime = 1100;
const int sparkleInterval = 40;

unsigned long beatInterval = 500;
unsigned long beatStart = 0;
unsigned long lastTapTime = 0;
int tapCount = 0;
unsigned long tapIntervals[3] = {500, 500, 500};
const int maxTapGap = 2000;
const int minBeatInterval = 200;

void setup()
{
  pinMode(buttonPin, INPUT);
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop()
{
  checkButton();
  updateLEDColor();
  setRGBLEDColor(RCurrentColor, GCurrentColor, BCurrentColor);
}

void setRGBLEDColor(int r, int g, int b){
  analogWrite(RLedPin, r);
  analogWrite(GLedPin, g);
  analogWrite(BLedPin, b);
}

void checkButton(){
  int reading = !digitalRead(buttonPin);

  if(reading != lastReading){
    lastChangeTime = millis();
    lastReading = reading;
  }
  if(millis() - lastChangeTime > debounceDelay){
    buttonState = reading;
  }

  if(buttonState == HIGH && !buttonPressed){
    pressingTime = millis();
    buttonPressed = true;
    if(currentMode == 5){
      registerTap();
      Serial.println("tap");
    }
  }
  if(buttonState == LOW && buttonPressed){
    unsigned long currentTime = millis();
    if(currentTime - pressingTime >= longPressInterval){
      clickCount = 0;
      handleLongPress();
      Serial.println("long");
    }
    else if(currentMode != 5){
      clickCount = clickCount + 1;
      lastClickTime = currentTime;
    }
    buttonPressed = false;
  }

  if(clickCount > 0 && !buttonPressed && millis() - lastClickTime > doubleClickWindow){
    if(clickCount == 1){
      handleShortClick();
      Serial.println("short");
    }
    else{
      handleDoubleClick();
      Serial.println("double");
    }
    clickCount = 0;
  }
}

void handleShortClick(){
  if(currentMode <= lastRequiredMode){
    changeLEDColor();
  }
  else if(currentMode == 4){
    startFirework();
  }
}

void handleLongPress(){
  if(currentMode >= firstSpecialMode){
    currentMode = 0;
  }
  else{
    changeMode();
  }
}

void handleDoubleClick(){
  if(currentMode < firstSpecialMode || currentMode >= lastSpecialMode){
    currentMode = firstSpecialMode;
  }
  else{
    currentMode = currentMode + 1;
  }

  if(currentMode == 3){
    rainbowIndex = 1;
    rainbowTimer = millis();
  }
  if(currentMode == 4){
    startFirework();
  }
  if(currentMode == 5){
    tapCount = 0;
    beatStart = millis();
  }
}

void changeLEDColor(){
  lightNum = lightNum + 1;
  if(lightNum >= colorCount)
    lightNum = 0;

  RLightColor = colorTable[lightNum][0];
  GLightColor = colorTable[lightNum][1];
  BLightColor = colorTable[lightNum][2];
}

void changeMode(){
  currentMode = currentMode + 1;
  if(currentMode > lastRequiredMode){
    currentMode = 0;
  }
  if(currentMode == 1){
    blinkTimer = 0;
    blinkOn = true;
  }
  if(currentMode == 2){
    breathLevel = 255;
    fadeDirection = -1;
  }
}

void hueToRGB(int hue, int &r, int &g, int &b){
  int part = hue / 60;
  int rising = (hue % 60) * 255 / 60;
  int falling = 255 - rising;

  switch(part){
    case 0:  r = 255;     g = rising;  b = 0;       break;
    case 1:  r = falling; g = 255;     b = 0;       break;
    case 2:  r = 0;       g = 255;     b = rising;  break;
    case 3:  r = 0;       g = falling; b = 255;     break;
    case 4:  r = rising;  g = 0;       b = 255;     break;
    default: r = 255;     g = 0;       b = falling; break;
  }
}

void showColor(int r, int g, int b, int brightness){
  RCurrentColor = 255 - (long)r * brightness / 255;
  GCurrentColor = 255 - (long)g * brightness / 255;
  BCurrentColor = 255 - (long)b * brightness / 255;
}

void showTableColor(int index, int brightness){
  showColor(colorTable[index][0], colorTable[index][1], colorTable[index][2], brightness);
}

void updateBreathing(){
  breathLevel = breathLevel + fadeDirection * fadeAmount;

  if(breathLevel <= 0){
    breathLevel = 0;
    fadeDirection = 1;
  }
  if(breathLevel >= 255){
    breathLevel = 255;
    fadeDirection = -1;
  }

  showColor(RLightColor, GLightColor, BLightColor, breathLevel);
  delay(10);
}

void updateRainbow(){
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - rainbowTimer;

  if(elapsed >= rainbowStepTime){
    rainbowIndex = (rainbowIndex + 1) % colorCount;
    rainbowTimer = currentTime;
    elapsed = 0;
  }
  int nextIndex = (rainbowIndex + 1) % colorCount;

  long amount = (long)elapsed * 255 / rainbowStepTime;

  int r = colorTable[rainbowIndex][0] + (colorTable[nextIndex][0] - colorTable[rainbowIndex][0]) * amount / 255;
  int g = colorTable[rainbowIndex][1] + (colorTable[nextIndex][1] - colorTable[rainbowIndex][1]) * amount / 255;
  int b = colorTable[rainbowIndex][2] + (colorTable[nextIndex][2] - colorTable[rainbowIndex][2]) * amount / 255;
  showColor(r, g, b, 255);
}

void startFirework(){
  fireworkStage = 0;
  fireworkStageStart = millis();
  fireworkHue = random(0, 360);
  sparkleOn = true;
}

void updateSparkle(int chance){
  unsigned long currentTime = millis();
  if(currentTime - sparkleTimer >= sparkleInterval){
    sparkleOn = random(0, 100) < chance;
    sparkleTimer = currentTime;
  }
}

void updateFirework(){
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - fireworkStageStart;

  if(fireworkStage == 0){
    if(elapsed >= launchTime){
      fireworkStage = 1;
      fireworkStageStart = currentTime;
    }
    int level = 20 + elapsed * 100 / launchTime;
    updateSparkle(70);
    if(!sparkleOn) level = level / 3;
    showColor(255, 120, 20, level);
  }
  else if(fireworkStage == 1){
    if(elapsed >= flashTime){
      fireworkStage = 2;
      fireworkStageStart = currentTime;
    }
    showColor(255, 255, 255, 255);
  }
  else if(fireworkStage == 2){
    if(elapsed >= bloomTime){
      fireworkStage = 3;
      fireworkStageStart = currentTime;
      fireworkPause = random(400, 900);
      showColor(0, 0, 0, 0);
      return;
    }
    long remaining = (long)(bloomTime - elapsed) * 255 / bloomTime;
    int level = remaining * remaining / 255;
    int hue = (fireworkHue + elapsed * 40 / bloomTime) % 360;
    if(elapsed > bloomTime / 2){
      updateSparkle(60);
      if(!sparkleOn) level = level / 4;
    }
    int r = 0;
    int g = 0;
    int b = 0;
    hueToRGB(hue, r, g, b);
    showColor(r, g, b, level);
  }
  else{
    showColor(0, 0, 0, 0);
    if(elapsed >= fireworkPause){
      startFirework();
    }
  }
}

void registerTap(){
  unsigned long currentTime = millis();

  if(tapCount > 0 && currentTime - lastTapTime > maxTapGap){
    tapCount = 0;
  }

  if(tapCount > 0){
    tapIntervals[(tapCount - 1) % 3] = currentTime - lastTapTime;

    int used = tapCount;
    if(used > 3) used = 3;
    unsigned long total = 0;
    for(int i = 0; i < used; i++){
      total = total + tapIntervals[i];
    }
    beatInterval = total / used;
    if(beatInterval < minBeatInterval) beatInterval = minBeatInterval;
  }

  tapCount = tapCount + 1;
  lastTapTime = currentTime;
  beatStart = currentTime;
}

void updateTapTempo(){
  unsigned long elapsed = millis() - beatStart;
  unsigned long beatNumber = elapsed / beatInterval;
  unsigned long phase = elapsed % beatInterval;

  int colorIndex = 1 + beatNumber % (colorCount - 1);

  unsigned long fadeTime = beatInterval * 7 / 10;
  int level = 0;
  if(phase < fadeTime){
    long remaining = (long)(fadeTime - phase) * 255 / fadeTime;
    level = remaining * remaining / 255;
  }
  if(level < 15) level = 15;

  showTableColor(colorIndex, level);
}

void updateLEDColor(){
  if(currentMode == 0){
    showColor(RLightColor, GLightColor, BLightColor, 255);
  }
  else if(currentMode == 1){
    unsigned long currentTime = millis();
    if(currentTime - blinkTimer > blinkInterval){
      blinkOn = !blinkOn;
      blinkTimer = currentTime;
    }

    if(blinkOn){
      showColor(RLightColor, GLightColor, BLightColor, 255);
    }
    else{
      showColor(0, 0, 0, 0);
    }
  }
  else if(currentMode == 2){
    updateBreathing();
  }
  else if(currentMode == 3){
    updateRainbow();
  }
  else if(currentMode == 4){
    updateFirework();
  }
  else if(currentMode == 5){
    updateTapTempo();
  }
}