#define EMERGENCY_STOP_PIN 13 // GPIO pin connected to the NC contact

void setup() {
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
    Serial.begin(9600);
}

// Will use NC option so the in case of wire malfunction a lack of current
// will indicate E-stop
// Use of Internal pullup resistor will also lead to floating pins activating the E stop
void loop() {
    // Read the state of the emergency stop button
    int buttonState = digitalRead(EMERGENCY_STOP_PIN);
    
    if (buttonState == HIGH) {
        Serial.println("Emergency Stop Activated!");
        // TODO: add stop logic
    } else {
        Serial.println("System Running Normally");
    }
    
    delay(100); // Small delay for stability
}
