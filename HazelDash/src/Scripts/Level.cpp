#include "Level.h"

#include "LevelDefinition.h"
#include "Random.h"
#include "Components/Amoeba.h"
#include "Components/Animation.h"
#include "Components/EnemyMovement.h"
#include "Components/Explosion.h"
#include "Components/Mass.h"
#include "Components/PlayerState.h"
#include "Components/Roll.h"
#include "Components/Tile.h"

#include "Scripts/PlayerController.h"

#include "Hazel/Scene/Components.h"


namespace std {

	template<>
	struct hash<std::pair<int, int>> {
		size_t operator()(const std::pair<int, int>& pr) const {
			std::hash<int> hasher;
			size_t hash = hasher(pr.first);
			hash ^= hasher(pr.second) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			return hash;
		}
	};

}


Level::Level(Hazel::Entity entity, int level, float fixedTimestep, float animationTimestep)
: NativeScript{entity}
, m_FixedTimestep{fixedTimestep}
, m_AnimationTimestep{animationTimestep}
{
	HZ_ASSERT(!sm_Instance, "Level instance already exists!");
	sm_Instance = this;

	const LevelDefinition& definition = s_LevelDefinition[level];

	m_Entities.clear();
	m_Width = definition.Width;
	m_Height = definition.Height;
	m_WonLevel = false;
	m_Score = 0;
	m_ScoreRequired = definition.ScoreRequired;

	m_EmptyEntity = CreateEntity();
	m_EmptyEntity.AddComponent<Tile>(Tile::Empty);

	m_Entities.resize(m_Width * m_Height);
	for (int row = 0; row < m_Height; ++row) {
		for (int col = 0; col < m_Width; ++col) {
			int charIndex = (m_Width * (m_Height - (row + 1))) + col;
			HZ_ASSERT(charIndex < definition.LevelData.size(), "insufficient levelData supplied");
			if (charIndex < definition.LevelData.size()) {
				char ch = definition.LevelData[charIndex];
				Tile tile = CharToTile(ch);
				int index = (m_Width * row) + col;
				auto entity = m_EmptyEntity;
				if (!IsEmpty(tile)) {
					entity = CreateEntity();
					if (IsAmoeba(tile)) {
						entity.AddComponent<Amoeba>();
					} else if (IsBoulder(tile) || IsDiamond(tile)) {
						entity.AddComponent<Mass>();
					} else if (IsBarrel(tile)) {
						entity.AddComponent<Mass>(MassState::Stationary, 0, 5);
					} else if (IsButterfly(tile)) {
						entity.AddComponent<EnemyMovement>(3, false);
					} else if (IsFirefly(tile)) {
						entity.AddComponent<EnemyMovement>(1, true);
					} else if (IsDoor(tile)) {
						m_ExitEntity = entity;
					} else if (IsPlayer(tile)) {
						entity
							.AddComponent<PlayerState>(PlayerState::Idle)
							.AddComponent<Hazel::NativeScriptComponent>()
							.GetComponent<Hazel::NativeScriptComponent>()
							.Bind<PlayerController>()
						;
						m_PlayerEntity = entity;
						m_PlayerIsAlive = true;
					}
					entity
						.AddComponent<Tile>(tile)
					   .AddComponent<Hazel::SpriteRendererComponent>(CharToColor(ch))
						.GetComponent<Hazel::TransformComponent>()
						.Translation = {col + 0.5f, row + 0.5f, 0.0f}
					;
					Animation animation = CharToAnimation(ch);
					if (!animation.Frames.empty()) {
						entity.AddComponent<Animation>(animation);
					}
					Roll roll = CharToRoll(ch);
					if (!roll.Frames.empty()) {
						entity.AddComponent<Roll>(roll);
					}
				}
				m_Entities[index] = entity;
			}
		}
	}
	HZ_ASSERT(m_PlayerEntity, "ERROR: level definition does not contain player start point!");
}


Level::~Level() {
	sm_Instance = nullptr;
}


void Level::OnUpdate(Hazel::Timestep ts) {
	m_FixedUpdateAccumulatedTs += ts;
	if (m_FixedUpdateAccumulatedTs > m_FixedTimestep) {
			//
			// Each of these functions is a "system" that operates on a set of game entities.
			// I envisage splitting these into separate scripts in the future
			//
			// These systems are updated on a fixed timestep
			PhysicsFixedUpdate();
			EnemiesFixedUpdate();
			AmoebaFixedUpdate();
			m_FixedUpdateAccumulatedTs = 0.0;
		}

	// These systems update as fast as they like
	ExploderUpdate(ts);
	AnimatorUpdate(ts);
}


