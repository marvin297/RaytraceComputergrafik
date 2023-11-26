#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override //this func gets called every frame
	{
		ImGui::Begin("Settings");

		ImGui::Text("Last render: %.2fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
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
		m_Renderer.Render();

		m_LastRenderTime = timer.ElapsedMillis();

	}

private: 
	Renderer m_Renderer;
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