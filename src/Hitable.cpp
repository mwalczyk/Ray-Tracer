#include "../include/Hitable.h"

//----------------------------------------------------------------------------------
// hitable list
HitableList::HitableList(const std::vector<HitableRef> &aList) :
	mList(aList)
{
}

HitableListRef HitableList::create()
{
	return HitableListRef(new HitableList());
}

HitableListRef HitableList::create(const std::vector<HitableRef> &aList)
{
	return HitableListRef(new HitableList(aList));
}

bool HitableList::hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const
{
	HitRecord tempRecord;
	bool hitAnything = false;
	float closest = aTMax;
	for (const auto &object : mList)
	{
		if (object->hit(aRay, aTMin, closest, tempRecord))
		{
			hitAnything = true;
			closest = tempRecord.t;
			aRecord = tempRecord;
		}
	}
	
	return hitAnything;
}

bool HitableList::boundingBox(float aTime0, float aTime1, AABB &aBox) const
{
	// calculate the enclosing bounding box of the entire list
	if (mList.empty())
	{
		return false;
	}
	AABB tempBox;
	bool firstTrue = mList.at(0)->boundingBox(aTime0, aTime1, tempBox);
	if (!firstTrue)
	{
		return false;
	}
	else
	{
		aBox = tempBox;
	}
	for (size_t i = 1; i < mList.size(); ++i)
	{
		if (mList.at(i)->boundingBox(aTime0, aTime1, tempBox))
		{
			aBox = enclosingBox(aBox, tempBox);
		}
		else
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------
// sphere
Sphere::Sphere() :
	mCenter(0.0f),
	mRadius(1.0f)
{
}

Sphere::Sphere(const glm::vec3 &aCenter, float aRadius, const MaterialRef &aMaterial) :
	mCenter(aCenter),
	mRadius(aRadius),
	mMaterial(aMaterial)
{
}

SphereRef Sphere::create(const glm::vec3 &aCenter, float aRadius, const MaterialRef &aMaterial)
{
	return SphereRef(new Sphere(aCenter, aRadius, aMaterial));
}

bool Sphere::hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const
{
	// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	glm::vec3 oc{ aRay.origin() - mCenter };
	float a = glm::dot(aRay.direction(), aRay.direction());
	float b = 2.0f * glm::dot(oc, aRay.direction());
	float c = glm::dot(oc, oc) - mRadius * mRadius;
	float discriminant = b * b - 4.0f * a * c;
	
	// a positive discriminant implies one or two points of intersection
	if (discriminant > 0.0f)
	{
		// test the closest t first
		float temp = (-b - sqrtf(discriminant)) / (2.0f * a);
		if (temp > aTMin && temp < aTMax)
		{
			aRecord.t = temp;
			aRecord.position = aRay.pointAtTime(aRecord.t);
			aRecord.normal = (aRecord.position - mCenter) / mRadius;
			aRecord.material = mMaterial;
			return true;
		}

		// otherwise, check the other t
		temp = (-b + sqrtf(discriminant)) / (2.0f * a);
		if (temp > aTMin && temp < aTMax)
		{
			aRecord.t = temp;
			aRecord.position = aRay.pointAtTime(aRecord.t);
			aRecord.normal = (aRecord.position - mCenter) / mRadius;
			aRecord.material = mMaterial;
			return true;
		}
	}
	return false;
}

bool Sphere::boundingBox(float aTime0, float aTime1, AABB &aBox) const
{
	aBox = AABB(mCenter - glm::vec3(mRadius),	// min
				mCenter + glm::vec3(mRadius));	// max
	return true;
}

//----------------------------------------------------------------------------------
// moving sphere
MovingSphere::MovingSphere(const glm::vec3 &aCenter0, const glm::vec3 &aCenter1, float aTime0, float aTime1, float aRadius, const MaterialRef &aMaterial) :
	mCenter0(aCenter0),
	mCenter1(aCenter1),
	mTime0(aTime0),
	mTime1(aTime1),
	mRadius(aRadius),
	mMaterial(aMaterial)
{
}

MovingSphereRef MovingSphere::create(const glm::vec3 &aCenter0, const glm::vec3 &aCenter1, float aTime0, float aTime1, float aRadius, const MaterialRef &aMaterial)
{
	return MovingSphereRef(new MovingSphere(aCenter0, aCenter1, aTime0, aTime1, aRadius, aMaterial));
}

bool MovingSphere::hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const
{
	glm::vec3 currCenter = centerAtTime(aRay.time());
	glm::vec3 oc{ aRay.origin() - currCenter };
	float a = glm::dot(aRay.direction(), aRay.direction());
	float b = 2.0f * glm::dot(oc, aRay.direction());
	float c = glm::dot(oc, oc) - mRadius * mRadius;
	float discriminant = b * b - 4.0f * a * c;

	// a positive discriminant implies one or two points of intersection
	if (discriminant > 0.0f)
	{
		// test the closest t first
		float temp = (-b - sqrtf(discriminant)) / (2.0f * a);
		if (temp > aTMin && temp < aTMax)
		{
			aRecord.t = temp;
			aRecord.position = aRay.pointAtTime(aRecord.t);
			aRecord.normal = (aRecord.position - currCenter) / mRadius;
			aRecord.material = mMaterial;
			return true;
		}

		// otherwise, check the other t
		temp = (-b + sqrtf(discriminant)) / (2.0f * a);
		if (temp > aTMin && temp < aTMax)
		{
			aRecord.t = temp;
			aRecord.position = aRay.pointAtTime(aRecord.t);
			aRecord.normal = (aRecord.position - currCenter) / mRadius;
			aRecord.material = mMaterial;
			return true;
		}
	}
	return false;
}

glm::vec3 MovingSphere::centerAtTime(float aTime) const
{
	return mCenter0 + ((aTime - mTime0) / (mTime1 - mTime0)) * (mCenter1 - mCenter0);
}

bool MovingSphere::boundingBox(float aTime0, float aTime1, AABB &aBox) const
{
	// the bounding box of a moving sphere must enclose the sphere's
	// entire range of motion, i.e. from it's center at time 0 to it's
	// center at time 1
	auto box0 = AABB(mCenter0 - glm::vec3(mRadius),	// min
					 mCenter0 + glm::vec3(mRadius));// max
	auto box1 = AABB(mCenter1 - glm::vec3(mRadius),	// min
					 mCenter1 + glm::vec3(mRadius));// max
	aBox = enclosingBox(box0, box1);
	return true;
}

//----------------------------------------------------------------------------------
// BVH node
BVHNode::BVHNode(const HitableListRef &aList, float aTime0, float aTime1)
{
	size_t n = aList->list().size();
	
	// pick an axis to sort along
	int axis = static_cast<int>(3.0f * randFloat());

	// sort along the chosen axis
	switch (axis)
	{
	case 0:	// x
		std::sort(aList->list().begin(), aList->list().end(), comparator(0));
		break;
	case 1: // y
		std::sort(aList->list().begin(), aList->list().end(), comparator(1));
		break;
	case 2: // z
		std::sort(aList->list().begin(), aList->list().end(), comparator(2));
		break;
	default:
		break;
	}

	if (n == 1) // one element
	{
		mLeft = mRight = aList->list().at(0);
	}
	if (n == 2) // two elements
	{
		mLeft = aList->list().at(0);
		mRight = aList->list().at(1);
	}
	else // split in half
	{
		size_t half_size = aList->list().size() / 2;
		std::vector<HitableRef> lowerHalf(aList->list().begin(), aList->list().begin() + half_size);
		std::vector<HitableRef> upperHalf(aList->list().begin() + half_size, aList->list().end());

		HitableListRef low = HitableList::create(lowerHalf);
		HitableListRef high = HitableList::create(upperHalf);

		mLeft = create(low, aTime0, aTime1);
		mRight = create(high, aTime0, aTime1);
	}

	AABB boxLeft;
	AABB boxRight;
	if (!mLeft->boundingBox(aTime0, aTime1, boxLeft) || !mRight->boundingBox(aTime0, aTime1, boxRight))
	{
		std::cerr << "No bounding box in BVH node constructor." << std::endl;
	}
	mBox = enclosingBox(boxLeft, boxRight);
}

BVHNodeRef BVHNode::create(const HitableListRef &aList, float aTime0, float aTime1)
{
	return BVHNodeRef(new BVHNode(aList, aTime0, aTime1));
}

bool BVHNode::hit(const Ray &aRay, float aTMin, float aTMax, HitRecord &aRecord) const
{
	if (mBox.hit(aRay, aTMin, aTMax))
	{
		// check child nodes for a hit
		HitRecord leftRecord, rightRecord;
		bool hitLeft = mLeft->hit(aRay, aTMin, aTMax, leftRecord);
		bool hitRight = mRight->hit(aRay, aTMin, aTMax, leftRecord);
		if (hitLeft && hitRight) // pick the node with the closer t
		{
			aRecord = leftRecord.t < rightRecord.t ? leftRecord : rightRecord;
			return true;
		}
		else if (hitLeft) // hit left node?
		{
			aRecord = leftRecord;
			return true;
		}
		else if (hitRight) // hit right node?
		{
			aRecord = rightRecord;
			return true;
		}
		else // hit neither
		{
			return false;
		}
	}
	return false;
}

bool BVHNode::boundingBox(float aTime0, float aTime1, AABB &aBox) const
{
	aBox = mBox;
	return true;
}

std::function<bool(HitableRef, HitableRef)> BVHNode::comparator(size_t aAxis) const
{
	auto boxCompare = [=](const HitableRef &lhs, const HitableRef &rhs) -> bool
	{
		AABB boxLeft;
		AABB boxRight;
		if (!lhs->boundingBox(0.0f, 0.0f, boxLeft) || !rhs->boundingBox(0.0f, 0.0f, boxRight))
		{
			std::cerr << "No bounding box in BVH node constructor." << std::endl;
		}
		return boxLeft.min()[aAxis] < boxRight.min()[aAxis];
	};

	return boxCompare;
}