#include "Physics.h"

#include "Level.h"
#include "Components/Mass.h"
#include "Components/Roll.h"

#include "Hazel/Scene/Components.h"

Physics::Physics(Hazel::Entity entity, float fixedTimeStep)
: NativeScript{entity}
, m_FixedTimestep{fixedTimeStep}
{}


void Physics::OnUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();
	// Note: To get the behaviour of the original BD game, the "physics" system must evaluate
	//       the entities in level top-down order.
	//       However, the ECS will not do that.
	//       Instead, entities will just be iterated in whatever order they happen to be stored in the
	//       underlying data structures (and to try and iterate them in any other order kind of defeats
	//       the purpose of the ECS in the first place (which is to iterate in a cache-friendly way))
	//       EnTT does allow for sorting of components (i.e. sort them first, and then iterate them in order)
	//       So that is worth investigating.
	//       However, for now I am just going to ignore it, and iterate the entities in the order that the ECS
	//       has them.

	static const std::pair<int, int> Below = {-1, 0};
	static const std::pair<int, int> Left = {0, -1};
	static const std::pair<int, int> BelowLeft = {-1, -1};
	static const std::pair<int, int> Right = {0, 1};
	static const std::pair<int, int> BelowRight = {-1, 1};

	m_FixedUpdateAccumulatedTs += ts;
	if (m_FixedUpdateAccumulatedTs > m_FixedTimestep) {
		Level* level = Level::Get();

		auto& mass = GetComponent<Mass>();
		auto& transformComponent = GetComponent<Hazel::TransformComponent>();

		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);

		Hazel::Entity entityBelow = level->GetEntity(row + Below.first, col + Below.second);
		auto tileBelow = entityBelow.GetComponent<Tile>();
		if (IsEmpty(tileBelow)) {
			mass.State = MassState::Falling;
			++mass.HeightFallen;
			level->SwapEntities(row, col, row + Below.first, col + Below.second);
			transformComponent.Translation += glm::vec3{Below.second, Below.first, 0.0f};
		} else {
			if ((mass.State == MassState::Falling) && IsExplosive(tileBelow)) {
				Level::Get()->OnExplode(row + Below.first, col + Below.second);
			} else if (mass.HeightFallen > mass.FallLimit) {
				level->OnExplode(row, col);
			} else {
				if (IsRounded(tileBelow)) {
					Hazel::Entity entityLeft = level->GetEntity(row + Left.first, col + Left.second);
					Hazel::Entity entityBelowLeft = level->GetEntity(row + BelowLeft.first, row + BelowLeft.second);
					auto tileLeft = entityLeft.GetComponent<Tile>();
					auto tileBelowLeft = entityBelowLeft.GetComponent<Tile>();
					if (IsEmpty(tileLeft) && IsEmpty(tileBelowLeft)) {
						// bounce left
						mass.State = MassState::Falling;
						level->SwapEntities(row, col, row + Left.first, col + Left.second);
						transformComponent.Translation += glm::vec3{Left.second, Left.first, 0.0f};
						if (HasComponent<Roll>()) {
							auto& roll = GetComponent<Roll>();
							auto& tile = GetComponent<Tile>();
							roll.CurrentFrame = (roll.CurrentFrame - 1) % roll.Frames.size();
							tile = roll.Frames[roll.CurrentFrame];
						}
					} else {
						Hazel::Entity entityRight = level->GetEntity(row + Right.first, col + Right.second);
						Hazel::Entity entityBelowRight = level->GetEntity(row + BelowRight.first, col + BelowRight.second);
						auto tileRight = entityRight.GetComponent<Tile>();
						auto tileBelowRight = entityBelowRight.GetComponent<Tile>();
						if (IsEmpty(tileRight) && IsEmpty(tileBelowRight)) {
							// bounce right
							mass.State = MassState::Falling;
							level->SwapEntities(row, col, row + Right.first, col + Right.second);
							transformComponent.Translation += glm::vec3{Right.second, Right.first, 0.0f};
							if (HasComponent<Roll>()) {
								auto& roll = GetComponent<Roll>();
								auto& tile = GetComponent<Tile>();
								roll.CurrentFrame = (roll.CurrentFrame + 1) % roll.Frames.size();
								tile = roll.Frames[roll.CurrentFrame];
							}
						} else {
							mass.State = MassState::Stationary;
							mass.HeightFallen = 0;
						}
					}
				} else {
					mass.State = MassState::Stationary;
					mass.HeightFallen = 0;
				}
			}
		}
		m_FixedUpdateAccumulatedTs = 0.0f;
	}
}
