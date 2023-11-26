#include "Renderer.h"

#include "Walnut/Random.h"

void Renderer::Render()
{
	// this renders every pixel we have
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coordinate = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };

			m_ImageData[ x + y * m_FinalImage->GetWidth() ] = PerPixel(coordinate); // calculate the correct adress each pixel is stored in
			//m_ImageData[i] = 0xff00ffff; // ff=alpha, 00=blue, ff=green, ff=red
			//m_ImageData[x] = Walnut::Random::UInt();
			//m_ImageData[x] |= 0xff000000; // ensure the alpha channel is alwas at 255
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coordinate)
{
	//uint8_t r = (uint8_t)coordinate.x * 255.0f;
	uint8_t r = (uint8_t)(coordinate.x * 255.0f);
	//uint8_t g = (uint8_t)coordinate.y * 255.0f;
	uint8_t g = (uint8_t)(coordinate.y * 255.0f);
	
	//return Walnut::Random::UInt() | 0xff000000; // test
	return 0xff000000 | (g << 8) | r;
}

void Renderer::onResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage) // if the image doesnt exist create a new one
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) // if image is right size, abort
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[height * width]; // the rgba format uses 1 byte per channel so 1px = 1 uint32_T
	
}
