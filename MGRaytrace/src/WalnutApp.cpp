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
		yellowSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		yellowSphere.roughness = 0.01f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.roughness = 0.1f;

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 1.0f;
			sphere.MaterialIndex = 0; // apply the first material
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, -0.0f };
			sphere.radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
		
	}


	virtual void OnUpdate(float ts) override // ts is a timestep of the time passed from the last call of the func
	{
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override //this func gets called every frame
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.2fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::Begin("Scene");
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

		for (size_t i = 0; i < m_Scene.Materials.size(); ++i)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &material.metallic, 0.05f, 0.0f, 1.0f);

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
	Walnut::ApplicationSpecification spec;
	spec.Name = "MGRaytrace";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}