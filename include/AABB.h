#pragma once
#include "Ray.h"
#include <algorithm>

class AABB
{
public:
	AABB() = default;
	AABB(const glm::vec3 &aMin, const glm::vec3 &aMax);

	glm::vec3 min() const { return mMin; };
	glm::vec3 max() const { return mMax; };
	bool hit(const Ray &aRay, float aTMin, float aTMax) const;

	friend AABB enclosingBox(const AABB &lhs, const AABB &rhs)
	{
		glm::vec3 min{ std::fminf(lhs.mMin.x, rhs.mMin.x),
					   std::fminf(lhs.mMin.y, rhs.mMin.y),
					   std::fminf(lhs.mMin.z, rhs.mMin.z) };
		glm::vec3 max{ std::fmaxf(lhs.mMax.x, rhs.mMax.x),
					   std::fmaxf(lhs.mMax.y, rhs.mMax.y),
					   std::fmaxf(lhs.mMax.z, rhs.mMax.z) };
		return { min, max };
	}
private:
	glm::vec3 mMin;
	glm::vec3 mMax;
};
