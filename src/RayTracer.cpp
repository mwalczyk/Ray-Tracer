#include "../include/Hitable.h"
#include "../include/Ray.h"
#include "../include/Camera.h"

#include <iostream>
#include <fstream>
#include <limits>

//! returns a color based on the result of intersecting the given ray with the given list of hitable objects
glm::vec3 color(const Ray &aRay, const HitableList &aList, uint32_t depth)
{
	HitRecord record;
	const float delta = 0.001f;
	const uint32_t maxDepth = 50;

	// did we hit anything?
	if (aList.hit(aRay, delta, std::numeric_limits<float>::max(), record))
	{
		Ray scattered;
		glm::vec3 attenuation;
		
		// calculate the scattered ray based on the material properties at the hit location
		if (depth < maxDepth && record.material->scatter(aRay, record, attenuation, scattered))
		{
			return attenuation * color(scattered, aList, ++depth);
		}
		else
		{
			return glm::vec3(0.0f);
		}
	}
	else 
	{
		glm::vec3 unitDirection = glm::normalize(aRay.direction());

		// remap the y-coord of the ray direction to 0...1
		float t = 0.5f * (unitDirection.y + 1.0f);
		const glm::vec3 white{ 1.0f };
		const glm::vec3 sky{ 0.5f, 0.7f, 1.0f };
		return lerp(white, sky, t);
	}
}

HitableListRef randomScene()
{
	const size_t n = 500;
	HitableListRef scene = HitableList::create();
	//scene->list().resize(3);

	//scene.list().resize(500);

	//MaterialRef groundMat = std::make_shared<Lambertian>(glm::vec3(0.5f));
	//scene.list()[0] = std::make_shared<Sphere>(glm::vec3(0.0f, -1000.f, 0.0f), 1000.0f, groundMat);

	auto groundMat = std::make_shared<Lambertian>(glm::vec3(0.5f));
	scene->push_back(Sphere::create(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, groundMat));

	auto mat1 = std::make_shared<Dieletric>(1.5);
	scene->push_back(Sphere::create(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, mat1));

	auto mat2 = std::make_shared<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1));
	scene->push_back(Sphere::create(glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, mat2));

	auto mat3 = std::make_shared<Metallic>(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f);
	scene->push_back(Sphere::create(glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, mat3));

	//for (size_t i = 0; i < 20; ++i)
	//{
	//	auto randMat = std::make_shared<Lambertian>(glm::vec3(randFloat(), randFloat(), randFloat()));
	//	float x = (randFloat() - 0.5f) * 10.0f;
	//	float z = (randFloat() - 0.5f) * 10.0f;
	//	glm::vec3 randCenter{x, 0.2f, z};
	//	scene->push_back(MovingSphere::create(randCenter, randCenter + glm::vec3(0.0f, 0.5f, 0.0f), 0.0f, 1.0f, 0.2f, randMat));
	//}

	return scene;
}

int main()
{
	// output a dummy ppm image file:
	// P3 means the colors are in ASCII
	// 3 columns and 2 rows
	// 255 is the max color
	const uint32_t width = 200;
	const uint32_t height = 100;
	const uint32_t ns = 1;
	const uint32_t maxColor = 255;
	const float gamma = 2.2f;

	std::ofstream outFile("test.ppm");
	outFile << "P3\n" << width << " " << height << "\n" << maxColor << "\n";

	// scene
	const float r = 0.5f; 
	HitableListRef scene = randomScene();
	BVHNodeRef sceneHierarchy = BVHNode::create(scene, 0.0f, 0.0f);

	// camera
\	glm::vec3 eyePos(13.0f, 2.0f, 3.0f);
	glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	float aspectRatio = static_cast<float>(width) / height;
	float focusDistance = 10.0;
	Camera camera{ eyePos, lookAt, up, aspectRatio, focusDistance, 20.0f, 0.0f, 0.0f, 1.0f };

	// actual pixel data
	for (int j = height - 1; j >= 0; --j)	// 99 to 0
	{
		for (int i = 0; i < width; ++i)	// 0 to 200
		{
			glm::vec3 accumColor{ 0.0f };

			// perform anti-aliasing by taking multiple samples 
			for (int samp = 0; samp < ns; ++samp)
			{
				// jitter the position by a small amount
				float u = float(i + randFloat()) / float(width);
				float v = float(j + randFloat()) / float(height);
				Ray ray = camera.generateRay(u, v);

				// accumulate the total color contributions
				accumColor += color(ray, scene->list(), 0.0f);
			}
			accumColor /= ns;

			// gamma correction
			accumColor = glm::vec3(powf(accumColor.r, 1.0f / gamma), powf(accumColor.g, 1.0f / gamma), powf(accumColor.b, 1.0f / gamma));

			int ir = static_cast<int>(255.99 * accumColor.r);
			int ig = static_cast<int>(255.99 * accumColor.g);
			int ib = static_cast<int>(255.99 * accumColor.b);
			outFile << ir << " " << ig << " " << ib << "\n";
		}
	}
	return 0;
}

