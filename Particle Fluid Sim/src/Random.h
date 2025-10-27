#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Random
{
public:
	static void Init();
	
	static float GetFloat(float min = 0.0f, float max = 1.0f);
	static glm::vec2 GetPointOnCenterDisk(float radius = 1.0f);
	static glm::vec2 GetPointInBounds(const glm::vec2& bounds);

private:
	Random() = default;

};