const int buttonPin = 2;
const int R = 9;
const int G = 10;
const int B = 11;

int buttonState = 1;
int ledcolor = 0;
bool buttonPressed = false;
String currentcolor = "LED off";

// Variables for counting clicks
unsigned long clickStartTime = 0;
const long clickWindow = 1500;
int clickCount = 0;
bool isCounting = false;

// Track current color values for the gradient
float currentR = 255;
float currentG = 255;
float currentB = 255;

void setup() {
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use built-in pullup just in case
  Serial.begin(9600);
  
  // Set initial pins to OFF
  analogWrite(R, 255);
  analogWrite(G, 255);
  analogWrite(B, 255);
  
  Serial.println("Type a color (e.g., 'red') or click the button multiple times!");
}

void loop() {
  // Check for Serial Monitor Input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();          
    input.toLowerCase();   

    if (input == "off") ledcolor = 0;
    else if (input == "red") ledcolor = 1;
    else if (input == "green") ledcolor = 2;
    else if (input == "blue") ledcolor = 3;
    else if (input == "yellow") ledcolor = 4;
    else if (input == "purple") ledcolor = 5;
    else if (input == "cyan") ledcolor = 6;
    else if (input == "white") ledcolor = 7;
    
    setLEDColor();
  }

  // Check for Button Clicks
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    delay(50); // Debounce
    
    if (!isCounting) {
      isCounting = true;         
      clickStartTime = millis();
      clickCount = 0;
    }
    clickCount++;
  } 
  
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
    delay(50); // Debounce
  }

  // Apply color when interval finishes
  if (isCounting && (millis() - clickStartTime >= clickWindow)) {
    isCounting = false; 
    ledcolor = clickCount % 8; 
    setLEDColor();
  }
}

// Smoothly fade to target color
void fadeToColor(int targetR, int targetG, int targetB) {
  int steps = 100;       // How many steps in the gradient
  int stepDelay = 8;     // Delay per step in ms (100 * 5 = 500ms total fade time)
  
  // Calculate how much to change the color per step
  float diffR = (targetR - currentR) / steps;
  float diffG = (targetG - currentG) / steps;
  float diffB = (targetB - currentB) / steps;

  for (int i = 1; i <= steps; i++) {
    currentR += diffR;
    currentG += diffG;
    currentB += diffB;
    
    analogWrite(R, (int)currentR);
    analogWrite(G, (int)currentG);
    analogWrite(B, (int)currentB);
    delay(stepDelay);
  }
  
  // Snap exactly to target at the end to prevent rounding errors
  currentR = targetR;
  currentG = targetG;
  currentB = targetB;
  analogWrite(R, (int)currentR);
  analogWrite(G, (int)currentG);
  analogWrite(B, (int)currentB);
}

// Map ledcolor variable to RGB values
void setLEDColor() {
  if (ledcolor == 0) {
    currentcolor = "LED off";
    fadeToColor(255, 255, 255); // All OFF
  }
  else if (ledcolor == 1) {
    currentcolor = "Red";
    fadeToColor(0, 255, 255);   // R on, G/B off
  }
  else if (ledcolor == 2) {
    currentcolor = "Green";
    fadeToColor(255, 0, 255);
  }
  else if (ledcolor == 3) {
    currentcolor = "Blue";
    fadeToColor(255, 255, 0);
  }
  else if (ledcolor == 4) {
    currentcolor = "Yellow";
    fadeToColor(0, 0, 255); // Red + Green
  }
  else if (ledcolor == 5) {
    currentcolor = "Purple";
    fadeToColor(0, 255, 0);// Red + Blue
  }
  else if (ledcolor == 6) {
    currentcolor = "Cyan";
    fadeToColor(255, 0, 0); // Green + Blue
  }
  else if (ledcolor == 7) {
    currentcolor = "White";
    fadeToColor(0, 0, 0); // All ON
  }

  Serial.print("Current color: ");
  Serial.println(currentcolor);
}