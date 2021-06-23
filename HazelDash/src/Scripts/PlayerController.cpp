#include "PlayerController.h"

#include "Random.h"
#include "Components/Animation.h"
#include "Components/PlayerState.h"
#include "Components/Roll.h"
#include "Components/Tile.h"
#include "Scripts/Level.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Scene/Components.h"


bool IsIdle(PlayerState state) {
	return (state == PlayerState::Idle) || (state == PlayerState::Blink) || (state == PlayerState::FootTap) || (state == PlayerState::BlinkFootTap);
}


PlayerState SetPlayerBlinkState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::Blink;
			break;
		case PlayerState::Blink:
			state = PlayerState::Blink;
			break;
		case PlayerState::FootTap:
			state = PlayerState::BlinkFootTap;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::BlinkFootTap;
			break;
	}
	return state;
}


PlayerState ClearPlayerBlinkState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::Idle;
			break;
		case PlayerState::Blink:
			state = PlayerState::Idle;
			break;
		case PlayerState::FootTap:
			state = PlayerState::FootTap;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::FootTap;
			break;
	}
	return state;
}


PlayerState SwapPlayerFootTapState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::FootTap;
			break;
		case PlayerState::Blink:
			state = PlayerState::BlinkFootTap;
			break;
		case PlayerState::FootTap:
			state = PlayerState::Idle;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::Blink;
			break;
	}
	return state;
}


const Animation& GetPlayerAnimation(PlayerState state, bool lastWasLeft) {
	static std::array<Animation, 6> animations = {
		Animation {{ Tile::PlayerIdle0,  Tile::PlayerIdle0,  Tile::PlayerIdle0,  Tile::PlayerIdle0}},
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerLeft0, Tile::PlayerLeft1, Tile::PlayerLeft2, Tile::PlayerLeft3}},
		Animation {{ Tile::PlayerRight0, Tile::PlayerRight1, Tile::PlayerRight2, Tile::PlayerRight3}}
	};
	if (state == PlayerState::MovingUp || state == PlayerState::MovingDown) {
		state = lastWasLeft ? PlayerState::MovingLeft : PlayerState::MovingRight;
	}
	return animations[static_cast<int>(state)];
}


bool TryMovePlayer(Hazel::TransformComponent& transformComponent, const std::pair<int, int> direction, const bool ctrlPressed) {
	static float pushProbability = 0.25f;

	bool retVal = false;

	int row = static_cast<int>(transformComponent.Translation.y);
	int col = static_cast<int>(transformComponent.Translation.x);

	Level* level = Level::Get();

	Hazel::Entity entity = level->GetEntity(row + direction.first, col + direction.second);
	auto& tile = entity.GetComponent<Tile>();
	if (CanBeOccupied(tile)) {
		retVal = true;
		if (!ctrlPressed) {
			transformComponent.Translation += glm::vec3{direction.second, direction.first, 0.0f};
		}
	} else if ((direction.first == 0) && IsPushable(tile)) {
		retVal = true;
		if (Random::Uniform0_1() < pushProbability) {
			int rowBelow = row - 1;
			int colBelow = col + direction.second;
			int rowAcross = row;
			int colAcross = col + (2 * direction.second);
			Hazel::Entity entityBelow = level->GetEntity(rowBelow, colBelow);
			Hazel::Entity entityAcross = level->GetEntity(rowAcross, colAcross);
			const auto tileBelow = entityBelow.GetComponent<Tile>();
			const auto tileAcross = entityAcross.GetComponent<Tile>();
			if (!IsEmpty(tileBelow) && IsEmpty(tileAcross)) {
				level->SwapEntities(rowAcross, colAcross, row + direction.first, col + direction.second);
				auto& transformPushed = entity.GetComponent<Hazel::TransformComponent>();
				transformPushed.Translation += glm::vec3{direction.second, direction.first, 0.0f};
				if (entity.HasComponent<Roll>()) {
					auto& roll = entity.GetComponent<Roll>();
					roll.CurrentFrame = (roll.CurrentFrame + direction.second) % roll.Frames.size();
					tile = roll.Frames[roll.CurrentFrame];
				}
				if (!ctrlPressed) {
					transformComponent.Translation += glm::vec3{direction.second, direction.first, 0.0f};
				}
			}
		}
	}
	return retVal;
}


