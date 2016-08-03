#include "../include/Camera.h"

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &up,
	float aspectRatio, float focus, float fov, float aperature,
	float t0, float t1) :
	mTime0(t0),
	mTime1(t1)
{
	// larger aperatures produce "blurrier" images
	mLensRadius = aperature * 0.5f;				// radius is half of the diameter
	float theta = fov * M_PI / 180.0f;			// convert to radians - note that fov is the *vertical* field of view
	float halfHeight = tanf(theta * 0.5f);		// calculate the height of the image plane
	float halfWidth = aspectRatio * halfHeight;	// calculate the width of the image plane

	mOrigin = eye;

	// calculate basis vectors
	mW = glm::normalize(eye - lookAt);
	mU = glm::normalize(glm::cross(up, mW));
	mV = glm::cross(mW, mU);

	mLowerLeftCorner = mOrigin - halfWidth * focus * mU - halfHeight * focus * mV - focus * mW;
	mHorizontal = 2.0f * halfWidth * focus * mU;
	mVertical = 2.0f * halfHeight * focus * mV;
}

Ray Camera::generateRay(float u, float v) const
{
	glm::vec3 rd = mLensRadius * glm::vec3(glm::circularRand(1.0f), 0.0f);
	glm::vec3 offset = mU * rd.x + mV * rd.y;
	float randTime = mTime0 + randFloat() * (mTime1 - mTime0);

	// construct a ray that originates from somewhere on the circular lens at a random time interval
	return Ray{ mOrigin + offset, mLowerLeftCorner + u * mHorizontal + v * mVertical - mOrigin - offset, randTime };
}