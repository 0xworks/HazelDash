#include "Enemy.h"

#include "Level.h"
#include "Components/EnemyMovement.h"

#include "Hazel/Scene/Components.h"


Enemy::Enemy(Hazel::Entity entity, float fixedTimeStep)
: NativeScript{entity}
, m_FixedTimestep{fixedTimeStep}
{}


void Enemy::OnUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	static std::array<std::pair<int, int>, 4> Directions{
		std::pair<int, int>{-1,  0},
		std::pair<int, int>{ 0, -1},
		std::pair<int, int>{ 1,  0},
		std::pair<int, int>{ 0,  1}
	};

	m_FixedUpdateAccumulatedTs += ts;
	if (m_FixedUpdateAccumulatedTs > m_FixedTimestep) {
		Level* level = Level::Get();
		auto& transformComponent = GetComponent<Hazel::TransformComponent>();
		auto& movement = GetComponent<EnemyMovement>();

		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);

		// If next to player, then explode (and do not move)
		bool move = true;
		for (auto direction : Directions) {
			if (IsPlayer(level->GetEntity(row + direction.first, col + direction.second).GetComponent<Tile>())) {
				level->OnExplode(row, col);
				move = false;
				break;
			}
		}
		if (move) {
			// try to turn in preferred direction
			// if that is not possible, go straight ahead
			// if that is not possible either, then don't move, but change to opposite direction for next frame
			int direction = (movement.Direction + (movement.PreferLeftTurn ? -1 : 1)) % Directions.size();
			int preferredRow = row + Directions[direction].first;
			int preferredCol = col + Directions[direction].second;
			if (IsEmpty(level->GetEntity(preferredRow, preferredCol).GetComponent<Tile>())) {
				level->SwapEntities(row, col, preferredRow, preferredCol);
				transformComponent.Translation += glm::vec3{Directions[direction].second, Directions[direction].first, 0.0f};
				movement.Direction = direction;
			} else {
				int straightRow = row + Directions[movement.Direction].first;
				int straightCol = col + Directions[movement.Direction].second;
				if (IsEmpty(level->GetEntity(straightRow, straightCol).GetComponent<Tile>())) {
					level->SwapEntities(row, col, straightRow, straightCol);
					transformComponent.Translation += glm::vec3{Directions[movement.Direction].second, Directions[movement.Direction].first, 0.0f};
				} else {
					movement.Direction = (movement.Direction + 2) % Directions.size();
				}
			}
		}
		m_FixedUpdateAccumulatedTs = 0.0f;
	}
}
