#include "Explosion.h"
#include "Diamond.h"
#include "Level.h"

Explosion::Explosion(Tile explodeTo)
: m_Frame(0)
, m_ExplodeTo(explodeTo)
{
	if (explodeTo == Tile::Diamond0) {
		m_Animation = {Tile::DiamondExplode1, Tile::DiamondExplode2, Tile::DiamondExplode3, Tile::DiamondExplode4};
	} else {
		m_Animation = {Tile::Explode1, Tile::Explode2, Tile::Explode1, Tile::Explode0};
	}
}


void Explosion::FixedUpdate(size_t row, size_t col, Level& level) {
	if (++m_Frame >= m_Animation.size()) {
		if (m_ExplodeTo == Tile::Diamond0) {
			level.SetGameObject(row, col, std::make_unique<Diamond>());
		} else {
			level.SetGameObject(row, col, std::make_unique<GameObject>(Tile::Empty));
		}
	}
	level.SetUpdated(row, col, true);
}


Tile Explosion::GetTile() const {
	return m_Animation[m_Frame];
}
