#include "Amoeba.h"
#include "Level.h"
#include "Random.h"

std::vector<std::pair<int, int>> Amoeba::Directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1} };

void Amoeba::FixedUpdate(size_t row, size_t col, Level& level) {

	for (auto [rowOffset, colOffset] : Directions) {
		if (level.GetGameObject(row + rowOffset, col + colOffset).IsPlayer()) {
			level.Explode(row, col);
			return;
		}
	}

	for (const auto& [rowOffset, colOffset] : Directions) {
		if (level.GetGameObject(row + rowOffset, col + colOffset).CanBeOccupied()) {
			if (Random::Uniform0_1() < m_GrowProbability) {
				level.GrowAmoeba(row + rowOffset, col + colOffset);
				break;
			}
		}
	}
 	level.SetUpdated(row, col, true);
}


void Amoeba::Animate(Hazel::Timestep ts) {
	m_AnimationAccumulator += ts;

	if (m_AnimationAccumulator >= m_AnimationTimeStep) {
		++m_Frame;
		if (m_Frame >= m_Animation.size()) {
			m_Frame = 0;
		}
		m_AnimationAccumulator = 0.0;
	}
}


Tile Amoeba::GetTile() const {
	return m_Animation[m_Frame];
}