void Level::PhysicsFixedUpdate() {
	HZ_PROFILE_FUNCTION();
	// Note: To get the behaviour of the original DB game, the "physics" system must evaluate
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

	for (auto&& [entityHandle, mass, transformComponent] : GetScene().m_Registry.group<Mass>(entt::get<Hazel::TransformComponent>).each()) {
		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);
		Hazel::Entity entity = {entityHandle, GetScene()};
		Hazel::Entity entityBelow = GetEntity(row + Below.first, col + Below.second);
		auto tileBelow = entityBelow.GetComponent<Tile>();
		if (IsEmpty(tileBelow)) {
			mass.State = MassState::Falling;
			++mass.HeightFallen;
			SwapEntities(row, col, row + Below.first, col + Below.second);
			transformComponent.Translation += glm::vec3{Below.second, Below.first, 0.0f};
		} else {
			if ((mass.State == MassState::Falling) && IsExplosive(tileBelow)) {
				OnExplode(row + Below.first, col + Below.second);
			} else if (mass.HeightFallen > mass.FallLimit) {
				OnExplode(row, col);
			} else {
				if (IsRounded(tileBelow)) {
					Hazel::Entity entityLeft = GetEntity(row + Left.first, col + Left.second);
					Hazel::Entity entityBelowLeft = GetEntity(row + BelowLeft.first, row + BelowLeft.second);
					auto tileLeft = entityLeft.GetComponent<Tile>();
					auto tileBelowLeft = entityBelowLeft.GetComponent<Tile>();
					if (IsEmpty(tileLeft) && IsEmpty(tileBelowLeft)) {
						// bounce left
						mass.State = MassState::Falling;
						SwapEntities(row, col, row + Left.first, col + Left.second);
						transformComponent.Translation += glm::vec3{Left.second, Left.first, 0.0f};
						if (entity.HasComponent<Roll>()) {
							auto& roll = entity.GetComponent<Roll>();
							auto& tile = entity.GetComponent<Tile>();
							roll.CurrentFrame = (roll.CurrentFrame - 1) % roll.Frames.size();
							tile = roll.Frames[roll.CurrentFrame];
						}
					} else {
						Hazel::Entity entityRight = GetEntity(row + Right.first, col + Right.second);
						Hazel::Entity entityBelowRight = GetEntity(row + BelowRight.first, col + BelowRight.second);
						auto tileRight = entityRight.GetComponent<Tile>();
						auto tileBelowRight = entityBelowRight.GetComponent<Tile>();
						if (IsEmpty(tileRight) && IsEmpty(tileBelowRight)) {
							// bounce right
							mass.State = MassState::Falling;
							SwapEntities(row, col, row + Right.first, col + Right.second);
							transformComponent.Translation += glm::vec3{Right.second, Right.first, 0.0f};
							if (entity.HasComponent<Roll>()) {
								auto& roll = entity.GetComponent<Roll>();
								auto& tile = entity.GetComponent<Tile>();
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
	}
}


void Level::EnemiesFixedUpdate() {
	HZ_PROFILE_FUNCTION();

	static std::array<std::pair<int, int>, 4> Directions{
		std::pair<int, int>{-1,  0},
		std::pair<int, int>{ 0, -1},
		std::pair<int, int>{ 1,  0},
		std::pair<int, int>{ 0,  1}
	};

	for (auto&& [entityHandle, movement, transformComponent] : GetScene().m_Registry.group<EnemyMovement>(entt::get<Hazel::TransformComponent>).each()) {
		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);

		// If next to player, then explode (and do not move)
		bool move = true;
		for (auto direction : Directions) {
			if (IsPlayer(GetEntity(row + direction.first, col + direction.second).GetComponent<Tile>())) {
				OnExplode(row, col);
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
			if (IsEmpty(GetEntity(preferredRow, preferredCol).GetComponent<Tile>())) {
				SwapEntities(row, col, preferredRow, preferredCol);
				transformComponent.Translation += glm::vec3{Directions[direction].second, Directions[direction].first, 0.0f};
				movement.Direction = direction;
			} else {
				int straightRow = row + Directions[movement.Direction].first;
				int straightCol = col + Directions[movement.Direction].second;
				if (IsEmpty(GetEntity(straightRow, straightCol).GetComponent<Tile>())) {
					SwapEntities(row, col, straightRow, straightCol);
					transformComponent.Translation += glm::vec3{Directions[movement.Direction].second, Directions[movement.Direction].first, 0.0f};
				} else {
					movement.Direction = (movement.Direction + 2) % Directions.size();
				}
			}
		}
	}
}


void Level::OnExplode(const int row, const int col) {
	HZ_PROFILE_FUNCTION();

	// TODO: placeholder code.  Should be done with "proper" events at some point

	static std::array<std::pair<int, int>, 9> Offsets = {
		std::pair{ 1, -1},
		std::pair{ 1,  0},
		std::pair{ 1,  1},
		std::pair{ 0, -1},
		std::pair{ 0,  0},
		std::pair{ 0,  1},
		std::pair{-1, -1},
		std::pair{-1,  0},
		std::pair{-1,  1}
	};

	static Animation animation1 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3, Tile::Explosion4, Tile::Explosion5, Tile::Explosion6, Tile::Explosion7}};
	static Animation animation2 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3,  Tile::ExplosionDiamond4, Tile::ExplosionDiamond5, Tile::ExplosionDiamond6, Tile::Diamond7}};

	auto tile = GetEntity(row, col).GetComponent<Tile>();
	if (IsPlayer(tile)) {
		OnPlayerDied();
	}
	bool explodeToDiamond = IsButterfly(tile);
	//
	// At this point, other systems are still iterating their entities,
	// so we can't go destroying anything just yet.
	// What we'll do here is create an explosion entities at appropriate
	// positions, then when the exploder system gets its Update, we will
	// wreak the destruction there.
	for (auto offset : Offsets) {
		if (!IsExplodable(GetEntity(row + offset.first, col + offset.second).GetComponent<Tile>())) {
			continue;
		}
		Hazel::Entity entity = CreateEntity();
		entity.GetComponent<Hazel::TransformComponent>().Translation = {col + offset.second + 0.5f, row + offset.first + 0.5f, 0.0f};
		entity.AddComponent<Hazel::SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});
		entity.AddComponent<Explosion>(Explosion::Ignite);
		if (explodeToDiamond) {
			entity.AddComponent<Animation>(animation2);
			entity.AddComponent<Tile>(animation2.Frames[0]);
		} else {
			entity.AddComponent<Animation>(animation1);
			entity.AddComponent<Tile>(animation1.Frames[0]);
		}
	}
}


