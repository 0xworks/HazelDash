#pragma once

#include "Components/Tile.h"

#include "Hazel/Core/Layer.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Scene/Entity.h"

class HazelDashLayer : public Hazel::Layer
{
public:
	HazelDashLayer();
	virtual ~HazelDashLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Hazel::Timestep ts) override;

#ifdef _DEBUG
	virtual void OnImGuiRender() override;
#endif

	void OnEvent(Hazel::Event& e) override;

private:
	bool OnKeyPressed(Hazel::KeyPressedEvent& e);

	void LoadScene(int level);

	// game "systems" and "events" (TODO: rework to use ScriptComponents instead)
	void PhysicsFixedUpdate();
	void PlayerControllerFixedUpdate();
	void EnemiesFixedUpdate();
	void OnExplode(const int row, const int col);
	void AmoebaFixedUpdate();
	void OnSolidify(const Tile solidfyTo);
	void PlayerControllerUpdate(Hazel::Timestep ts);
	bool TryMovePlayer(Hazel::TransformComponent& transformComponent, const std::pair<int, int> direction, const bool ctrlPressed);
	void OnPlayerDied();
	void OnLevelCompleted();
	void OnIncreaseScore();
	void ExploderUpdate(Hazel::Timestep ts);
	void AnimatorUpdate(Hazel::Timestep ts);

	Hazel::Entity GetEntity(const int row, const int col);
	void SetEntity(const int row, const int col, Hazel::Entity entity);
	void ClearEntity(const int row, const int col);
	void SwapEntities(const int rowA, const int colA, const int rowB, const int colB);

private:
	std::array<Hazel::Ref<Hazel::Texture2D>, (size_t)Tile::NumTiles> m_Tiles; // DELETE ME

	Hazel::Scene m_Scene;

	std::vector<Hazel::Entity> m_Entities;  // }- TODO: need to be able to get rid of these.
	Hazel::Entity m_EmptyEntity;            // }  They are only here because we need them for game logic.
	Hazel::Entity m_ExitEntity;             // }  However, game logic should be part of "scripts" in the scene.

	Hazel::Timestep m_FixedTimestep;
	Hazel::Timestep m_AnimationTimestep;
	float m_FixedUpdateAccumulatedTs;  // Might be be better as a Hazel::Timestep, but that class doesnt support operator +=
	float m_AnimatorAccumulatedTs;     // Might be be better as a Hazel::Timestep, but that class doesnt support operator +=

	float m_PushProbability;

	int m_CurrentLevel;
	int m_Width;
	int m_Height;
	int m_Score;
	int m_ScoreRequired;
	int m_AmoebaSize;
	int m_AmoebaPotential;

	bool m_GamePaused;
	bool m_PlayerIsAlive;   // if false, then pressing spacebar restarts.  Otherwise, pressing spacebar pauses
	bool m_WonLevel;
};
