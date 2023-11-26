#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		// glm::vec4 prvides useful names for color rendering already so its
		// not necessary to call components x y z ...
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t res = (a << 24) | (b << 16) | (g << 8) | r;
		return res;
	}
}

void Renderer::Render()
{
	// this renders every pixel we have
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coordinate = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };

			coordinate = coordinate * 2.0f - 1.0f; //coordinate range is now from -1 to 1

			glm::vec4 color = PerPixel(coordinate);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f)); // ensure that each rgba channel is between 0 and 1

			m_ImageData[ x + y * m_FinalImage->GetWidth() ] = Utils::ConvertToRGBA(color); // calculate the correct adress each pixel is stored in
			//m_ImageData[i] = 0xff00ffff; // ff=alpha, 00=blue, ff=green, ff=red
			//m_ImageData[x] = Walnut::Random::UInt();
			//m_ImageData[x] |= 0xff000000; // ensure the alpha channel is alwas at 255
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coordinate)
{
	uint8_t r = (uint8_t)(coordinate.x * 255.0f);
	uint8_t g = (uint8_t)(coordinate.y * 255.0f);
	//return Walnut::Random::UInt() | 0xff000000; // test

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDir(coordinate.x, coordinate.y, -1); // shoot the ray from the camera to the objects in z direction
	float radius = 0.5f;
	//rayDir = glm::normalize(rayDir);

	// the following equation defines the points of an intersection between a ray and a circle
	// (bx^2 + by^2)t^2 + (2axbx + 2ayby)t + (ax^2 + ay^2 - r^2) = 0
	// simplify by factoring 2 out
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// a is the origin / pixel; b is the direction; r is sphere radius; t is intersection distance

	//float a = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z; // this equals the dot product with itself
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrigin, rayDir);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;



	// calculate the discriminant of the PQ-formula: b^2 - 4ac
	float discr = b * b - 4.0f * a * c;

	if (discr < 0.0f) // ray didnt hit anything
	{
		return glm::vec4(0, 0, 0, 1); // black
	}

	// now use the PQ-formula to get points of intersection (-b (+-) sqrt(discr)) / 2a
	float tPlus = (-b + glm::sqrt(discr)) / (2.0f * a); // as a > 0 this is always bigger than calc below
	float tClosest = (-b - glm::sqrt(discr)) / (2.0f * a);

	//glm::vec3 hitPosition0 = rayOrigin + rayDir * tPlus; // not needed
	glm::vec3 hitPosition = rayOrigin + rayDir * tClosest;

	glm::vec3 sphereCol(1, 0, 1);
	sphereCol = hitPosition;

	return glm::vec4(sphereCol, 1.0f);
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
