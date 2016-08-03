#pragma once
#include "Utils.h"
#include "Ray.h"
#include "glm/gtc/random.hpp"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

//! a simple axis-aligned camera class
class Camera
{
public:
	//! constructs a camera object
	Camera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &up, float aspectRatio, float focus, float fov = 45.0f, float aperature = 1.0f, float t0 = 0.0f, float t1 = 1.0f);
	
	//! constructs a new ray at position uv on the camera's view plane
	Ray generateRay(float u, float v) const;
private:
	glm::vec3 mOrigin;
	glm::vec3 mLowerLeftCorner;
	glm::vec3 mHorizontal;
	glm::vec3 mVertical;
	glm::vec3 mU;
	glm::vec3 mV;
	glm::vec3 mW;
	float mLensRadius;
	float mTime0;
	float mTime1;
};

