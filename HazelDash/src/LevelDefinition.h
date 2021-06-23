#pragma once

#include "Components/Animation.h"
#include "Components/Roll.h"
#include "Components/Tile.h"

#include <glm/glm.hpp>

#include <string>

struct LevelDefinition {
	int Width;
	int Height;
	int ScoreRequired;
	int TimeAllowed;
	std::string LevelData;
};

extern std::vector<LevelDefinition> s_LevelDefinition;

// convert characters in level definition LevelData member to various components...
Tile CharToTile(const char ch);
Animation CharToAnimation(const char ch);
glm::vec4 CharToColor(const char ch);
Roll CharToRoll(const char ch);