PlayerController::PlayerController(Hazel::Entity entity)
: NativeScript{entity}
{}


void PlayerController::OnUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();
	static const std::pair<int, int> Left = {0, -1};
	static const std::pair<int, int> Right = {0, 1};
	static const std::pair<int, int> Up = {1, 0};
	static const std::pair<int, int> Down = {-1, 0};

	auto& animation = GetComponent<Animation>();
	auto& state = GetComponent<PlayerState>();

	auto level = Level::Get();

	// update on fixed timestep
	m_FixedUpdateAccumulatedTs += ts;
	if (m_FixedUpdateAccumulatedTs > level->GetFixedTimestep()) {

		auto& transformComponent = GetComponent<Hazel::TransformComponent>();

		PlayerState newState = PlayerState::Idle;
		PlayerState secondaryState = PlayerState::Idle;

		if (Hazel::Input::IsKeyPressed(Hazel::Key::Left) || Hazel::Input::IsKeyPressed(Hazel::Key::A)) {
			newState = PlayerState::MovingLeft;
			m_LastWasLeft = true;
		} else if (Hazel::Input::IsKeyPressed(Hazel::Key::Right) || Hazel::Input::IsKeyPressed(Hazel::Key::D)) {
			newState = PlayerState::MovingRight;
			m_LastWasLeft = false;
		}

		if (Hazel::Input::IsKeyPressed(Hazel::Key::Up) || Hazel::Input::IsKeyPressed(Hazel::Key::W)) {
			secondaryState = newState;
			newState = PlayerState::MovingUp;
		} else if (Hazel::Input::IsKeyPressed(Hazel::Key::Down) || Hazel::Input::IsKeyPressed(Hazel::Key::S)) {
			secondaryState = newState;
			newState = PlayerState::MovingDown;
		}

		if (IsIdle(state)) {
			if (!IsIdle(newState)) {
				state = newState;
				animation = GetPlayerAnimation(state, m_LastWasLeft);
			}
		} else {
			if (state != newState) {
				state = newState;
				animation = GetPlayerAnimation(state, m_LastWasLeft);
			}
		}

		bool ctrlPressed = Hazel::Input::IsKeyPressed(Hazel::Key::LeftControl) || Hazel::Input::IsKeyPressed(Hazel::Key::RightControl);
		int oldRow = static_cast<int>(transformComponent.Translation.y);
		int oldCol = static_cast<int>(transformComponent.Translation.x);
		switch (state) {
			case PlayerState::MovingLeft:
				TryMovePlayer(transformComponent, Left, ctrlPressed);
				break;
			case PlayerState::MovingRight:
				TryMovePlayer(transformComponent, Right, ctrlPressed);
				break;
			case PlayerState::MovingUp:
				if (!TryMovePlayer(transformComponent, Up, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(transformComponent, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(transformComponent, Right, ctrlPressed);
					}
				}
				break;
			case PlayerState::MovingDown:
				if (!TryMovePlayer(transformComponent, Down, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(transformComponent, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(transformComponent, Right, ctrlPressed);
					}
				}
				break;
		}

		int row = static_cast<int>(transformComponent.Translation.y);
		int col = static_cast<int>(transformComponent.Translation.x);
		if ((row != oldRow) || (col != oldCol)) {
			Hazel::Entity entityAtNewPos = level->GetEntity(row, col);
			auto tile = entityAtNewPos.GetComponent<Tile>();
			level->SwapEntities(oldRow, oldCol, row, col);
			level->ClearEntity(oldRow, oldCol);
			if (IsDoor(tile)) {
				level->OnLevelCompleted();
			}
			if (IsCollectable(tile)) {
				level->OnIncreaseScore();
			}
		}

		m_FixedUpdateAccumulatedTs = 0.0;
	}

	// update every frame
	if (animation.CurrentFrame == (animation.Frames.size() - 1)) {
		if (IsIdle(state)) {
			PlayerState newState = state;
			if (Random::Uniform0_1() < 0.25f) {
				newState = SetPlayerBlinkState(state);
			} else {
				newState = ClearPlayerBlinkState(state);
			}
			if (Random::Uniform0_1() < 1.0f / 16.0f) {
				newState = SwapPlayerFootTapState(state);
			}
			if (state != newState) {
				state = newState;
				animation = GetPlayerAnimation(state, false);
			}
		}
	}
}




