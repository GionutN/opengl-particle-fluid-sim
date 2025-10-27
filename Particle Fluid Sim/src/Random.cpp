#include "Random.h"

#include <random>

static std::mt19937 s_Engine;

void Random::Init()
{
	s_Engine.seed(std::random_device()());
}

float Random::GetFloat(float min, float max)
{
	// way faster than uniform_real_distribution
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	float t = (float)distribution(s_Engine) / INT32_MAX;
	return (1.0f - t) * min + t * max;
}

// returns a random position in a centered disk with a given radius
glm::vec2 Random::GetPointOnCenterDisk(float radius)
{
	glm::vec2 result;
	float theta, scale;
	// using polar coordinates ensures a uniform distribution over the disk
	// avoids points clustering around the corners of the circumscribed square
	theta = Random::GetFloat(0.0f, glm::two_pi<float>());
	scale = Random::GetFloat(0.0f, radius);
	result.x = scale * std::cosf(theta);
	result.y = scale * std::sinf(theta);
	return result;
}

glm::vec2 Random::GetPointInBounds(const glm::vec2& bounds)
{
	glm::vec2 result;
	result.x = Random::GetFloat(-bounds.x, bounds.x);
	result.y = Random::GetFloat(-bounds.y, bounds.y);
	return result;
}
