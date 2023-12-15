#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"


using namespace Walnut;

class RaytraceScene : public Walnut::Layer
{
public:
	RaytraceScene()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{

		Material& goldMaterial = m_Scene.Materials.emplace_back();
		goldMaterial.Albedo = { 0.80f, 0.40f, 0.05f };
		goldMaterial.roughness = 0.01f;
		goldMaterial.metallic = 0.7f;

		Material& silver = m_Scene.Materials.emplace_back();
		silver.Albedo = { 0.98f, 0.98f, 0.98f };
		silver.roughness = 0.01f;
		silver.metallic = 0.9f;

		Material& glass = m_Scene.Materials.emplace_back();
		glass.Albedo = { 0.9f, 0.9f, 0.9f };
		glass.roughness = 0.0f;
		glass.metallic = 0.0f;
		glass.transparency = 0.9f;

		Material& pinkTitanium = m_Scene.Materials.emplace_back();
		pinkTitanium.Albedo = { 0.999f, 0.0f, 0.98f };
		pinkTitanium.roughness = 0.0f;
		pinkTitanium.metallic = 0.999f;

		Material& bluePlastic = m_Scene.Materials.emplace_back();
		bluePlastic.Albedo = { 0.1f, 0.2f, 0.95f };
		bluePlastic.roughness = 0.99f;
		bluePlastic.metallic = 0.0f;
		bluePlastic.transparency = 0.0f;

		Material& redPlastic = m_Scene.Materials.emplace_back();
		redPlastic.Albedo = { 0.99f, 0.1f, 0.0f };
		redPlastic.roughness = 0.90f;
		redPlastic.metallic = 0.0f;
		redPlastic.transparency = 0.0f;
		//-----------------------------------------------------------
		Material& mirror = m_Scene.Materials.emplace_back();
		mirror.Albedo = { 0.9f, 0.9f, 0.9f };
		mirror.roughness = 0.0f;
		mirror.metallic = 0.999f;
		mirror.transparency = 0.0f;

		Material& sunMaterial = m_Scene.Materials.emplace_back();
		sunMaterial.Albedo = { 0.8f, 0.4f, 0.2f };
		sunMaterial.roughness = 0.01f;
		sunMaterial.emissionCol = sunMaterial.Albedo;
		sunMaterial.emissionPow = 6.0f;

		Material& floor = m_Scene.Materials.emplace_back();
		floor.Albedo = { 0.5f, 0.5f, 0.5f };
		floor.roughness = 0.1f;


		const glm::vec3 positions[] = {
			glm::vec3(6.0, 0.0, 2.0),
			glm::vec3(-4.0, 0.0, 4.0),
			glm::vec3(8.0, 0.0, -6.0),
			glm::vec3(-3.0, 0.0, -2.0),
			glm::vec3(8.0, 3.0, 5.0),
			glm::vec3(5.0, 0.0, -4.0),
			glm::vec3(3.0, 0.0,-4.0),
			glm::vec3(6.5, 0.0, 5.0),
			glm::vec3(16.0, 0.0, 12.0),
			glm::vec3(-14.0, 0.0, 4.0),
			glm::vec3(18.0, 0.0, -6.0),
			glm::vec3(-13.0, 0.0, -2.0),
			glm::vec3(18.0, 0.0, 5.0),
			glm::vec3(13.0, 0.0, 9.0)
		};

		for (size_t i = 0; i < sizeof(positions)/sizeof(positions[0]); i++)
		{
			Sphere sphere;
			sphere.Position = positions[i];
			sphere.radius = 1.0f;
			sphere.MaterialIndex = i % 6;
			//sphere.MaterialIndex = i % m_Scene.Materials.size() - 2; // leave floor and sun material out
			m_Scene.Spheres.push_back(sphere);
		}
		
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 1.0f;
			sphere.MaterialIndex = 0; // apply the first material
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere virtualSun;
			virtualSun.Position = { -80.0f, 30.0f, -80.0f };
			virtualSun.radius = 50.0f;
			virtualSun.MaterialIndex = 7; // apply the sun material
			m_Scene.Spheres.push_back(virtualSun);
		}

		{
			//this is the floor
			Sphere floor;
			floor.Position = { 0.0f, -1001.0f, -0.0f };
			floor.radius = 1000.0f;
			floor.MaterialIndex = 8;
			m_Scene.Spheres.push_back(floor);
		}

