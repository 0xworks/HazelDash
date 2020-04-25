#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Barrel : public GameObject {
public:

	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;

	virtual void IncreaseFrame() override;
	virtual void DecreaseFrame() override;

	virtual Tile GetTile() const override;

private:

	enum class State {
		Stationary = 0,
		Falling
	};

private:
	std::vector<Tile> m_Animation = {Tile::Barrel0, Tile::Barrel1, Tile::Barrel2, Tile::Barrel3};
	int m_Frame = 0;
	int m_FallenRows = 0;
	int m_FallLimit = 4;
	State m_State = State::Stationary;
};
