#ifndef TBM_H
#define TBM_H

#include <Arduino.h>
#include <sensors.h>
#include <jsonSerial.h>
#include <estop.h>
#include <states.h>
#include <comms.h>
#include <config.h>

// -------------------------------------------------------------------
//    SYSTEM DATA STRUCT
// -------------------------------------------------------------------
struct sys_json {
  TBMState     state;
  Sensor       motor_temp;
  Sensor       flow_temp;
  Sensor       flow_in;
  Sensor       flow_out;
  Sensor       motor_power;
  Sensor       pump_power;
  Sensor       bentonite_power;
  Sensor       estop_button;
  unsigned long global_time;
};

extern sys_json  systemData;
extern TBMState  currentState;


// -------------------------------------------------------------------
//    HELPER: Convert TBMState enum -> string for JSON, etc.
// -------------------------------------------------------------------
inline const char* stateToString(TBMState s) {
  switch(s) {
    case STATE_CONFIG:  return "config";
    case STATE_RUNNING: return "running";
    case STATE_ERROR:   return "error";
    case STATE_STOP:    return "stop";
    default:            return "unknown";
  }
}

#endif // TBM_H