#include "pch.h"

#include <tchar.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "imgui.h"
#include "GuiUtils.h"

#include "Controllers/AppManager.h"
#include "Controllers/AppBehaviourController.h"
#include "Data/Day.h"

//TODO: callback for importing csv file.
void GuiUtils::OpenFile()
{
    wchar_t filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = _T("Text Files\0*.txt\0Any File\0*.*\0");
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = _T("Select a File");
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        std::wcout << "You chose the file \"" << filename << "\"\n";
    }
    else
    {
        // All this stuff below is to tell you exactly how you messed up above. 
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }
}

void GuiUtils::RenderHelpWindow()
{
    ImGui::Begin("Help window", nullptr, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking));

    ImGui::Text("some text, user guide i'm alex, what to write idk just test \nsome new text on new line\none more line");
    ImGui::NewLine();

    ImGui::SameLine(150);
    if (ImGui::Button("Close", ImVec2(100, 0)))
    {
        AppManager::GetAppBehController()->SetRenderHelpWindowBool(false);
    }

    ImGui::End();
}

void GuiUtils::RenderDebugWindow(DebugData* debugData, int selectedDayIndex, UserController& userController)
{
    if (debugData->RenderWindow)
    {
        ImGui::Begin("DebugWindow", &debugData->RenderWindow, ImGuiWindowFlags_NoCollapse);
            if(ImGui::Button("Add 10 Days"))
            {
                for (int i = 0; i < 10; i++)
                {
                    userController.AddDay();
                }
            }

            Day* selectedDay = userController.GetDay(selectedDayIndex);
            if (selectedDay != nullptr)
            {
                ImGui::Text("Selected Day index %d", selectedDayIndex);
                ImGui::SameLine();
                if (ImGui::Button("Add Meal"))
                {
                    selectedDay->AddMeal(50, 50, 50);
                }
            }

        ImGui::End();
    }
}

void GuiUtils::RenderRemoveDayPopup(const char* popupName, std::function<void()> positiveCallback)
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(popupName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure want to delete selected day?\nThis operation cannot be undone!\n\n");
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            positiveCallback();
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}
