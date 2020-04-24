#include "Player.h"
#include "Level.h"
#include "Random.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Renderer/Renderer2D.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

Player::Player()
: m_Animations({
	{ Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle,  Tile::PlayerIdle},
	{ Tile::PlayerAnim1_0, Tile::PlayerAnim1_1, Tile::PlayerAnim1_2, Tile::PlayerAnim1_3, Tile::PlayerAnim1_4, Tile::PlayerAnim1_5, Tile::PlayerAnim1_6, Tile::PlayerAnim1_7},
	{ Tile::PlayerAnim2_0, Tile::PlayerAnim2_1, Tile::PlayerAnim2_2, Tile::PlayerAnim2_3, Tile::PlayerAnim2_4, Tile::PlayerAnim2_5, Tile::PlayerAnim2_6, Tile::PlayerAnim2_7},
	{ Tile::PlayerAnim3_0, Tile::PlayerAnim3_1, Tile::PlayerAnim3_2, Tile::PlayerAnim3_3, Tile::PlayerAnim3_4, Tile::PlayerAnim3_5, Tile::PlayerAnim3_6, Tile::PlayerAnim3_7},
	{ Tile::PlayerLeft0, Tile::PlayerLeft1, Tile::PlayerLeft2, Tile::PlayerLeft3, Tile::PlayerLeft4, Tile::PlayerLeft5, Tile::PlayerLeft6, Tile::PlayerLeft7},
	{ Tile::PlayerRight0, Tile::PlayerRight1, Tile::PlayerRight2, Tile::PlayerRight3, Tile::PlayerRight4, Tile::PlayerRight5, Tile::PlayerRight6, Tile::PlayerRight7}
})
, m_Animation(0)
, m_Frame(0)
, m_AnimationTimeStep(1.0f / 25.0f)
, m_AnimationAccumulator(0.0f)
, m_State(State::Idle)
, m_BlinkProbability(0.25)
, m_SwapFootTapProbability(1.0f / 16.0f)
, m_PushProbability(0.25)
, m_LastWasLeft(false)
{}


void Player::FixedUpdate(size_t row, size_t col, Level& level) {
	State newState = State::Idle;
	
	if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT) || Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
		newState = State::MovingLeft;
		m_LastWasLeft = true;
	} else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT) || Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
		newState = State::MovingRight;
		m_LastWasLeft = false;
	}

	if (Hazel::Input::IsKeyPressed(HZ_KEY_UP) || Hazel::Input::IsKeyPressed(HZ_KEY_W)) {
		newState = State::MovingUp;
	} else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN) || Hazel::Input::IsKeyPressed(HZ_KEY_S)) {
		newState = State::MovingDown;
	}

	if (newState == State::Idle) {
		if (!IsIdle()) {
			m_Frame = 0;
			m_State = newState;
		}
	} else {
		if (newState != m_State) {
			m_Frame = 0;
			m_State = newState;
		}
	}

	bool ctrlPressed = Hazel::Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
	size_t oldRow = row;
	size_t oldCol = col;
	switch (m_State) {
		case State::MovingLeft:
			if (level.GetGameObject(row, col - 1).CanBeOccupied()) {
				if (!ctrlPressed) {
					--col;
				}
			} else if (level.GetGameObject(row, col - 1).IsPushable()) {
				if (!level.GetGameObject(row - 1, col - 1).IsEmpty() && level.GetGameObject(row, col - 2).IsEmpty()) {
					if(Random::Uniform0_1() < m_PushProbability) {
						level.SwapObjects(row, col - 2, row, col - 1);
						level.SetUpdated(row, col - 2, true);
						if (!ctrlPressed) {
							--col;
						}
					}
				}
			}
			break;
		case State::MovingRight:
			if (level.GetGameObject(row, col + 1).CanBeOccupied()) {
				if (!ctrlPressed) {
					++col;
				}
			} else if (level.GetGameObject(row, col + 1).IsPushable()) {
				if (!level.GetGameObject(row - 1, col + 1).IsEmpty() && level.GetGameObject(row, col + 2).IsEmpty()) {
					if (Random::Uniform0_1() < m_PushProbability) {
						level.SwapObjects(row, col + 2, row, col + 1);
						level.SetUpdated(row, col + 2, true);
						if (!ctrlPressed) {
							++col;
						}
					}
				}
			}
			break;
		case State::MovingUp:
			if (level.GetGameObject(row + 1, col).CanBeOccupied()) {
				if (!ctrlPressed) {
					++row;
				}
			}
			break;
		case State::MovingDown:
			if (level.GetGameObject(row - 1, col).CanBeOccupied()) {
				if (!ctrlPressed) {
					--row;
				}
			}
			break;
	}
	if ((row != oldRow) || (col != oldCol)) {
		if (level.GetGameObject(row, col).IsCollectable()) {
			level.IncreaseScore();
		}
		level.SwapObjects(oldRow, oldCol, row, col);
		level.SetGameObject(oldRow, oldCol, std::make_unique<GameObject>(Tile::Empty));
		level.SetUpdated(oldRow, oldCol, true);
		level.SetUpdated(row, col, true);
	}
}


