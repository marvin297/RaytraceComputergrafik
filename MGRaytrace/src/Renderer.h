#pragma once

#include "Walnut/Image.h"
#include <memory> // required for shared ptrs
#include <glm/glm.hpp>

class Renderer
{
public:

	Renderer() = default;


	void onResize(uint32_t width, uint32_t height);

	void Render();

	std::shared_ptr<Walnut::Image> GetFinalImage() const 
	{
		return m_FinalImage;
	};
private:
	glm::vec4 PerPixel(glm::vec2 coordinate);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};