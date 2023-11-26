#pragma once

#include "Walnut/Image.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include <memory> // required for shared ptrs
#include <glm/glm.hpp>

class Renderer
{
public:

	Renderer() = default;


	void onResize(uint32_t width, uint32_t height);

	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const 
	{
		return m_FinalImage;
	};
private:
	struct HitPayload
	{
		float hitDist;
		int objectIndex;
		glm::vec3 WorldNorm;
		glm::vec3 WorldPos;
	};

	// this is going to implement a raygen shader similar to vulkan
	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	HitPayload NearestHit(const Ray& ray, float hitDist, int objectIndex);
	HitPayload Missed(const Ray& ray);
	HitPayload TraceRay(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
};