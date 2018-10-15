#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	//! \todo Implement this function
	glm::vec3 p = (1 - x) * p0 + x * p1;

	return p;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
	glm::vec3 const& p2, glm::vec3 const& p3,
	float const t, float const x)
{
	//! \todo Implement this function
	glm::vec4 first(1.0, x, pow(x, 2), pow(x, 3));
	glm::mat4 second(glm::vec4(0.0, -t, 2 * t, -t),
		glm::vec4(1.0, 0.0, t - 3, 2 - t),
		glm::vec4(0.0, t, 3 - 2 * t, t - 2),
		glm::vec4(0.0, 0.0, -t, t));
	glm::mat3x4 third = glm::transpose(glm::mat4x3(p0, p1, p2, p3));

	return first * second * third;
}
