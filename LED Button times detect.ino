const int buttonPin = 2;   // the number of the pushbutton pin
const int RledPin = 9;     // the number of the R pin
const int GledPin = 10;     // the number of the G pin
const int BledPin = 11;     // the number of the B pin

// variables will change:
int buttonState = 0;   // variable for reading the pushbutton status
int ledState = LOW;
bool ButtonPressed = false;

unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long previousMillis2 = 0; // will store last time LED was updated
const long interval = 1000; // interval at which to blink (milliseconds)
const long Gametime = 5000;
const long result = 3000;
bool Gamestart = false;
int hits = 0;
int color = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();
  buttonState = digitalRead(buttonPin);

  if (Gamestart == false){
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      }
      else {
        ledState = LOW;
      }
    }
  if (color == 1){
    if (ledState == LOW) {
      digitalWrite(RledPin, LOW);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
    }
    else {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
    }
  }
  else if (color == 2){
    if (ledState == LOW) {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, LOW);
    }
    else {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
    }
  }
  else if (color == 3){
    if (ledState == LOW) {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, LOW);
      digitalWrite(BledPin, HIGH);
    }
    else {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
    }
  }
  else if (color == 0){
    if (ledState == LOW) {
      digitalWrite(RledPin, LOW);
      digitalWrite(GledPin, LOW);
      digitalWrite(BledPin, LOW);
    }
    else {
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
    }

    
  }
  if(buttonState == LOW && !ButtonPressed) {
    ButtonPressed = true;
  }
  else if(buttonState == HIGH && ButtonPressed){
    ButtonPressed = false;
    previousMillis = currentMillis;
    hits=0;
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, HIGH);
    Gamestart = true;
  }
  }
  else if (Gamestart == true) {
    if (currentMillis - previousMillis >= Gametime) {
      if (currentMillis2 - previousMillis2 >= result ) {
        Gamestart = false ;
      }
      else{
        digitalWrite(RledPin, LOW);
        digitalWrite(GledPin, LOW);
        digitalWrite(BledPin, LOW);
        if (hits <= 10){
          color = 1 ;
        }
        else if (10 < hits && hits <= 20){
          color = 2 ;
        }
        else if (hits >20){
          color = 3 ;
        }
      }
    }
    else {
      //Serial.println("gaming_else");
      previousMillis2 = currentMillis2;
      if(buttonState == LOW && ButtonPressed == false) {
        ButtonPressed = true;
        hits = hits + 1 ;
        Serial.println(hits);
        digitalWrite(RledPin, LOW);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
      else if (buttonState == HIGH && ButtonPressed == true){
        ButtonPressed = false;
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    }
  }
}