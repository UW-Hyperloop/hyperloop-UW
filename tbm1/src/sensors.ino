// Thermocouple code? - def has bugs and errors, but seems to be a good starting point 

const int LED_out = 5; // what is the pin we use ? 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_out, OUTPUT);

}

void loop() {
#define TC_PIN LED_out     // ADC pin being used?
#define AREF 3.3           // voltage for esp32
#define ADC_RESOLUTION 12 // Is the resolution 12-bit on the ESP32?  

float reading, voltage, temperature;

float get_voltage(int raw_adc) {
  return raw_adc * (AREF / (pow(2, ADC_RESOLUTION)-1));  
}

float get_temperature(float voltage) {
  return (voltage - 1.25) / 0.005;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  reading = analogRead(TC_PIN);
  voltage = get_voltage(reading);
  temperature = get_temperature(voltage);
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" C");
  delay(500);
}
}
