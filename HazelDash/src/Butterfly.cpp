#include "Butterfly.h"
#include "Level.h"

static std::vector<std::pair<int, int>> s_Directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1} };

Butterfly::Butterfly()
: m_Animation({Tile::Butterfly0, Tile::Butterfly1, Tile::Butterfly2, Tile::Butterfly3, Tile::Butterfly4, Tile::Butterfly5, Tile::Butterfly6, Tile::Butterfly7})
, m_Frame(0)
, m_AnimationTimeStep(1.0f / 25.0f)
, m_AnimationAccumulator(0.0f)
, m_LastMove(0)
{}


void Butterfly::FixedUpdate(size_t row, size_t col, Level& level) {
	
	for (auto [rowOffset, colOffset] : s_Directions) {
		if (level.GetGameObject(row + rowOffset, col + colOffset).IsPlayer()) {
			level.Explode(row, col);
			return;
		}
	}

	// Butterflies always try to turn right first, otherwise keep going straight.
	// If that doesn't work, then try turn left on next turn
	int thisMove = (m_LastMove + 1) % 4;
	auto [rowOffset, colOffset] = s_Directions[thisMove];

	if (level.GetGameObject(row + rowOffset, col + colOffset).IsEmpty()) {
		level.SwapObjects(row + rowOffset, col + colOffset, row, col);
		level.SetUpdated(row + rowOffset, col + colOffset, true);
		m_LastMove = thisMove;
	} else {
		auto [rowOffset, colOffset] = s_Directions[m_LastMove];
		if (level.GetGameObject(row + rowOffset, col + colOffset).IsEmpty()) {
			level.SwapObjects(row + rowOffset, col + colOffset, row, col);
			level.SetUpdated(row + rowOffset, col + colOffset, true);
		} else {
			m_LastMove = (m_LastMove + 2) % 4;
		}
	}
	level.SetUpdated(row, col, true);
}


void Butterfly::Animate(Hazel::Timestep ts) {
	m_AnimationAccumulator += ts;

	if (m_AnimationAccumulator >= m_AnimationTimeStep) {
		++m_Frame;
		if (m_Frame >= m_Animation.size()) {
			m_Frame = 0;
		}
		m_AnimationAccumulator = 0.0;
	}
}


Tile Butterfly::GetTile() const {
	return m_Animation[m_Frame];
}
