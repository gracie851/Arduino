const int buttonPin = 2;
const int ledPin    = 3;

// Common-anode RGB LED: LOW = ON, HIGH = OFF
const int LED_ON  = LOW;
const int LED_OFF = HIGH;

// Change to LOW if your button reads LOW when pressed
const int PRESSED = HIGH;

// Mode 0 steady, 1 slow, 2 medium, 3 fast (ms)
const unsigned long blinkInterval[] = {0, 1000, 500, 150};

int mode = 0;

int buttonState;
int lastReading;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

bool ledOn = true;
unsigned long lastToggle = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  buttonState = digitalRead(buttonPin);
  lastReading = buttonState;

  digitalWrite(ledPin, LED_ON);       // start: steady ON
  Serial.println("Mode 0: Steady ON");
}

void loop() {
  // 1. Read button with debounce
  int reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay && reading != buttonState) {
    buttonState = reading;

    if (buttonState == PRESSED) {
      mode = (mode + 1) % 4;          // 0 -> 1 -> 2 -> 3 -> 0

      ledOn = true;
      digitalWrite(ledPin, LED_ON);
      lastToggle = millis();

      Serial.print("Mode ");
      Serial.println(mode);
    }
  }
  lastReading = reading;

  // 2. Control LED based on mode
  if (mode == 0) {
    digitalWrite(ledPin, LED_ON);     // steady ON
  } else if (millis() - lastToggle >= blinkInterval[mode]) {
    lastToggle = millis();
    ledOn = !ledOn;
    digitalWrite(ledPin, ledOn ? LED_ON : LED_OFF);
  }
}