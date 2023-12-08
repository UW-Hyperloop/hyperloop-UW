// The following is the control for tunnel boring machine 0. It involves interfacing with 2 motor drivers:
// one for the operation of left and right wheels, and one for the control of the cutterhead

// Wheel pins
int leftWheel1 = 2;
int leftWheel2 = 3;
int rightWheel1 = 4;
int rightWheel2 = 7;
int wheelENA = 5;
int wheelENB = 6;
// Cutterhead pins
int cut1 = 8;
int cut2 = 11;
int cutENA = 9;

void setup() {
  pinMode(leftWheel1, OUTPUT);
  pinMode(leftWheel2, OUTPUT);
  pinMode(rightWheel1, OUTPUT);
  pinMode(rightWheel2, OUTPUT);
  pinMode(wheelENA, OUTPUT);
  pinMode(wheelENB, OUTPUT);
  pinMode(cut1, OUTPUT);
  pinMode(cut2, OUTPUT);
  pinMode(cutENA, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    char direction = Serial.read();
    drive(direction);
  }
}

// can input l, r, f, b, c (cut), and s (stop cutterhead) through serial monitor
// if left and right wheels are going different directions, switch the wiring of one
void drive(char direction) {
  // right wheel goes faster when turning left
  if (direction == 'l') {
    Serial.println("left");
    digitalWrite(leftWheel1, HIGH);
    digitalWrite(leftWheel2, LOW);
    digitalWrite(rightWheel1, HIGH);
    digitalWrite(rightWheel2, LOW);
    analogWrite(wheelENA, 150);
    analogWrite(wheelENB, 255);
  }
  else if (direction == 'r') {
    Serial.println("right");
    digitalWrite(leftWheel1, HIGH);
    digitalWrite(leftWheel2, LOW);
    digitalWrite(rightWheel1, HIGH);
    digitalWrite(rightWheel2, LOW);
    analogWrite(wheelENA, 255);
    analogWrite(wheelENB, 150);
  }
  else if (direction == 'f') {
    Serial.println("forward");
    digitalWrite(leftWheel1, HIGH);
    digitalWrite(leftWheel2, LOW);
    digitalWrite(rightWheel1, HIGH);
    digitalWrite(rightWheel2, LOW);
    analogWrite(wheelENA, 255);
    analogWrite(wheelENB, 255);
  }
  else if (direction == 'b') {
    Serial.println("backward");
    digitalWrite(leftWheel1, LOW);
    digitalWrite(leftWheel2, HIGH);
    digitalWrite(rightWheel1, LOW);
    digitalWrite(rightWheel2, HIGH);
    analogWrite(wheelENA, 255);
    analogWrite(wheelENB, 255);
  }
  else if (direction == 'c') {
    Serial.println("cut");
    runCutterHead("right", 255);
  }
  else if (direction == 's') {
    Serial.println("stop cut");
    runCutterHead("right", 0);
  }
  // Drive for 500 ms then stop
  delay(500);
  analogWrite(wheelENA, 0);
  analogWrite(wheelENB, 0);
}

// direction should be a string, left or right
// speed should be between 0 and 255
void runCutterHead(String direction, int speed) {
  if (direction.equalsIgnoreCase("right")) {
    digitalWrite(cut1, HIGH);
    digitalWrite(cut2, LOW);
  }
  else if (direction.equalsIgnoreCase("left")) {
    digitalWrite(cut1, LOW);
    digitalWrite(cut2, HIGH);
  }
  analogWrite(cutENA, speed);
}

// Test/showcase cutterhead
void testCutterHead() {
  runCutterHead("left", 255);
  delay(1000);
  runCutterHead("left", 120);
  delay(1000);
  runCutterHead("right", 120);
  delay(1000);
  runCutterHead("right", 255);
  delay(1000);
  runCutterHead("right", 120);
  delay(1000);
  runCutterHead("left", 120);
  delay(1000);
}