		int materials[] = { 2, 6, 3, 4}; // specify which materials to use
		float sizes[] = { 2.0f, 3.0f , 2.5f, 1.0f}; // specify the sizes
		for (size_t i = 0; i < 4; i++)
		{
			glm::vec3 cubeCenter = glm::vec3{ 10.0f * (float)i, sizes[i]/2.0f - 1.2f, 2.0f - (float)i * (float)i };
			Cube cube = Cube::FromCenterAndSize(cubeCenter, sizes[i]);
			cube.MaterialIndex = materials[i];
			m_Scene.Cubes.push_back(cube);
		}
		
		
	}


	virtual void OnUpdate(float ts) override // ts is a timestep of the time passed from the last call of the func to the current call
	{
		if (m_Camera.OnUpdate(ts))
		{
			m_Renderer.FrameCountReset();
		}
	}

	virtual void OnUIRender() override //this func gets called every frame
	{
		ImGui::Begin("Settings");
		ImGui::Text("FPS: %.0f \nt_Frame: %.3fms", (float)(1000.0f / m_LastRenderTime), m_LastRenderTime);
		if (ImGui::Button("Reset rendering"))
		{
			m_Renderer.FrameCountReset();
		}
		ImGui::Checkbox("Ambient Occlusion", &m_Renderer.GetSettings().ambientOcclusion);
		ImGui::Checkbox("Accumulate Samples", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::End();

		ImGui::Begin("Scene");
		/*ImGui::Text("Cubes");
		for (size_t i = 0; i < m_Scene.Cubes.size(); ++i)
		{
			ImGui::PushID(i);
			
			Cube& cube = m_Scene.Cubes[i];
			ImGui::DragFloat3("Pos", glm::value_ptr(cube.center), 0.1f);
			//ImGui::DragFloat("Rad", &sphere.radius, 0.1f);
			//ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);
			
			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::Text("Spheres");
		for (size_t i = 0; i < m_Scene.Spheres.size(); ++i)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Pos", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Rad", &sphere.radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

			ImGui::Separator();

			ImGui::PopID();
		}*/
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Artificial Sun");
		ImGui::DragFloat("Emission power", &m_Scene.Materials[7].emissionPow, 0.05f, 0.0f, 60.0f);
		ImGui::ColorEdit3("Emission color", glm::value_ptr(m_Scene.Materials[7].emissionCol));
		ImGui::Separator();

		ImGui::Text("Materials");
		for (size_t i = 0; i < m_Scene.Materials.size() - 2; ++i)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &material.metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Transparency", &material.transparency, 0.05f, 0.0f, 1.0f);
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Transparency uses more CPU!");
			ImGui::ColorEdit3("Emission col", glm::value_ptr(material.emissionCol));
			ImGui::DragFloat("Emission pow", &material.emissionPow, 0.05f, 0.0f, std::numeric_limits<float>::max());

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // remove bezels from viewport side
		ImGui::Begin("Renderer Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image) {
			//check if there is even a image, otherwise it will crash as it tries to render at nullptr
			ImGui::Image
			(
				//the last two ImVec2's just reverse the way the image is being displayed
				image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0)
			);
		}

		ImGui::End();
		ImGui::PopStyleVar();

		Render(); // continuously render frames
	}

	void Render()
	{
		Timer timer; // monitor frametimes with timer object

		m_Renderer.onResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();

	}

private: 
	Renderer m_Renderer;
	Camera m_Camera; // create a camera object from the external camera class
	Scene m_Scene;

	uint32_t* m_ImageData = nullptr;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification appSpecification;
	appSpecification.Name = "Raytrace Project by Marvin Geiger";

	Walnut::Application* app = new Walnut::Application(appSpecification);
	app->PushLayer<RaytraceScene>();
	app->SetMenubarCallback([app]()
	{
		ImVec4 darkRed = ImVec4(0.20f, 0.08f, 0.08f, 1.00f);
		ImVec4 brightRed = ImVec4(0.92f, 0.30f, 0.30f, 1.00f);
		ImVec4 brighterRed = ImVec4(0.99f, 0.64f, 0.64f, 1.00f);

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Button] = darkRed;
		colors[ImGuiCol_FrameBg] = darkRed;
		colors[ImGuiCol_ButtonHovered] = brightRed;
		colors[ImGuiCol_FrameBgHovered] = brightRed;
		colors[ImGuiCol_ButtonActive] = brighterRed;
		colors[ImGuiCol_FrameBgActive] = brighterRed;
		colors[ImGuiCol_CheckMark] = brighterRed;

		colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1f, 0.1f, 1.00f);;

		colors[ImGuiCol_Header] = darkRed;
		colors[ImGuiCol_HeaderHovered] = brightRed;
		colors[ImGuiCol_HeaderActive] = brighterRed;

		colors[ImGuiCol_TabUnfocused] = darkRed;
		colors[ImGuiCol_TabHovered] = brightRed;
		colors[ImGuiCol_TabActive] = brightRed;
		colors[ImGuiCol_TabUnfocusedActive] = darkRed;

		ImGui::GetStyle().FrameRounding = 4.0f;

		if (ImGui::BeginMenu("More"))
		{
			if (ImGui::MenuItem("Exit to Desktop"))
			{
				app->Close();
			}
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Raytrace Project by Marvin Geiger");
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "https://github.com/marvin297");
			ImGui::EndMenu();
		}
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.99f, 0.64f, 0.64f, 1.00f), "Raytrace Project by Marvin Geiger @ DHBW Stuttgart");
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.99f, 0.64f, 0.64f, 1.00f), "13th October 2023 until 15th December 2023");
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.00f), "(c) Copyright Marvin Geiger 2023");
	});
	return app;
}