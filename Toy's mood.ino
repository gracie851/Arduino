const int R = 9;
const int G = 10;
const int B = 11;
const int buttonPin = 2;

int mood = 0;
const int neutralMood = 10;
int buttonState = 0;   
bool ButtonPressed = false;
unsigned long touchedTimer = 0;
unsigned long reducedTimer = 0;
const long unTouchInterval = 5000;
const long reducedInterval = 1000;


void setup() {
pinMode(buttonPin, INPUT);
pinMode(R, OUTPUT);
pinMode(G, OUTPUT);
pinMode(B, OUTPUT);

mood = neutralMood;
}


void loop() {

showLEDState (mood);

buttonState = digitalRead(buttonPin);

//if the button is clicked
if (buttonState == LOW && !ButtonPressed) {
    mood = mood + 1;
    if(mood > 20) mood = 20;
    touchedTimer = millis();
    ButtonPressed = true;
}

if (buttonState == HIGH && ButtonPressed) {
    ButtonPressed = false;
}

unsigned long currentTimer = millis();
if (currentTimer - touchedTimer > unTouchInterval){
  if (currentTimer - reducedTimer > reducedInterval){
     mood = mood - 1;
    if(mood < 0) mood = 0;
    reducedTimer = currentTimer;

  }
}

}

void showLEDState (int state){
float brightnessInterval = 255 / 10.0;
if (state >= neutralMood) {
analogWrite(R, 255);
analogWrite(G, brightnessInterval * (state - neutralMood));
analogWrite(B, 255 - brightnessInterval * (state - neutralMood));
}
else{
analogWrite(R, 255 - brightnessInterval * (neutralMood - state));
analogWrite(G, brightnessInterval * (neutralMood - state));
analogWrite(B, 255);
}

}