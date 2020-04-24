#pragma once

#include "GameObject.h"
#include "Level.h"
#include "ViewPort.h"

#include "Hazel/Core/Layer.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/Texture.h"
#include <glm/glm.hpp>

class HelloBoulderLayer : public Hazel::Layer
{
public:
	HelloBoulderLayer();
	virtual ~HelloBoulderLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Hazel::Timestep ts) override;

#ifdef _DEBUG
	virtual void OnImGuiRender() override;
#endif

	void OnEvent(Hazel::Event& e) override;

private:
	bool OnKeyPressed(Hazel::KeyPressedEvent& e);

	void LoadLevel(int level);

private:
	Hazel::Timestep m_FixedTimestep;
	ViewPort m_ViewPort;
	std::array<Hazel::Ref<Hazel::Texture2D>, 96> m_Tiles;
	glm::vec2 m_TileSize;
	Level m_Level;
	float m_AccumulatedTs;  // would be better as Hazel::Timestep, but that doesnt support operator +=
	int m_CurrentLevel;
	bool m_PlayerIsAlive;   // if false, then pressing spacebar restarts.  Otherwise, pressing spacebar pauses
	bool m_GamePaused;
	bool m_WonLevel;
};
