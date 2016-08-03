#pragma once
#include "Utils.h"
#include "Ray.h"
#include "glm/gtc/random.hpp"
#include <memory>

class Material;
using MaterialRef = std::shared_ptr<Material>;

//! a struct that holds hit information
struct HitRecord
{
	float t;
	glm::vec3 position;
	glm::vec3 normal;
	MaterialRef material;
};

class Material
{
public:
	//! produces a scattered ray
	virtual bool scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const glm::vec3 &aAlbedo);

	//! produces a scattered ray
	bool scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const override;
private:
	glm::vec3 mAlbedo;
};

class Metallic : public Material
{
public:
	Metallic(const glm::vec3 &aAlbedo, float aRoughness = 0.0f);

	//! produces a scattered ray
	bool scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const override;
private:
	glm::vec3 mAlbedo;
	float mRoughness;
};

class Dieletric : public Material
{
public:
	Dieletric(float aIOR);

	//! produces a scattered ray
	bool scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const override;
private:
	float mIOR; // index of refraction

	float schlick(float aCosine, float aIOR) const;
};