void Level::AmoebaFixedUpdate() {
	HZ_PROFILE_FUNCTION();

	static const std::array<std::pair<int, int>, 4> Directions = {
		std::pair{-1,  0},
		std::pair{ 0, -1},
		std::pair{ 1,  0},
		std::pair{ 0,  1}
	};

	auto amoebas = GetScene().m_Registry.group<Amoeba>(entt::get<Hazel::TransformComponent>);

	m_AmoebaSize = static_cast<int>(amoebas.size());
	if (m_AmoebaSize >= 200) { // TODO: parameterize?
		OnSolidify(Tile::BoulderFirst);
	}

	m_AmoebaPotential = 0;
	std::unordered_set<std::pair<int, int>> growPositions;

	for (auto&& [entityHandle, amoeba, transformComponent] : amoebas.each()) {
		for (auto direction : Directions) {
			int row = static_cast<int>(transformComponent.Translation.y);
			int col = static_cast<int>(transformComponent.Translation.x);
			Hazel::Entity entityOther = GetEntity(row + direction.first, col + direction.second);
			auto tile = entityOther.GetComponent<Tile>();
			if (IsEmpty(tile) || tile == Tile::Dirt1) {
				++m_AmoebaPotential;
				if (Random::Uniform0_1() < amoeba.GrowthProbability) {
					growPositions.emplace(row + direction.first, col + direction.second);
				}
			} else if (IsExplosive(tile)) {
				OnExplode(row + direction.first, col + direction.second);
			}
			amoeba.GrowthProbability *= 1.0f + static_cast<float>(amoebas.size()) / 200000.0f;
		}
	}

	if (m_AmoebaPotential == 0) {
		OnSolidify(Tile::Diamond0);
	} else {
		for (auto pos : growPositions) {
			Hazel::Entity entity = GetEntity(pos.first, pos.second);
			auto& tileInitial = entity.GetComponent<Tile>();
			if (IsEmpty(tileInitial)) {
				entity = CreateEntity();
				entity.AddComponent<Tile>(tileInitial);
				entity.AddComponent<Hazel::SpriteRendererComponent>();
				entity.GetComponent<Hazel::TransformComponent>().Translation = {pos.second + 0.5f, pos.first + 0.5f, 0.0f};
				SetEntity(pos.first, pos.second, entity);
			}
			entity.AddComponent<Amoeba>();
			const Animation& animation = CharToAnimation('A');
			entity.AddComponent<Animation>(animation);
			auto& tile = entity.GetComponent<Tile>();
			tile = animation.Frames[animation.CurrentFrame]; // TODO: it would be nicer to use EnTT "short circuit" to automatically set the tile when Animation component is added
			entity.GetComponent<Hazel::SpriteRendererComponent>().Color = CharToColor('A');
		}
	}
}


