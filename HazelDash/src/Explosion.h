#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Explosion: public GameObject {
public:
	Explosion(Tile explodeTo, bool isChainReaction);
	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;

	virtual Tile GetTile() const override;

private:
	std::vector<Tile> m_Animation;
	Tile m_ExplodeTo;
	int m_Frame;
	bool m_IsChainReaction;
};
