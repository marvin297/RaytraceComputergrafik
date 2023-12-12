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

struct Sphere
{
	glm::vec3 Position{0.0f};
	float radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};