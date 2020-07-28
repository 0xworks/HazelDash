#pragma once

enum class MassState {
	Stationary = 0,
	Falling
};

struct Mass {
	MassState State = MassState::Stationary;
	int HeightFallen = 0;
	int FallLimit = INT_MAX;
};
