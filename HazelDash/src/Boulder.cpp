#include "Boulder.h"
#include "Level.h"

Boulder::Boulder()
: GameObject(Tile::Boulder)
, m_State(State::Stationary)
{}


void Boulder::FixedUpdate(size_t row, size_t col, Level& level) {
	GameObject& objectBelow = level.GetGameObject(row - 1, col);
	if (objectBelow.IsSolid()) {
		if (m_State == State::Falling && objectBelow.IsExplosive()) {
			level.Explode(row - 1, col);
		} else {
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

#ifdef _DEBUG
Tile Boulder::GetTile() const {
	if (m_State == State::Falling) {
		return Tile::Spare0;
	}
	return __super::GetTile();
}
#endif
