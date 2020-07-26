#pragma once

#include <glm/glm.hpp>

struct Camera {
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ViewProjectionMatrix;
};
