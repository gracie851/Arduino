int Photoresistor = 2;
int led = 9;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(Photoresistor, INPUT);
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int PhotoresistorState = analogRead(A0);
  if(PhotoresistorState > 200)
    digitalWrite(led, LOW);
  else
   digitalWrite(led, HIGH);
  // print out the value you read:
  Serial.println(PhotoresistorState);
  delay(1);  // delay in between reads for stability
}