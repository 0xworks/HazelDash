#pragma once

#include "Hazel/Scene/NativeScript.h"

#include "Components/Animation.h"
#include "Components/PlayerState.h"

#include "Hazel/Scene/Components.h"

class HazelDashLayer;

class PlayerController : public Hazel::NativeScript {
public:

	PlayerController(Hazel::Entity entity, HazelDashLayer* level, float fixedTimeStep, float pushProbability);
	virtual void OnUpdate(Hazel::Timestep ts) override;

private:
	bool IsIdle(PlayerState state);
	PlayerState SetPlayerBlinkState(PlayerState state);
	PlayerState ClearPlayerBlinkState(PlayerState state);
	PlayerState SwapPlayerFootTapState(PlayerState state);
	const Animation& GetPlayerAnimation(PlayerState state, bool lastWasLeft);

	bool TryMovePlayer(Hazel::TransformComponent& transformComponent, const std::pair<int, int> direction, const bool ctrlPressed);

private:
	HazelDashLayer* m_Level; // there is no need for this to be a smart pointer. We know that scope of this PlayerController is entirely within scope of m_Level
	float m_FixedUpdateAccumulatedTs = 0.0f;
	float m_FixedTimeStep = 1.0f / 10.0f;
	float m_PushProbability = 0.25f;
	bool m_LastWasLeft = false;
};
