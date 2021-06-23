#pragma once

#include "Hazel/Scene/NativeScript.h"

class PlayerController : public Hazel::NativeScript {
public:

	PlayerController(Hazel::Entity entity);
	virtual void OnUpdate(Hazel::Timestep ts) override;

private:
	float m_FixedUpdateAccumulatedTs = 0.0f;
	bool m_LastWasLeft = false;
};
