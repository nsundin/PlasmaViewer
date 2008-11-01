#ifndef __PLAYER_H__
#define __PLAYER_H__

class Player {
public:
	void SetStill();
	bool isMovingForward;
	bool isMovingBackward;
	bool isTurningRight;
	bool isTurningLeft;
	bool isMovingUp;
	bool isMovingDown;
	bool isRun;
};
#endif
