#include "Barrel.h"
#include "Level.h"

void Barrel::FixedUpdate(size_t row, size_t col, Level& level) {
	GameObject& objectBelow = level.GetGameObject(row - 1, col);
	if (objectBelow.IsSolid()) {
		if (m_State == State::Falling && objectBelow.IsExplosive()) {
			level.SetGameObject(row, col, std::make_unique<GameObject>(Tile::Empty));
			level.Explode(row - 1, col);
		} else if(m_FallenRows >= m_FallLimit) {
			level.Explode(row, col);
		} else {
			if (objectBelow.IsRounded()) {
				if (!level.GetGameObject(row, col - 1).IsSolid() && !level.GetGameObject(row - 1, col - 1).IsSolid()) {
					// bounce left
					m_State = State::Falling;
					level.SwapObjects(row, col - 1, row, col);
					level.SetUpdated(row, col - 1, true);
					DecreaseFrame();
				} else if (!level.GetGameObject(row, col + 1).IsSolid() && !level.GetGameObject(row - 1, col + 1).IsSolid()) {
					// bounce right
					m_State = State::Falling;
					level.SwapObjects(row, col + 1, row, col);
					level.SetUpdated(row, col + 1, true);
					IncreaseFrame();
				} else {
					m_State = State::Stationary;
					m_FallenRows = 0;
				}
			} else {
				m_State = State::Stationary;
				m_FallenRows = 0;
			}
		}
	} else {
		m_State = State::Falling;
		++m_FallenRows;
		level.SwapObjects(row - 1, col, row, col);
		level.SetUpdated(row - 1, col, true);
	}
	level.SetUpdated(row, col, true);
}


void Barrel::IncreaseFrame() {
	m_Frame = (m_Frame + 1) % m_Animation.size();
}


void Barrel::DecreaseFrame() {
	m_Frame = (m_Frame - 1) % m_Animation.size();
}


Tile Barrel::GetTile() const {
	return m_Animation[m_Frame];
}
