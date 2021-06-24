#pragma once

#include "Hazel/Scene/NativeScript.h"

class Physics : public Hazel::NativeScript {
public:

	Physics(Hazel::Entity entity, float fixedTimeStep);
	virtual void OnUpdate(Hazel::Timestep ts) override;

private:
	float m_FixedUpdateAccumulatedTs = 0.0f;
	float m_FixedTimestep = 1.0f / 10.0f;
};
