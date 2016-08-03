#pragma once
#include "../include/glm/glm.hpp"

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3 &aOrigin, const glm::vec3 &aDirection, float aTime = 0.0f);
	Ray(const Ray &rhs) = default;

	glm::vec3 origin() const { return mOrigin; };
	glm::vec3 direction() const { return mDirection; };
	float time() const { return mTime; };
	glm::vec3 pointAtTime(float aT) const { return mOrigin + aT * mDirection; };
private:
	glm::vec3 mOrigin;		// point
	glm::vec3 mDirection;	// direction vector
	float mTime;			// the time at which this ray was fired
};

