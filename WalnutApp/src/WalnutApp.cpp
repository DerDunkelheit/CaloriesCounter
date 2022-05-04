#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>
#include <Walnut/Image.h>

#include "GuiUtils.h"
#include "Controllers/AppManager.h"
#include "Controllers/UserController.h"
#include "Controllers/AppBehaviourController.h"

//TODO: precompile header.
//TODO: disable docking for Layout

class ApplicationLayer : public Walnut::Layer
{
public:
    virtual void OnAttach() override
    {
        AppManager::Init();
        
        mUserController = AppManager::GetController<UserController>();
        mAppBehController = AppManager::GetController<AppBehaviourController>();

        //TODO: for testing, remove in final version.
        for (int i = 0; i < 10; i++)
        {
            mUserController->AddDay();
        }
    }

	virtual void OnUIRender() override
	{
        if (ImGui::Begin("Days statictic"))
        {
            if (ImGui::Button("Add day", ImVec2(150, 0)))
            {
                mUserController->AddDay();
            }

            // Left
            static int selected = 0;
            {
                ImGui::BeginChild("left pane", ImVec2(150, 0), true);
                for (int i = 0; i < mUserController->GetTotalDays(); i++)
                {
                    char label[128];
                    sprintf_s(label, "Day %d", i);
                    if (ImGui::Selectable(label, selected == i))
                        selected = i;
                }
                ImGui::EndChild();
            }
            ImGui::SameLine();

            // Right
            {
                ImGui::BeginGroup();
                ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                ImGui::Text("Day: %d", selected);
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Description"))
                    {
                        ImGui::TextWrapped(mUserController->GetDay(selected)->GetConsumedInfoShort().c_str());
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Details"))
                    {
                        ImGui::Text("ID: 0123456789");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndChild();
                if (ImGui::Button("Revert")) {}
                ImGui::SameLine();
                if (ImGui::Button("Save")) {}
                ImGui::EndGroup();
            }
        }
        ImGui::End();

        if (mAppBehController->ShouldRenderHelpWindow())
        {
            GuiUtils::RenderHelpWindow();
        }




#ifdef WL_DEBUG
        ImGui::ShowDemoWindow();
#endif
	}

private:
    UserController* mUserController = nullptr;
    AppBehaviourController* mAppBehController = nullptr;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Calories Counter";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ApplicationLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
            if (ImGui::MenuItem("Help window"))
            {
                AppManager::GetAppBehController()->SetRenderHelpWindowBool(true);
            }
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}