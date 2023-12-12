#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float roughness = 1.0f;
	float metallic = 0.0f;
	float transparency = 0.0f;
	float refractiveIndex = 1.33f;

	float emissionPow = 0.0f;
	glm::vec3 emissionCol{ 0.0f };

	glm::vec3 GetEmission() const
	{
		return emissionCol * emissionPow;
	}
};

struct Cube {
	glm::vec3 min; // Minimum corner of the cube
	glm::vec3 max; // Maximum corner of the cube

	glm::vec3 center;

	// Constructor to create a cube
	Cube(const glm::vec3& minCorner, const glm::vec3& maxCorner) : min(minCorner), max(maxCorner)
	{
		center = GetCenter();
	}

	// Helper function to create a cube given its center and side length
	static Cube FromCenterAndSize(const glm::vec3& center, float sideLength)
	{
		glm::vec3 halfSize = glm::vec3(sideLength / 2.0f);
		return Cube(center - halfSize, center + halfSize);
	}

	glm::vec3 GetCenter()
	{
		glm::vec3 sideLength = max - min;
		return glm::vec3(sideLength / 2.0f);
	}

	int MaterialIndex = 0;
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Cube> Cubes;
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};