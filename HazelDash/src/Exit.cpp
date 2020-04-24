#include "Exit.h"
#include "Level.h"

Exit::Exit()
: m_Animation({Tile::SteelWall, Tile::Exit})
, m_Frame(0)
, m_AnimationTimeStep(1.0f / 8.0f)
, m_AnimationAccumulator(0.0f)
, m_State(State::Closed)
{}


// TODO: duplicates Boulder::FixedUpdate() - Could refactor into base "Fallable" game object?
//       (or better: Compose GameObjects, and have a "Fallable" behaviour)
void Exit::FixedUpdate(size_t row, size_t col, Level& level) {
	if (level.GetScore() >= level.GetScoreRequired()) {
		m_State = State::Open;
	}
	level.SetUpdated(row, col, true);
}


void Exit::Animate(Hazel::Timestep ts) {
	m_AnimationAccumulator += ts;
	if (m_AnimationAccumulator >= m_AnimationTimeStep) {
		if (m_State == State::Open) {
			++m_Frame;
			if (m_Frame >= m_Animation.size()) {
				m_Frame = 0;
			}
		}
		m_AnimationAccumulator = 0.0;
	}
}


Tile Exit::GetTile() const {
	return m_Animation[m_Frame];
}
