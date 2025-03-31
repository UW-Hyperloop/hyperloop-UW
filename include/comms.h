static const int MSG_START = 0x02;
static const int MSG_END = 0x03;
static const int MSG_SIZE = 515;

extern HardwareSerial CH9121;

typedef enum {
    NO_MESSAGE = 0x30,
    TBM_INIT = 0x31,
    TBM_START = 0x32,
    TBM_STOP = 0x33,
    TBM_ERROR = 0x34,
    TBM_DATA = 0x35,
    TBM_ACK = 0x36,
} MessageID;

extern MessageID incomingMessage;

uint8_t* tbm_init();
void tbm_start_stop();
uint8_t* tbm_error();
uint8_t* tbm_data();
uint8_t* tbm_estop();
uint8_t* tbm_estop_start();
bool tbm_ack();