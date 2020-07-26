#include "ViewPort.h"
#include <algorithm>

ViewPort::ViewPort(float left, float bottom, float width, float height)
: m_Camera(left, left + width, bottom, bottom + height)
, m_CameraSpeed(7.0f)
, m_LevelWidth(40.0f)
, m_LevelHeight(20.0f)
, m_ScrollZoneSize(3.0f)
, m_ScrollX(8.0f)
, m_ScrollY(5.0f)
, m_Left(left)
, m_Bottom(bottom)
, m_Width(width)
, m_Height(height)
, m_NewLeft(left)
, m_NewBottom(bottom)
{}


void ViewPort::SetPlayerPosition(float x, float y) {
	if (((m_Left + m_Width) - x) <= m_ScrollZoneSize) {
		m_NewLeft = std::min(m_LevelWidth, x + m_ScrollX) - m_Width;
	} else if ((x - m_Left) <= m_ScrollZoneSize) {
		m_NewLeft = std::max(0.0f, x - m_ScrollX);
	}

	if (((m_Bottom + m_Height) - y) <= m_ScrollZoneSize) {
		m_NewBottom = std::min(m_LevelHeight, y + m_ScrollY) - m_Height;
	} else if ((y - m_Bottom) <= m_ScrollZoneSize) {
		m_NewBottom = std::max(0.0f, y - m_ScrollY);
	}
}


void ViewPort::Update(Hazel::Timestep ts) {
	if (std::abs(m_NewLeft - m_Left) > 0.000001) {
		if (m_NewLeft < m_Left) {
			m_Left -= m_CameraSpeed * ts;
			if (m_Left < m_NewLeft) {
				m_Left = m_NewLeft;
			}
		} else {
			m_Left += m_CameraSpeed * ts;
			if (m_Left > m_NewLeft) {
				m_Left = m_NewLeft;
			}
		}
	} else if(std::abs(m_NewBottom - m_Bottom) > 0.000001) {
		if (m_NewBottom < m_Bottom) {
			m_Bottom -= m_CameraSpeed * ts;
			if (m_Bottom < m_NewBottom) {
				m_Bottom = m_NewBottom;
			}
		} else {
			m_Bottom += m_CameraSpeed * ts;
			if (m_Bottom > m_NewBottom) {
				m_Bottom = m_NewBottom;
			}
		}
	}
	m_Camera.SetProjection(m_Left - 0.5f, (m_Left + m_Width) - 0.5f, m_Bottom - 0.5f, (m_Bottom + m_Height) - 0.5f);
}


bool ViewPort::Overlaps(const Position& pos) {
	return
		(pos.Col > m_Left - 1.0f) &&
		(pos.Col < (m_Left + m_Width) + 1.0f) &&
		(pos.Row > m_Bottom - 1.0f) &&
		(pos.Row < (m_Bottom + m_Height) + 1.0f)
	;
}
