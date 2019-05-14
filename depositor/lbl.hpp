#ifndef DEVICE_LBL_H
#define DEVICE_LBL_H

#define MAX_SESSION_SIZE 128

#define STAGE_FILL 0
#define STAGE_WASH 1
#define STAGE_DRY 2

enum State {
  IDLE,
  READ,
  WAIT,
  EXEC
};

typedef struct {
	int type;
	int pump;
	int dur;
} Stage;

void resetStage( Stage& s );

#endif
