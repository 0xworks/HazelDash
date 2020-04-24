#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Firefly: public GameObject {
public:
	Firefly();

	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;

	virtual void Animate(Hazel::Timestep ts) override;

	virtual Tile GetTile() const override;

private:
	std::vector<Tile> m_Animation;
	int m_Frame;

	float m_AnimationTimeStep;
	float m_AnimationAccumulator;  // would be better as Hazel::Timestep, but that doesnt currently support operator +=

	int m_LastMove;
};
