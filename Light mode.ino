//The Pin of the push button
const int buttonPin = 2;

//The Pin of the RGB LED Module
const int RLedPin = 9;
const int GLedPin = 10;
const int BLedPin = 11;

//The current Push button state
int buttonState = 0;
//The variable used to memorize whether the user have pressed the button د
bool buttonPressed = false;
//The timer used to count the pressing time 
unsigned long pressingTime = 0;
//The threshold of the long press interval
const int longPressInterval = 1500;

//The Current Light Stick number
//0: white, 1:red, 2: blue, 3: green, 4: yellow....
int lightNum = 0;
//The Current Light Stick Color
int RLightColor = 0;
int GLightColor = 0;
int BLightColor = 0;

int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

//Light Stick Current Mode
//Mode 0: Constant Light Mode
//Mode 1: Blink Mode
//Mode 2: Breathing Light Mode
int currentMode = 0;
//Variables for blink mode
//timer for record the last time toggle blink effect 
unsigned long blinkTimer = 0;
//the blinking time interval const 
int blinkInterval = 500;
//used to memorize whether the LED is light on/off 
bool blinkOn = true;

//Variables for fading mode
//the fading amount per time step
const int fadeAmount = 2;
//used to memorize the fading direction
int fadeDirection = 1;

void setup()
{
  pinMode(buttonPin, INPUT);
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);
}

void loop()
{
  checkButton();
  updateLEDColor();
  setRGBLEDColor (RCurrentColor, GCurrentColor, BCurrentColor);
}

void setRGBLEDColor(int r, int g, int b){
  analogWrite(RLedPin, r);
  analogWrite(GLedPin, g);
  analogWrite(BLedPin, b);
}

void checkButton(){
//Read the button state.
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && !buttonPressed) {
    pressingTime = millis();
    buttonPressed = true;
  }

  if(buttonState == LOW && buttonPressed) {
    unsigned long currentTime = millis();
    if(currentTime - pressingTime < longPressInterval) {
      //short click
      changeLEDColor();
    }

  else{
    //long press
    changeMode();
    
  }
  buttonPressed = false;
  }

}

void changeLEDColor(){
//Increase the lignt number
lightNum = lightNum + 1;
if(lightNum >= 5)
  lightNum = 0;

//Set the RGB Light Color according to the light number
if(lightNum == 0){
  //white
  RLightColor = 0;
  GLightColor = 0;
  BLightColor = 0;
}

if(lightNum == 1){
  //white
  RLightColor = 0;
  GLightColor = 225;
  BLightColor = 225;
}

if(lightNum == 2){
  //white
  RLightColor = 225;
  GLightColor = 0;
  BLightColor = 225;
}

if(lightNum == 3){
  //white
  RLightColor = 225;
  GLightColor = 225;
  BLightColor = 0;
}

if(lightNum == 4){
  //white
  RLightColor = 0;
  GLightColor = 0;
  BLightColor = 225;
}
  RCurrentColor = RLightColor;
  GCurrentColor = GLightColor;
  BCurrentColor = BLightColor;

}

void changeMode() {
currentMode = currentMode + 1;
if(currentMode >= 3) {
  currentMode = 0;
}

//Reset variables for blinking effects
if (currentMode == 1) {
  blinkTimer = 0;
  blinkOn = true;
}

if (currentMode == 2){
  fadeDirection = 1;
}
}

void updateLEDColor(){
if(currentMode == 0) {
  //Constant Light Mode
  RCurrentColor = RLightColor;
  GCurrentColor = GLightColor;
  BCurrentColor = BLightColor;
}

else if(currentMode == 1){
//Blink Mode
  unsigned long currentTime = millis();
  if (currentTime - blinkTimer > blinkInterval) {
    blinkOn = !blinkOn;
    blinkTimer = currentTime;
  }

  if(blinkOn) {
  RCurrentColor = RLightColor;
  GCurrentColor = GLightColor;
  BCurrentColor = BLightColor;
  }
  else{
  RCurrentColor = 255;
  GCurrentColor = 255;
  BCurrentColor = 255;
  }
}

  else if (currentMode == 2){
  //Breathing Light Mode
  if (RLightColor > 0)
    RCurrentColor = RCurrentColor + fadeDirection * fadeAmount; 
  if(GLightColor > 0)
    GCurrentColor = GCurrentColor + fadeDirection * fadeAmount; 
  if (BLightColor > 0)
    BCurrentColor = BCurrentColor + fadeDirection * fadeAmount;

  if (RCurrentColor > RLightColor || GCurrentColor > GLightColor || BCurrentColor > BLightColor){
    fadeDirection = -fadeDirection;
  }
  else if (RCurrentColor < 0 || GCurrentColor < 0 || BCurrentColor < 0){ 
    fadeDirection = -fadeDirection;
  }
  }
}
