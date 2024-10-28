#define MOTOR_TEMP_PIN 36
#define CIRCUIT_TEMP_PIN 34
// #define RPM_PIN 32
// #define PRESS_IN_PIN 25
// #define PRESS_OUT_PIN 27
#define FLOW_PIN 12

//#define MOTOR_OUT_PIN xx
//#define PUMP_OUT_PIN xx

#define SENSOR_API "/sensor"
#define CONFIG_API "/config"

#define STOP "stop"
#define START "start"
#define RUNNING "running"
#define ERROR "error"

struct Sensor {
    bool active;
    int value;   // can be an array if we need multiple values.
    signed int timestamp;
};

struct sys_json {
    char* state;     // one of the macros (14-17)
    Sensor motor_temp;
    Sensor circuit_temp;
    // Sensor rpm;
    // Sensor press_in;
    // Sensor press_out;
    Sensor flow;
    signed int global_time;
};
