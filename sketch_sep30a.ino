const int buttonPin = 2;
const int R = 9;
const int G = 10;
const int B = 11;

int buttonState = 1;
int ledState = LOW;
int ledcolor = 0;

bool buttonPressed = false;
String currentcolor ="led";

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);
  Serial.print("Current color: ");
  Serial.println(currentcolor);

  if (buttonState == LOW && !buttonPressed) {
    // turn LED on:
    ledcolor = ledcolor + 1;
    buttonPressed = true;
    //delay(200);
  } 
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    currentMillis = previousMillis;
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }
  }

  }
  if (ledcolor == 0) {
    currentcolor="LED off";
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH); 
  }
  else if (ledcolor == 1) {
    currentcolor="Red";
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 2) {
    currentcolor="Green";
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 3) {
    currentcolor="Blue";
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 4) {
    currentcolor="Yellow";
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 5) {
    currentcolor="Purple";
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 6) {
    currentcolor="Cyan";
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 7) {
    currentcolor="White";
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 8) {
    ledcolor = 0;
  }
}
