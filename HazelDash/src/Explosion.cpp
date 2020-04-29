#include "Explosion.h"
#include "Diamond.h"
#include "Level.h"

Explosion::Explosion(Tile explodeTo, bool isChainReaction)
: m_Frame(0)
, m_ExplodeTo(explodeTo)
, m_IsChainReaction(isChainReaction)
{
	if (explodeTo == Tile::Diamond0) {
		m_Animation = {Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3,  Tile::ExplosionDiamond4, Tile::ExplosionDiamond5, Tile::ExplosionDiamond6, Tile::Diamond7};
	} else {
		m_Animation = {Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3, Tile::Explosion4, Tile::Explosion5, Tile::Explosion6, Tile::Explosion7};
	}
}


void Explosion::FixedUpdate(size_t row, size_t col, Level& level) {
	++m_Frame;
	if (m_Frame >= m_Animation.size()) {
		if (m_ExplodeTo == Tile::Diamond0) {
			level.SetGameObject(row, col, std::make_unique<Diamond>());
		} else {
			level.SetGameObject(row, col, std::make_unique<GameObject>(Tile::Empty));
		}
	} else if (m_IsChainReaction && (m_Frame == 1)) {
		level.Explode(row, col, m_ExplodeTo);
	}
	level.SetUpdated(row, col, true);
}


Tile Explosion::GetTile() const {
	return m_Animation[m_Frame];
}
