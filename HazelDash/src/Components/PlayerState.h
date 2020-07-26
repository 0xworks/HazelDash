#pragma once

enum class PlayerState {
	Idle = 0,
	Blink,
	FootTap,
	BlinkFootTap,
	MovingLeft,
	MovingRight,
	MovingUp,
	MovingDown
};

inline bool IsIdle(PlayerState state) {
	return (state == PlayerState::Idle) || (state == PlayerState::Blink) || (state == PlayerState::FootTap) || (state == PlayerState::BlinkFootTap);
}
