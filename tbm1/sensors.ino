#include "tbm.h"



#define SHUNT_RESISTOR 150.0 // Resistor value in ohms

#define AREF 3.3             // ADC reference voltage for ESP32
#define ADC_RESOLUTION 4095  // 12-bit ADC resolution

void sensorPinSetup() {
  Serial.begin(9600);
  pinMode(MOTOR_TEMP_PIN, INPUT); // Set the thermocouple pin as input
  pinMode(FLOW_IN_PIN, INPUT); // Set the thermocouple pin as input
}

float get_voltage(int raw_adc) {
  return (raw_adc * AREF) / ADC_RESOLUTION;;  
}

float get_temperature(float voltage) {
  // Temperature formula for thermocouple amplifier
  return (voltage - 1.25) / 0.005; // converts to °C (5 mV/°C) 
}

float get_flowRate(float voltage){
  // Flow rate formula for liquid flow meter LVB-15-A Liquid flow meter [3.5 .. 50 l/min]

  // first must convert voltage to current
  float current_mA = (voltage / SHUNT_RESISTOR) * 1000;
  // formula
  return 3.125 * (current_mA - 4);
}

void sensorDataReadLoop() {

  // Thermocoupler readings
  int thermo_reading = analogRead(MOTOR_TEMP_PIN);
  float thermo_voltage = get_voltage(thermo_reading);
  float temperature = get_temperature(thermo_voltage);
  SystemData.motor_temp.value = temperature;

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" C");

  int flow_reading = analogRead(FLOW_IN_PIN);
  float flow_voltage = get_voltage(flow_reading);
  float flow_rate = get_flowRate(flow_voltage);
  SystemData.flow_in.value = flow_rate;

  Serial.print("Flow Rate = ");
  Serial.print(flow_rate);
  Serial.println("L/min");
  delay(500);
}
