#ifndef TBM_H
#define TBM_H

#define RX 16
#define TX 17
#define ESTOPCTRL_PIN 18
#define MOTORCTRL_PIN 19
#define PUMPCTRL_PIN 21
#define BENTCTRL_PIN 25

#define MOTORSENSE_PIN 22
#define PUMPSENSE_PIN 23
#define ESTOPSENSE_PIN 33
#define MOTOR_TEMP_PIN 34
#define PUMP_TEMP_PIN 35
#define FLOW_IN_PIN 12
#define FLOW_OUT_PIN 13

#define SENSOR_API "/sensor"
#define CONFIG_API "/config"


const char* STOP = "stop";
const char* START = "start";
const char* RUNNING = "running";
const char* ERROR = "error";

typedef enum { 
    NO_MESSAGE = 0, 
    MESSAGE_TBM_INIT = 1,  
    TBM_START = 2,  
    TBM_STOP = 3,  
    TBM_ERROR = 4,  
    TBM_DATA = 5,  
} MessageID; 

MessageID incomingMessage = NO_MESSAGE; 
    
struct Sensor {
    bool active;
    int value;   // can be an array if we need multiple values.
    signed int timestamp;
};

struct sys_json {
    char* state;     // one of the state macros
    Sensor motor_temp;
    Sensor flow_temp;
    Sensor flow_in;
    Sensor flow_out;
    Sensor motor_power; // val should be 1 if there is power
    Sensor pump_power; // val should be 1 if there is power
    Sensor bentonite_power; // val should be 1 if there is power
    Sensor estop_button; // val should be 1 if e-stop is pressed
    signed int global_time;
};
#endif // TBM_H