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

private:
	std::array<Hazel::Ref<Hazel::Texture2D>, (size_t)Tile::NumTiles> m_Tiles; // DELETE ME

	Hazel::Scene m_Scene;

	Hazel::Entity m_LevelEntity;

	Hazel::Timestep m_FixedTimestep;
	Hazel::Timestep m_AnimationTimestep;

	int m_CurrentLevel;

	bool m_GamePaused;
};
