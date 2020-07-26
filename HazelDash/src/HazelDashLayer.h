#pragma once

#include "Components/Position.h"
#include "Components/Tile.h"
#include "ViewPort.h"

#include "Hazel/Scene/Entity.h"
#include "Hazel/Core/Layer.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/Texture.h"

#include <glm/glm.hpp>

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

	// game "systems" and "events" (TODO: rework into proper systems and events, later...)
	void PhysicsFixedUpdate();
	void PlayerControllerFixedUpdate();
	void EnemiesFixedUpdate();
	void OnExplode(const Position& pos);
	void AmoebaFixedUpdate();
	void OnSolidify(const Tile solidfyTo);
	void PlayerControllerUpdate(Hazel::Timestep ts);
	bool TryMovePlayer(Position& pos, Position direction, const bool ctrlPressed);
	void OnPlayerMoved(const Position& pos);
	void OnPlayerDied();
	void OnLevelCompleted();
	void OnIncreaseScore();
	void ExploderUpdate(Hazel::Timestep ts);
	void AnimatorUpdate(Hazel::Timestep ts);
	void CameraControllerUpdate(Hazel::Timestep ts);
	void RendererUpdate(Hazel::Timestep ts);

	Hazel::Entity GetEntity(const Position pos);
	void SetEntity(Position pos, Hazel::Entity entity);
	void ClearEntity(const Position pos);
	void SwapEntities(const Position posA, const Position posB);

private:
	std::array<Hazel::Ref<Hazel::Texture2D>, (size_t)Tile::NumTiles> m_Tiles;

	Hazel::Scene m_Scene;
	// TODO: Maybe viewport (and associated camera) should be components of a "camera entity" that belongs to the scene...?
	ViewPort m_ViewPort;
	std::vector<Hazel::Entity> m_Entities;
	Hazel::Entity m_EmptyEntity;
	Hazel::Entity m_ExitEntity;

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
