#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"


using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{

		Material& yellowSphere = m_Scene.Materials.emplace_back();

		yellowSphere.Albedo = { 0.9f, 0.9f, 0.9f };
		yellowSphere.roughness = 0.01f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.roughness = 0.1f;

		Material& sunSphere = m_Scene.Materials.emplace_back();
		sunSphere.Albedo = { 0.8f, 0.4f, 0.2f };
		sunSphere.roughness = 0.01f;
		sunSphere.emissionCol = sunSphere.Albedo;
		sunSphere.emissionPow = 2.0f;

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 1.0f;
			sphere.MaterialIndex = 0; // apply the first material
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 2.0f, 0.0f, 0.0f };
			sphere.radius = 1.0f;
			sphere.MaterialIndex = 2; // apply the third material
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, -1001.0f, -0.0f };
			sphere.radius = 1000.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}

		glm::vec3 cubeCenter = glm::vec3{ 2.0f, 0.0f, 2.0f };
		Cube cube = Cube::FromCenterAndSize(cubeCenter, 2.0f);
		m_Scene.Cubes.push_back(cube);
		
		
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
		ImGui::Text("FPS: %.2f \nt_Frame: %.1fms", (float)(1000.0f / m_LastRenderTime), m_LastRenderTime);
		if (ImGui::Button("Reset rendering"))
		{
			m_Renderer.FrameCountReset();
		}
		ImGui::Checkbox("Ambient Occlusion", &m_Renderer.GetSettings().ambientOcclusion);
		ImGui::Checkbox("Accumulate Samples", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Text("Cubes");
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
		}
		ImGui::Text("Materials");
		for (size_t i = 0; i < m_Scene.Materials.size(); ++i)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &material.metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Transparency", &material.transparency, 0.05f, 0.0f, 1.0f);
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Transparency is still experimental");
			ImGui::ColorEdit3("Emission col", glm::value_ptr(material.emissionCol));
			ImGui::DragFloat("Emission pow", &material.emissionPow, 0.05f, 0.0f, std::numeric_limits<float>::max());

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // remove bezels from viewport side
		ImGui::Begin("Viewport");

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
	app->PushLayer<ExampleLayer>();
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
	});
	return app;
}