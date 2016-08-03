#include "../include/Ray.h"

Ray::Ray(const glm::vec3 &aOrigin, const glm::vec3 &aDirection, float aTime) :
	mOrigin(aOrigin), mDirection(aDirection), mTime(aTime)
{
}