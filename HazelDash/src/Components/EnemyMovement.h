#pragma once

#include "Components/Position.h"

struct EnemyMovement {
	int Direction;          // index into Directions array of EnemyController
	bool PreferLeftTurn;
};
