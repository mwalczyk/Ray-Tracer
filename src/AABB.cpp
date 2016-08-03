#include "../include/AABB.h"

AABB::AABB(const glm::vec3 &aMin, const glm::vec3 &aMax) :
	mMin(aMin),
	mMax(aMax)
{
}

bool AABB::hit(const Ray &aRay, float aTMin, float aTMax) const
{
	for (size_t i = 0; i < 3; ++i)
	{
		float invD = 1.0f / aRay.direction()[i];
		float t0 = (mMin[i] - aRay.origin()[i]) * invD;
		float t1 = (mMax[i] - aRay.origin()[i]) * invD;
		if (invD < 0.0f)
		{
			std::swap(t0, t1);
		}
		aTMin = t0 > aTMin ? t0 : aTMin;
		aTMax = t1 < aTMax ? t1 : aTMax;
		if (aTMax <= aTMin)
		{
			return false;
		}
	}
	return true;
}