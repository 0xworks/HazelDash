#include "Boulder.h"
#include "Level.h"

Boulder::Boulder()
: m_Animation({Tile::Boulder0, Tile::Boulder1, Tile::Boulder2, Tile::Boulder3})
, m_Frame(0)
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
					DecreaseFrame();
				} else if (!level.GetGameObject(row, col + 1).IsSolid() && !level.GetGameObject(row - 1, col + 1).IsSolid()) {
					// bounce right
					m_State = State::Falling;
					level.SwapObjects(row, col + 1, row, col);
					level.SetUpdated(row, col + 1, true);
					IncreaseFrame();
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


void Boulder::IncreaseFrame() {
	m_Frame = (m_Frame + 1) % m_Animation.size();
}


void Boulder::DecreaseFrame() {
	m_Frame = (m_Frame - 1) % m_Animation.size();
}


Tile Boulder::GetTile() const {
	return m_Animation[m_Frame];
}
