#include "pch.h"

#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>
#include <Walnut/Image.h>

#include "GuiUtils.h"
#include "Controllers/AppManager.h"
#include "Controllers/UserController.h"
#include "Controllers/AppBehaviourController.h"

#include "Debug/DebugData.h"

//TODO: disable docking for Layout
//TODO: display days from 1 instead of 0

class ApplicationLayer : public Walnut::Layer
{
public:
    ApplicationLayer(DebugData* debugData)
    {
        mDebugData = debugData;
    }

    virtual void OnAttach() override
    {
        AppManager::Init();
        
        mUserController = AppManager::GetController<UserController>();
        mAppBehController = AppManager::GetController<AppBehaviourController>();
    }

	virtual void OnUIRender() override
	{
        static int selected = 0;
        if (ImGui::Begin("Days statictic"))
        {
            if (ImGui::Button("Add day", ImVec2(150, 0)))
            {
                mUserController->AddDay();
            }

            // Left
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
                    Day* currentDay = mUserController->GetDay(selected);

                    if (ImGui::BeginTabItem("Description"))
                    {
                        if (currentDay != nullptr)
                        {
                            ImGui::TextWrapped(currentDay->GetConsumedInfoShort().c_str());

                            //TODO: prevent negative int instead of this.
                            static int proteins = 0;
                            ImGui::PushItemWidth(200);
                            ImGui::InputInt("Add Proteins", &proteins);
                            static int carbohydrates = 0;
                            ImGui::PushItemWidth(200);
                            ImGui::InputInt("Add Carbohydrates", &carbohydrates);
                            static int fats = 0;
                            ImGui::PushItemWidth(200);
                            ImGui::InputInt("Add Fats", &fats);

                            if (ImGui::Button("Add meal"))
                            {
                                if (proteins > 0 || carbohydrates > 0 || fats > 0)
                                {
                                    currentDay->AddMeal(proteins, carbohydrates, fats);
                                    mUserController->SaveData();

                                    proteins = 0;
                                    carbohydrates = 0;
                                    fats = 0;
                                }
                                else
                                {
                                    std::cout << "Invalid Input" << std::endl;
                                }

                                std::cout << currentDay->GetMealCout() << std::endl;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Remove last meal"))
                            {
                                if (currentDay->GetMealCout() > 0)
                                {
                                    currentDay->RemoveLastMeal();
                                    mUserController->SaveData();
                                }
                            }
                        }

                        ImGui::EndTabItem();
                        
                    }
                    if (ImGui::BeginTabItem("All Days stat"))
                    {
                        if (currentDay != nullptr)
                        {
                            // When using ScrollX or ScrollY we need to specify a size for our table container!
                            // Otherwise by default the table will fit all available space, like a BeginChild() call.
                            static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                            ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
                            if (ImGui::BeginTable("table_scrolly", 5, flags, outer_size))
                            {
                                ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
                                ImGui::TableSetupColumn("Days", ImGuiTableColumnFlags_None);
                                ImGui::TableSetupColumn("Total calories", ImGuiTableColumnFlags_None);
                                ImGui::TableSetupColumn("Proteins", ImGuiTableColumnFlags_None);
                                ImGui::TableSetupColumn("Carbohydrates", ImGuiTableColumnFlags_None);
                                ImGui::TableSetupColumn("Fats", ImGuiTableColumnFlags_None);
                                ImGui::TableHeadersRow();

                                // Demonstrate using clipper for large vertical lists
                                ImGuiListClipper clipper;
                                clipper.Begin(static_cast<int>(mUserController->GetAllDays().size()));
                                while (clipper.Step())
                                {
                                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                                    {
                                        ImGui::TableNextRow();
                                        for (int column = 0; column < 5; column++)
                                        {
                                            ImGui::TableSetColumnIndex(column);
                                            switch (column)
                                            {
                                                case 0:
                                                {
                                                    ImGui::Text("Day %d", row);
                                                    ImGui::SameLine();
                                                    ImGui::Button("copy");
                                                    if (ImGui::IsItemClicked())
                                                    {
                                                        std::cout << row << std::endl;
                                                        const Day& copyDay = mUserController->GetAllDays()[row];
                                                        ImGui::LogToClipboard();
                                                        ImGui::LogText("Day:%d, Calories: %d, Proteins: %d, Carbohydrates: %d, Fats: %d", row, copyDay.GetTotalCalories(), copyDay.GetConsumedProteins(), copyDay.GetConsumedCarbohydrates(), copyDay.GetConsumedFats());
                                                        ImGui::LogFinish();
                                                    }
                                                    break;
                                                }
                                                case 1:
                                                {
                                                    ImGui::Text("Calories: %d", mUserController->GetAllDays()[row].GetTotalCalories());
                                                    break;
                                                }
                                                case 2:
                                                {
                                                    ImGui::Text("Proteins: %d", mUserController->GetAllDays()[row].GetConsumedProteins());
                                                    break;
                                                }
                                                case 3:
                                                {
                                                    ImGui::Text("Carbohydrates: %d", mUserController->GetAllDays()[row].GetConsumedCarbohydrates());
                                                    break;
                                                }
                                                case 4:
                                                {
                                                    ImGui::Text("Fats: %d", mUserController->GetAllDays()[row].GetConsumedFats());
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                ImGui::EndTable();
                            }
                        }

                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndChild();
                if (ImGui::Button("Remove Day"))
                {
                    ImGui::OpenPopup("Delete?");
                }

                GuiUtils::RenderRemoveDayPopup("Delete?", [this]() 
                    {
                        return mUserController->RemoveDayByIndex(selected);
                    });

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
        GuiUtils::RenderDebugWindow(mDebugData, selected, *mUserController);
#endif
	}

    virtual void OnDetach()
    {
        delete(mUserController);
        delete(mAppBehController);
        delete(mDebugData);
    }

private:
    UserController* mUserController = nullptr;
    AppBehaviourController* mAppBehController = nullptr;

    DebugData* mDebugData = nullptr;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Calories Counter";

	Walnut::Application* app = new Walnut::Application(spec);
    DebugData* debugData = new DebugData();
    std::shared_ptr<ApplicationLayer> appLayer = std::make_shared<ApplicationLayer>(debugData);
	app->PushLayer(appLayer);
	app->SetMenubarCallback([app, debugData]()
	{
		if (ImGui::BeginMenu("File"))
		{
            if (ImGui::MenuItem("Import stats"))
            {
                GuiUtils::OpenFile();
            }
            if (ImGui::MenuItem("Export stats"))
            {
                //TODO: export stats in csv format.
            }
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Help window"))
            {
                AppManager::GetAppBehController()->SetRenderHelpWindowBool(true);
            }
            ImGui::EndMenu();
        }
#ifdef WL_DEBUG
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Debug Window"))
            {
                debugData->RenderWindow = true;
            }
            ImGui::EndMenu();
        }
#endif
	});
	return app;
}