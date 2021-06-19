#pragma once

#include "Hazel/Scene/Components.h"
#include "Hazel/Scene/NativeScript.h"

class CameraController : public Hazel::NativeScript {
public:

	CameraController(Hazel::Entity entity, Hazel::Entity trackEntity, float levelWidth, float levelHeight, float cameraSpeed)
	: NativeScript(entity)
	, m_TrackEntity{trackEntity}
	, m_LevelWidth{levelWidth}
	, m_LevelHeight{levelHeight}
	, m_CameraSpeed{cameraSpeed}
	{}


	virtual void OnUpdate(Hazel::Timestep ts) override {

		// if tracked entity is no longer valid (e.g. player might have been killed)
		// then nothing to do here...
		if (!m_TrackEntity) {
			return;
		}

		/////////////////////////////////////////
		//
		// TODO: This block of code only needs to be done if the viewport is resized,
		//       or the player has moved.
		//       However, the event system isn't quite there yet, so for now
		//       just do this on every update
		auto& cameraTransform = GetComponent<Hazel::TransformComponent>();
		const auto& cameraProjection = GetComponent<Hazel::CameraComponent>().Camera.GetProjection();

		// HACK: There is currently no easy way to get the width/height of the (2d) camera's viewport
		//       So, we'll just extract it from the projection matrix
		float halfWidth = 1.0f / cameraProjection[0][0];
		float halfHeight = 1.0f / cameraProjection[1][1];

		// similarly, extract the camera's position in world space from its transform
		float& cameraX = cameraTransform.Translation.x;
		float& cameraY = cameraTransform.Translation.y;

		// grab the tracked entity's current position
		auto& trackTransform = m_TrackEntity.GetComponent<Hazel::TransformComponent>();
		const float trackX = trackTransform.Translation.x;
		const float trackY = trackTransform.Translation.y;

		m_NewCameraX = cameraX;
		m_NewCameraY = cameraY;
		if (trackX > (cameraX + halfWidth - m_Border)) {
			m_NewCameraX = std::min(m_LevelWidth - halfWidth, cameraX + m_DeltaX);
		} else if (trackX < (cameraX - halfWidth + m_Border)) {
			m_NewCameraX = std::max(halfWidth, cameraX - m_DeltaX);
		}
		if (trackY > (cameraY + halfHeight - m_Border)) {
			m_NewCameraY = std::min(m_LevelHeight - halfHeight, cameraY + m_DeltaY);
		} else if (trackY < (cameraY - halfHeight + m_Border)) {
			m_NewCameraY = std::max(halfHeight, cameraY - m_DeltaY);
		}
		//
		//////////////////////////////////////////////

		if (std::abs(m_NewCameraX - cameraX) > 0.000001) {
			if (m_NewCameraX < cameraX) {
				cameraX = std::max(cameraX - m_CameraSpeed * ts, m_NewCameraX);
			} else {
				cameraX = std::min(cameraX + m_CameraSpeed * ts, m_NewCameraX);
			}
		} else if (std::abs(m_NewCameraY - cameraY) > 0.000001) {
			if (m_NewCameraY < cameraY) {
				cameraY = std::max(cameraY - m_CameraSpeed * ts, m_NewCameraY);
			} else {
				cameraY = std::min(cameraY + m_CameraSpeed * ts, m_NewCameraY);
			}
		}
	}

private:
	Hazel::Entity m_TrackEntity = {}; // the entity that this controller makes the camera "track"

	float m_LevelWidth = 0.0f;
	float m_LevelHeight = 0.0f;

	float m_CameraSpeed = 7.0f; // how fast does the camera pan
	float m_DeltaX = 8.0f;      // how much does the camera move in the X axis in order to follow the tracked entity
	float m_DeltaY = 5.0f;      // how much does the camera move in the Y axis in order to follow the tracked entity
	float m_Border = 3.0;       // how close can the tracked entity get to the edge of the viewport before the camera should move

	float m_NewCameraX = 0.0f;
	float m_NewCameraY = 0.0;

};
