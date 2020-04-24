#pragma once

#include "GameObject.h"

class Boulder : public GameObject {
public:
	Boulder();

	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;

#ifdef _DEBUG
	virtual Tile GetTile() const override;
#endif

private:

	enum class State {
		Stationary = 0,
		Falling
	};

private:
	State m_State;
};
