#pragma once

#include "GameObject.h"
#include "Tile.h"
#include <vector>

class Player : public GameObject {
public:
	Player();

	virtual void FixedUpdate(size_t row, size_t col, Level& level) override;

	virtual void Animate(Hazel::Timestep ts) override;

	virtual Tile GetTile() const override;

#ifdef _DEBUG
	virtual void ImGuiRender() override;
#endif

private:

	enum class State {
		Idle = 0,
		Blink,
		FootTap,
		BlinkFootTap,
		MovingLeft,
		MovingRight,
		MovingUp,
		MovingDown
	};

private:
	bool IsIdle() const;

	bool TryMove(size_t& row, size_t& col, const size_t rowOffset, const size_t colOffset, Level& level, const bool ctrlPressed);

	void SetBlinkState();
	void ClearBlinkState();
	void SwapFootTapState();

private:
	std::vector<std::vector<Tile>> m_Animations;   // sequences of Tiles for various animations
	int m_Animation; // which animation is player currently on?
	int m_Frame;     // which frame of animation is player currently on?
	float m_AnimationTimeStep;
	float m_AnimationAccumulator;  // would be better as Hazel::Timestep, but that doesnt currently support operator +=

	State m_State;
	float m_BlinkProbability;
	float m_SwapFootTapProbability;
	float m_PushProbability;

	bool m_LastWasLeft;  // used to decide whether up/down animation faces left or right
};
