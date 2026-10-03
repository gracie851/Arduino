const int buttonPin = 2;
const int R = 9;
const int G = 10;
const int B = 11;

int buttonState = 1;
int ledcolor = 0;
bool buttonPressed = false;
String currentcolor = "LED off";

//Variables for counting clicks in a time interval
unsigned long clickStartTime = 0;
const long clickWindow = 1500; 
int clickCount = 0;
bool isCounting = false;

void setup() {
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buttonPin, INPUT); 
  Serial.begin(9600);
  
  Serial.println("Type a color (e.g., 'red') or click the button multiple times!");
  setLEDColor(); 
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
    delay(50); // Debounce to prevent false double-clicks
    
    if (!isCounting) {
      isCounting = true; // Start the timer on the first click
      clickStartTime = millis();
      clickCount = 0;
    }
    clickCount++;
  } 
  
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
    delay(50); 
  }

  // Apply the color when the click time interval finishes
  if (isCounting && (millis() - clickStartTime >= clickWindow)) {
    isCounting = false; // Stop counting
    
    // Assign color based on clicks 
    ledcolor = clickCount % 8; // % 8: wraps back to 0 if click more than 7 times
    setLEDColor();
  }
}

void setLEDColor() {
  if (ledcolor == 0) {
    currentcolor = "LED off";
    digitalWrite(R, HIGH); 
    digitalWrite(G, HIGH); 
    digitalWrite(B, HIGH); 
  }
  else if (ledcolor == 1) {
    currentcolor = "Red";
    digitalWrite(R, LOW); 
    digitalWrite(G, HIGH); 
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 2) {
    currentcolor = "Green";
    digitalWrite(R, HIGH); 
    digitalWrite(G, LOW); 
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 3) {
    currentcolor = "Blue";
    digitalWrite(R, HIGH); 
    digitalWrite(G, HIGH); 
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 4) {
    currentcolor = "Yellow";
    digitalWrite(R, LOW); 
    digitalWrite(G, LOW); 
    digitalWrite(B, HIGH);
  }
  else if (ledcolor == 5) {
    currentcolor = "Purple";
    digitalWrite(R, LOW); 
    digitalWrite(G, HIGH); 
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 6) {
    currentcolor = "Cyan";
    digitalWrite(R, HIGH); 
    digitalWrite(G, LOW); 
    digitalWrite(B, LOW);
  }
  else if (ledcolor == 7) {
    currentcolor = "White";
    digitalWrite(R, LOW); 
    digitalWrite(G, LOW); 
    digitalWrite(B, LOW);
  }

  Serial.print("Current color: ");
  Serial.println(currentcolor);
}