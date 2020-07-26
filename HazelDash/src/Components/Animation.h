#pragma once
#include "Components/Tile.h"

#include <vector>

struct Animation {
	std::vector<Tile> Frames;
	uint32_t CurrentFrame = 0;
};
