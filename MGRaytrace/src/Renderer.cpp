#include "Renderer.h"

#include "Walnut/Random.h"

#include <iostream>

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	
	// this renders every pixel we have
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			//PerPixel(x, y);
			
			//glm::vec2 coordinate = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			//coordinate = coordinate * 2.0f - 1.0f; //coordinate range is now from -1 to 1
			//const glm::vec3& rayDir = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth() ];

			glm::vec4 color = PerPixel(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f)); // ensure that each rgba channel is between 0 and 1

			m_ImageData[ x + y * m_FinalImage->GetWidth() ] = Utils::ConvertToRGBA(color); // calculate the correct adress each pixel is stored in
			//m_ImageData[i] = 0xff00ffff; // ff=alpha, 00=blue, ff=green, ff=red
			//m_ImageData[x] = Walnut::Random::UInt();
			//m_ImageData[x] |= 0xff000000; // ensure the alpha channel is alwas at 255
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f, 0.0f, 0.0f);
	float multiplier = 1.0f;

	int bounceCount = 5;
	for (int i = 0; i < bounceCount; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);

		if (payload.hitDist < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;

			break;
		}
		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float lightInt = glm::max(glm::dot(payload.WorldNorm, -lightDir), 0.0f); // dot product = cos(angle) but only positive values

		const Sphere& sphere = m_ActiveScene->Spheres[payload.objectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		glm::vec3 sphereCol = material.Albedo;
		sphereCol *= lightInt;
		color += sphereCol * multiplier;

		multiplier *= 0.5f;

		// set the ray origin to the hit position (which is worldposition)
		// it is also required to move a minuscule amount aoutwards due to 
		// some uncertainty within floating point numbers
		// there could be a colision with the sphere itseolf directly at the start/origin
		// so adding a tini amount in the Normal's direction fixes this
		ray.Origin = payload.WorldPos + payload.WorldNorm * 0.0001f;

		// calc the reflected ray direction.
		// the incoming angle from the ray to the Normal is equal to the outgoing angle
		// this is a simple optical law in physics
		// however tis is VERY idealized assuming a perfectly flat surface (which physically cannot exist):
		//ray.Direction = glm::reflect(ray.Direction, payload.WorldNorm);

		//this is a more realistic approach:
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNorm + material.roughness * Walnut::Random::Vec3(-0.5f, 0.5));

	}


	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	// the following equation defines the points of an intersection between a ray and a circle
	// (bx^2 + by^2)t^2 + (2axbx + 2ayby)t + (ax^2 + ay^2 - r^2) = 0
	// simplify by factoring 2 out
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// a is the origin / pixel; b is the direction; r is sphere radius; t is intersection distance
	
	int closestSphere = -1;
	float hitDist = std::numeric_limits<float>::max(); // set hit distance to infinity

	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); ++i)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];

		glm::vec3 origin = ray.Origin - sphere.Position;

		//float a = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z; // this equals the dot product with itself
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

		// calculate the discriminant of the PQ-formula: b^2 - 4ac
		float discr = b * b - 4.0f * a * c;

		if (discr < 0.0f) // ray didnt hit anything
		{
			continue;
		}

		// now use the PQ-formula to get points of intersection (-b (+-) sqrt(discr)) / 2a
		//float tPlus = (-b + glm::sqrt(discr)) / (2.0f * a); // (unused) as a > 0 this is always bigger than calc above
		float tClosest = (-b - glm::sqrt(discr)) / (2.0f * a);
		if (tClosest > 0.0f && tClosest < hitDist)
		{
			hitDist = tClosest;
			closestSphere = (int)i;
		}

	}
	
	if (closestSphere < 0)
		return Missed(ray);

	return NearestHit(ray, hitDist, closestSphere);
	
}

Renderer::HitPayload Renderer::NearestHit(const Ray& ray, float hitDist, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.hitDist = hitDist;
	payload.objectIndex = objectIndex;
	
	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];
	
	glm::vec3 origin = ray.Origin - closestSphere.Position;

	payload.WorldPos = origin + ray.Direction * hitDist;
	payload.WorldNorm = glm::normalize(payload.WorldPos);
	payload.WorldPos += closestSphere.Position;
	return payload;
}

Renderer::HitPayload Renderer::Missed(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.hitDist = -1.0f; // never hit anything
	return payload;
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