void Player::Animate(Hazel::Timestep ts) {
	m_AnimationAccumulator += ts;

	m_Animation = (int)m_State;
	if ((m_State == State::MovingUp) || (m_State == State::MovingDown)) {
		m_Animation = m_LastWasLeft ? (int)State::MovingLeft : (int)State::MovingRight;
	}

	if (m_AnimationAccumulator >= m_AnimationTimeStep) {
		++m_Frame;
		if (m_Frame >= m_Animations[m_Animation].size()) {
			if (IsIdle()) {
				if (Random::Uniform0_1() < m_BlinkProbability) {
					SetBlinkState();
				} else {
					ClearBlinkState();
				}
				if (Random::Uniform0_1() < m_SwapFootTapProbability) {
					SwapFootTapState();
				}
			}
			m_Frame = 0;
		}
		m_AnimationAccumulator = 0.0;
	}
}


Tile Player::GetTile() const {
	return m_Animations[m_Animation][m_Frame];
}


#ifdef _DEBUG
void Player::ImGuiRender() {
	int animationFPS = (int)(1.0f / m_AnimationTimeStep);
	ImGui::Begin("Player Settings");
	ImGui::DragInt("Animation FPS", &animationFPS, 1, 1, 60);
	ImGui::DragFloat("Blink prob.", &m_BlinkProbability, 0.05f, 0.0f, 1.0f);
	ImGui::DragFloat("Foot tap prob.", &m_SwapFootTapProbability, 0.05f, 0.0f, 1.0f);
	ImGui::DragFloat("Push prob.", &m_PushProbability, 0.05f, 0.0f, 1.0f);
	ImGui::End();
	m_AnimationTimeStep = 1.0f / animationFPS;
}
#endif


bool Player::IsIdle() const {
	return (m_State == State::Idle) || (m_State == State::Blink) || (m_State == State::FootTap) || (m_State == State::BlinkFootTap);
}


void Player::SetBlinkState() {
	switch (m_State) {
		case State::Idle:
			m_State = State::Blink;
			break;
		case State::Blink:
			m_State = State::Blink;
			break;
		case State::FootTap:
			m_State = State::BlinkFootTap;
			break;
		case State::BlinkFootTap:
			m_State = State::BlinkFootTap;
			break;
	}
}


void Player::ClearBlinkState() {
	switch (m_State) {
		case State::Idle:
			m_State = State::Idle;
			break;
		case State::Blink:
			m_State = State::Idle;
			break;
		case State::FootTap:
			m_State = State::FootTap;
			break;
		case State::BlinkFootTap:
			m_State = State::FootTap;
			break;
	}
}


void Player::SwapFootTapState() {
	switch (m_State) {
		case State::Idle:
			m_State = State::FootTap;
			break;
		case State::Blink:
			m_State = State::BlinkFootTap;
			break;
		case State::FootTap:
			m_State = State::Idle;
			break;
		case State::BlinkFootTap:
			m_State = State::Blink;
			break;
	}
}
