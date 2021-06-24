#pragma once

#include "Components/Tile.h"

#include "Hazel/Scene/NativeScript.h"


class Level final : public Hazel::NativeScript {
public:
	Level(Hazel::Entity entity, int level, float fixedTimeStep, float animationTimestep);
	virtual ~Level();

	virtual void OnUpdate(Hazel::Timestep ts) override;

	void OnExplode(const int row, const int col);
	void AmoebaFixedUpdate();
	void OnSolidify(const Tile solidfyTo);
	void ExploderUpdate(Hazel::Timestep ts);
	void AnimatorUpdate(Hazel::Timestep ts);

	float GetFixedTimestep();
	uint32_t GetWidth();
	uint32_t GetHeight();

	Hazel::Entity GetPlayerEntity();
	Hazel::Entity GetEntity(const int row, const int col);
	void SetEntity(const int row, const int col, Hazel::Entity entity);
	void ClearEntity(const int row, const int col);
	void SwapEntities(const int rowA, const int colA, const int rowB, const int colB);

	// TODO: should be "events"
	void OnPlayerDied();
	void OnLevelCompleted();
	void OnIncreaseScore();

	int GetScore();
	int GetScoreRequired();

	int GetAmoebaSize();
	int GetAmoebaPotential();

	bool HasWonLevel();
	bool IsPlayerAlive();

public:
	static Level* Get() {
		HZ_ASSERT(sm_Instance, "Level has not been instantiated");
		return sm_Instance;
	}

private:
	inline static Level* sm_Instance = nullptr;

private:
	std::vector<Hazel::Entity> m_Entities;
	Hazel::Entity m_EmptyEntity;
	Hazel::Entity m_ExitEntity;
	Hazel::Entity m_PlayerEntity;

	float m_FixedTimestep = 1.0f / 10.0f;
	float m_FixedUpdateAccumulatedTs = 0.0f;
	float m_AnimationTimestep = 1.0f / 25.0f;
	float m_AnimatorAccumulatedTs = 0.0f;

	int m_Width = 0;
	int m_Height = 0;
	int m_Score = 0;
	int m_ScoreRequired = 0;
	int m_AmoebaSize = 0;
	int m_AmoebaPotential = 0;

	bool m_PlayerIsAlive = false;   // if false, then pressing spacebar restarts.  Otherwise, pressing spacebar pauses
	bool m_WonLevel = false;

};
