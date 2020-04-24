#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Amoeba: public GameObject {
public:
	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;
	virtual void Animate(Hazel::Timestep ts) override;
	virtual Tile GetTile() const override;

	static std::vector<std::pair<int, int>> Directions;

private:
	std::vector<Tile> m_Animation = {Tile::Amoeba0, Tile::Amoeba1, Tile::Amoeba2, Tile::Amoeba3, Tile::Amoeba4, Tile::Amoeba5, Tile::Amoeba6, Tile::Amoeba7};
	int m_Frame = 0;

	float m_AnimationTimeStep = 1.0f / 25.0f;
	float m_AnimationAccumulator = 0.0f;  // would be better as Hazel::Timestep, but that doesnt currently support operator +=
	float m_GrowProbability = 0.005f;
};
