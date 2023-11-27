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
	struct Settings
	{
		bool Accumulate = true;
		bool Multithreading = true;
	};

public:

	Renderer() = default;


	void onResize(uint32_t width, uint32_t height);

	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const 
	{
		return m_FinalImage;
	};

	void FrameCountReset()
	{
		m_FrameCount = 1;
	}

	Settings& GetSettings()
	{
		return m_Settings;
	}
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

	Settings m_Settings;

	std::vector<uint32_t> m_horizontalImgIterator;
	std::vector<uint32_t> m_verticalImgIterator;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;
	uint32_t m_FrameCount = 1; // this is the count of how many frames have been rendered for the avg
};