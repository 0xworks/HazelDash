#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Boulder : public GameObject {
public:
	Boulder();

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
	std::vector<Tile> m_Animation;
	int m_Frame;
	State m_State;
};
