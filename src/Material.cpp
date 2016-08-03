#include "../include/Material.h"

//----------------------------------------------------------------------------------
// lambertian
Lambertian::Lambertian(const glm::vec3 &aAlbedo) :
	mAlbedo(aAlbedo)
{
}

bool Lambertian::scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const
{
	glm::vec3 target = aRecord.position + aRecord.normal + glm::sphericalRand(1.0f);
	aScattered = Ray(aRecord.position, target - aRecord.position, aRay.time());
	aAttenuation = mAlbedo;
	return true;
}

//----------------------------------------------------------------------------------
// metallic
Metallic::Metallic(const glm::vec3 &aAlbedo, float aRoughness) :
	mAlbedo(aAlbedo)
{
	if (aRoughness > 1.0f) aRoughness = 1.0f;
	if (aRoughness < 0.0f) aRoughness = 0.0f;
	mRoughness = aRoughness;
}

bool Metallic::scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const
{
	glm::vec3 reflected = glm::reflect(glm::normalize(aRay.direction()), aRecord.normal);
	aScattered = Ray(aRecord.position, reflected + mRoughness * glm::sphericalRand(1.0f), aRay.time());
	aAttenuation = mAlbedo;
	return glm::dot(aScattered.direction(), aRecord.normal) > 0.0f;
}

//----------------------------------------------------------------------------------
// dieletric
Dieletric::Dieletric(float aIOR) :
	mIOR(aIOR)
{
}

bool Dieletric::scatter(const Ray &aRay, const HitRecord &aRecord, glm::vec3 &aAttenuation, Ray &aScattered) const
{
	glm::vec3 outwardNormal;
	glm::vec3 reflected = glm::reflect(glm::normalize(aRay.direction()), aRecord.normal);
	aAttenuation = glm::vec3(1.0f);

	float niOverNt;
	float dt = glm::dot(aRay.direction(), aRecord.normal);
	float cosine;
	float reflectProbability;

	if (dt > 0.0f)
	{
		outwardNormal = -aRecord.normal;
		niOverNt = mIOR;
		cosine = mIOR * dt / glm::length(aRay.direction());
	}
	else
	{
		outwardNormal = aRecord.normal;
		niOverNt = 1.0f / mIOR;
		cosine = -dt / glm::length(aRay.direction());
	}

	glm::vec3 refracted = glm::refract(glm::normalize(aRay.direction()), outwardNormal, niOverNt);

	float discriminant = 1.0f - niOverNt * niOverNt * (1.0f - dt * dt);
	if (discriminant > 0.0f)
	{
		reflectProbability = schlick(cosine, mIOR);
	}
	else
	{
		reflectProbability = 1.0f;
	}

	// reflect or refract based on the probabilities calculated above
	if (randFloat() < reflectProbability)
	{
		aScattered = Ray(aRecord.position, reflected, aRay.time());
	}
	else 
	{
		aScattered = Ray(aRecord.position, refracted, aRay.time());
	}
	
	return true;
}

float Dieletric::schlick(float aCosine, float aIOR) const
{
	float r0 = (1.0f - aIOR) / (1.0f + aIOR);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf(1.0f - aCosine, 5.0f);
}