void Level::OnSolidify(const Tile solidifyTo) {
	for (auto&& [entityHandle, amoeba, tile] : GetScene().m_Registry.view<Amoeba, Tile>().each()) {
		Hazel::Entity entity(entityHandle, GetScene());
		entity.RemoveComponent<Amoeba>();
		tile = solidifyTo;
		if (IsDiamond(tile)) {
			auto& animation = entity.GetComponent<Animation>(); // we know it has an Animation component because it was an Amoeba, and Amoeba entities have an Animation
			animation = CharToAnimation('d');
		} else {
			entity.RemoveComponent<Animation>();
		}
	}
}


void Level::ExploderUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	// When we get here, other systems are finished iterating.
	// It is now safe to destroy the game entities at position of explosion entities
	for (auto&& [entityHandle, explosion, transformComponent, animation, spriteRenderer, tile] : GetScene().m_Registry.group<Explosion>(entt::get<Hazel::TransformComponent, Animation, Hazel::SpriteRendererComponent, Tile>).each()) {
		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);
		if (explosion == Explosion::Ignite) {
			ClearEntity(row, col);
			SetEntity(row, col, {entityHandle, GetScene()});
			explosion = Explosion::Burn;
		} else {
			if (animation.CurrentFrame == animation.Frames.size() - 1) {
				if (IsDiamond(animation.Frames.back())) {
					// turn into a diamond
					Hazel::Entity entity(entityHandle, GetScene());
					entity.RemoveComponent<Explosion>();
					entity.AddComponent<Mass>();
					animation = CharToAnimation('d');
					spriteRenderer.Color = CharToColor('d');
				} else {
					HZ_ASSERT(Hazel::Entity(entityHandle, GetScene()) == GetEntity(row, col), "Something has misplaced an explosion - game logic error!");
					ClearEntity(row, col);
				}
			}
		}
	}
}


void Level::AnimatorUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	m_AnimatorAccumulatedTs += ts;
	if (m_AnimatorAccumulatedTs > m_AnimationTimestep) {
		m_AnimatorAccumulatedTs = 0.0f;

		for (auto&& [entityHandle, animation, tile] : GetScene().m_Registry.group<Animation>(entt::get<Tile>).each()) {
			if (++animation.CurrentFrame >= animation.Frames.size()) {
				if (animation.Repeat) {
					animation.CurrentFrame = 0;
				} else {
					Hazel::Entity entity(entityHandle, GetScene());
					entity.RemoveComponent<Animation>();
					return;
				}
			}
			tile = animation.Frames[animation.CurrentFrame];
		}
	}
}


float Level::GetFixedTimestep() {
	return m_FixedTimestep;
}


uint32_t Level::GetWidth() {
	return m_Width;
}


uint32_t Level::GetHeight() {
	return m_Height;
}


Hazel::Entity Level::GetPlayerEntity() {
	return m_PlayerEntity;
}


Hazel::Entity Level::GetEntity(const int row, const int col) {
	return m_Entities[(m_Width * row) + col];
}


void Level::SetEntity(const int row, const int col, Hazel::Entity entity) {
	m_Entities[(m_Width * row) + col] = entity;
}


void Level::ClearEntity(const int row, const int col) {
	int index = (m_Width * row) + col;
	if (m_Entities[index] != m_EmptyEntity) {
		DestroyEntity(m_Entities[index]);
		m_Entities[index] = m_EmptyEntity;
	}
}


void Level::SwapEntities(const int rowA, const int colA, const int rowB, const int colB) {
	std::swap(m_Entities[(m_Width * rowA) + colA], m_Entities[(m_Width * rowB) + colB]);
}


void Level::OnPlayerDied() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_PlayerIsAlive = false;
}


void Level::OnLevelCompleted() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_WonLevel = true;
}


void Level::OnIncreaseScore() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	++m_Score;
	if (m_Score == m_ScoreRequired) {
		Animation animation = {{Tile::Door0, Tile::Door1, Tile::Door2, Tile::Door3}, 0, false};
		m_ExitEntity.AddComponent<Animation>(animation);
	}
}


int Level::GetScore() {
	return m_Score;
}

int Level::GetScoreRequired() {
	return m_ScoreRequired;
}

int Level::GetAmoebaSize() {
	return m_AmoebaSize;
}

int Level::GetAmoebaPotential() {
	return m_AmoebaPotential;
}

bool Level::HasWonLevel() {
	return m_WonLevel;
}


bool Level::IsPlayerAlive() {
	return m_PlayerIsAlive;
}
