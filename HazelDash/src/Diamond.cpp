#include "Diamond.h"
#include "HazelDashAudio.h"
#include "Level.h"

Diamond::Diamond()
: m_Animation({Tile::Diamond0, Tile::Diamond1, Tile::Diamond2, Tile::Diamond3, Tile::Diamond4, Tile::Diamond5, Tile::Diamond6, Tile::Diamond7})
, m_Frame(0)
, m_AnimationTimeStep(1.0f / 25.0f)
, m_AnimationAccumulator(0.0f)
, m_State(State::Stationary)
{}


// TODO: duplicates Boulder::FixedUpdate() - Could refactor into base "Fallable" game object?
//       (or better: Compose GameObjects, and have a "Fallable" behaviour)
void Diamond::FixedUpdate(size_t row, size_t col, Level& level) {
	GameObject& objectBelow = level.GetGameObject(row - 1, col);
	if (objectBelow.IsSolid()) {
		if (m_State == State::Falling && objectBelow.IsExplosive()) {
			level.Explode(row - 1, col);
		} else {
			if (m_State == State::Falling) {
				HazelDashAudio::PlaySound(SoundEffect::Diamond);
			}
			if (objectBelow.IsRounded()) {
				if (!level.GetGameObject(row, col - 1).IsSolid() && !level.GetGameObject(row - 1, col - 1).IsSolid()) {
					// bounce left
					m_State = State::Falling;
					level.SwapObjects(row, col - 1, row, col);
					level.SetUpdated(row, col - 1, true);
				} else if (!level.GetGameObject(row, col + 1).IsSolid() && !level.GetGameObject(row - 1, col + 1).IsSolid()) {
					// bounce right
					m_State = State::Falling;
					level.SwapObjects(row, col + 1, row, col);
					level.SetUpdated(row, col + 1, true);
				} else {
					m_State = State::Stationary;
				}
			} else {
				m_State = State::Stationary;
			}
		}
	} else {
		m_State = State::Falling;
		level.SwapObjects(row - 1, col, row, col);
		level.SetUpdated(row - 1, col, true);
	}
	level.SetUpdated(row, col, true);
}


void Diamond::Animate(Hazel::Timestep ts) {
	m_AnimationAccumulator += ts;

	if (m_AnimationAccumulator >= m_AnimationTimeStep) {
		++m_Frame;
		if (m_Frame >= m_Animation.size()) {
			m_Frame = 0;
		}
		m_AnimationAccumulator = 0.0;
	}
}


Tile Diamond::GetTile() const {
	return m_Animation[m_Frame];
}
