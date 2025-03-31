#include <tbm.h>
// ---------------------------------------------------------
//  Read all sensors -> update systemData fields
// ---------------------------------------------------------
void readSensors() {
  systemData.estop_button.timestamp    = millis();

  // 8. Gas sense
  // int gasSense = digitalRead(// gas sensor pin here)
  // systemData.gas_sensor.value          = 
  // systemData.gas_sensor.timestamp      = millis(); 

  // 9. Update global_time
  systemData.global_time = millis();
  receivingESP_loop();
}