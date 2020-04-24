#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Core/Timestep.h"

class ViewPort {
public:
	ViewPort(float left, float bottom, float width, float height);

	float GetLeft() const { return m_Left; }
	float GetRight() const { return m_Left + m_Width; }
	float GetBottom() const { return m_Bottom; }
	float GetTop() const { return m_Bottom + m_Height; }

	void SetCameraSpeed(float speed) { m_CameraSpeed = speed; }
	void SetLevelSize(float width, float height) { m_LevelWidth = width; m_LevelHeight = height; }
	void SetPlayerPosition(float x, float y);

	void Update(Hazel::Timestep ts);

	Hazel::OrthographicCamera& GetCamera() { return m_Camera; }
	const Hazel::OrthographicCamera& GetCamera() const { return m_Camera; }

private:
	Hazel::OrthographicCamera m_Camera;
	float m_CameraSpeed;
	float m_LevelWidth;
	float m_LevelHeight;
	float m_ScrollZoneSize;
	float m_ScrollX;
	float m_ScrollY;

	float m_Left;
	float m_Bottom;
	float m_Width;
	float m_Height;

	float m_NewLeft;
	float m_NewBottom;

};
