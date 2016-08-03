#pragma once
#include "Ray.h"
#include "Material.h"
#include "AABB.h"
#include <memory>
#include <vector>
#include <iostream>
#include <functional>

class Hitable;
using HitableRef = std::shared_ptr<Hitable>;

class HitableList;
using HitableListRef = std::shared_ptr<HitableList>;

class Sphere;
using SphereRef = std::shared_ptr<Sphere>;

class MovingSphere;
using MovingSphereRef = std::shared_ptr<MovingSphere>;

class BVHNode;
using BVHNodeRef = std::shared_ptr<BVHNode>;

class Hitable
{
public:
	virtual bool hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const = 0;
	virtual bool boundingBox(float aTime0, float aTime1, AABB &aBox) const = 0;
};

class HitableList : public Hitable
{
public:
	HitableList() = default;
	HitableList(const std::vector<HitableRef> &aList);

	//! creates a shared pointer to a hitable list object
	static HitableListRef create();

	//! creates a shared pointer to a hitable list object
	static HitableListRef create(const std::vector<HitableRef> &aList);

	//! returns a const reference to the container of hitable objects
	const std::vector<HitableRef>& list() const { return mList; };

	//! returns a non-const reference to the container of hitable objects
	std::vector<HitableRef>& list() { return mList; };

	//! appends an item to the end of this object's list of hitables
	inline void push_back(const HitableRef &aHitable) { mList.push_back(aHitable); };

	//! iterates through all objects to find the closest point of intersection
	bool hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const override;

	//! computes the bounding box of the list and returns true if sucessful (not all objects have bounding boxes)
	bool boundingBox(float aTime0, float aTime1, AABB &aBox) const override;
private:
	std::vector<HitableRef> mList;
};

class Sphere : public Hitable
{
public:
	Sphere();
	Sphere(const glm::vec3 &aCenter, float aRadius, const MaterialRef &aMaterial);
	
	//! creates a shared pointer to a sphere object
	static SphereRef create(const glm::vec3 &aCenter, float aRadius, const MaterialRef &aMaterial);

	//! sphere-ray intersection test
	bool hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const override;

	//! computes the bounding box of the sphere and returns true if sucessful (not all objects have bounding boxes)
	bool boundingBox(float aTime0, float aTime1, AABB &aBox) const override;
private:
	MaterialRef mMaterial;
	glm::vec3 mCenter;
	float mRadius;
};

class MovingSphere : public Hitable
{
public:
	MovingSphere(const glm::vec3 &aCenter0, const glm::vec3 &aCenter1, float aTime0, float aTime1, float aRadius, const MaterialRef &aMaterial);

	//! creates a shared pointer to a sphere object
	static MovingSphereRef create(const glm::vec3 &aCenter0, const glm::vec3 &aCenter1, float aTime0, float aTime1, float aRadius, const MaterialRef &aMaterial);

	//! sphere-ray intersection test
	bool hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const override;

	//! returns the current location of the sphere
	glm::vec3 centerAtTime(float aTime) const;

	//! computes the bounding box of the moving sphere and returns true if sucessful (not all objects have bounding boxes)
	bool boundingBox(float aTime0, float aTime1, AABB &aBox) const override;
private:
	MaterialRef mMaterial;
	glm::vec3 mCenter0;
	glm::vec3 mCenter1;
	float mTime0;
	float mTime1;
	float mRadius;
};

class BVHNode : public Hitable
{
public:
	BVHNode() = default;
	BVHNode(const HitableListRef &aList, float aTime0, float aTime1);
	static BVHNodeRef create(const HitableListRef &aList, float aTime0, float aTime1);

	virtual bool hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const override;
	virtual bool boundingBox(float aTime0, float aTime1, AABB &aBox) const override;
private:
	//! returns a lambda to compare two object's bounding boxes along a specified axis
	std::function<bool(HitableRef, HitableRef)> comparator(size_t aAxis) const;

	HitableRef mLeft;
	HitableRef mRight;
	AABB mBox;
};
