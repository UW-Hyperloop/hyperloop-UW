#include <Arduino.h>
#include <tbm.h>

extern const char* STOP;
extern const char* START;
extern const char* RUNNING;
extern const char* ERROR;

// -------------------------------------------------------------------
//    STATE MACHINE
// -------------------------------------------------------------------
enum TBMState {
  STATE_CONFIG,
  STATE_RUNNING,
  STATE_ERROR,
  STATE_STOP
};

void state_setup();
bool checkStopped();
void updateSystemState();
void state_loop();