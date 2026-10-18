const int RledPin = 9;
const int GledPin = 10;
const int BledPin = 11;

//the maximum brightness of the LE
int maxBrightness = 0;

//the current brightness and timer value for the LED
int currentBrightness = 0;
int fadingDirection = 1;
unsigned long previousMillis = 0;
int interval = 1000;

// the setup routine runs once when you press reset:

void setup() {
pinMode(RledPin, OUTPUT);
pinMode(GledPin, OUTPUT);
pinMode(BledPin, OUTPUT);

}
// the loop routine runs over and over again forever:

void loop(){
// read the input on analog pin 0:
int sensorValue = analogRead(A0);

//map the value from the sensor range to RGB LED output range
maxBrightness = map(sensorValue, 0, 1023, 0, 255);

//Control the LED fading brightness
unsigned long currentMillis = millis();
int fadingAmount = fadingDirection * map(currentMillis - previousMillis, 0, interval, 0, maxBrightness);
currentBrightness = currentBrightness + fadingAmount;

if (currentBrightness >= 255) {
currentBrightness = 255;
fadingDirection = -fadingDirection;
}