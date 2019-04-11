#ifndef DEPOSITION_HPP
#define DEPOSITION_HPP

#define MAX_STAGES 32
#define MAX_CMD_LENGTH 16

#define END 0
#define FILL 1
#define WASH 2
#define DRY 3

#define PUMP_1 4
#define PUMP_2 5
#define PUMP_3 6
#define PUMP_W 7
#define PUMP_D 8
#define VALVE_D 9

#define SESSION_RECVD 0
#define NOTHING_RECVD 1
#define HANDSHAKE_RECVD 2

#define PARSE_SUCCESS 0
#define PARSE_ERROR -1

typedef struct {
    int type;
    int pump;
    int dur;
} Stage;

enum State {
    IDLE,
    READ,
    EXEC
};

#endif
