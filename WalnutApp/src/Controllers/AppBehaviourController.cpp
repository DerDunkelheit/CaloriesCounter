#include "pch.h"

#include "AppBehaviourController.h"
#include "AppManager.h"
#include "SaveLoadController.h"


const std::string AppBehaviourController::SAVE_FILE_NAME = "AppBehaviourData";

AppBehaviourController::AppBehaviourController()
{
    
}

void AppBehaviourController::Initialize()
{
    LoadData();

    if (mIsFirstLaunch)
    {
        SetRenderHelpWindowBool(true);
    }

    std::cout << "AppBehaviourController is Initialized" << std::endl;
}

void AppBehaviourController::SetRenderHelpWindowBool(bool newValue)
{
    mShouldRenderHelpWindow = newValue;
    SaveData();
}

void AppBehaviourController::SetSelectedDayIndex(int newIndex)
{
    //TODO:: checking for wrong input???
    mSelectedDayIndex = newIndex;
    SaveData();
}

void AppBehaviourController::SetShouldRenderAddNewMealWindow(bool newValue)
{
    mShouldRenderAddNewMealWindow = newValue;
    SaveData();
}

void AppBehaviourController::SaveData()
{
    nlohmann::json save;
    save["shouldRenderHelpWindow"] = mShouldRenderHelpWindow;
    save["selectedDayIndex"] = mSelectedDayIndex;
    save["firstLaunch"] = false;
    SaveLoadController* saveLoad = AppManager::GetController<SaveLoadController>();
    if (saveLoad->IsSaveFileExist(SAVE_FILE_NAME))
    {
        saveLoad->SaveJsonToFile(save, SAVE_FILE_NAME);
    }
    else
    {
        saveLoad->CreateSaveFile(SAVE_FILE_NAME);
        saveLoad->SaveJsonToFile(save, SAVE_FILE_NAME);
    }
}

void AppBehaviourController::LoadData()
{
    using namespace nlohmann;

    SaveLoadController* saveLoad = AppManager::GetController<SaveLoadController>();
    if (saveLoad->IsSaveFileExist(SAVE_FILE_NAME) && !saveLoad->IsEmpty(SAVE_FILE_NAME))
    {
        //TODO: implement save logic.
        //TODO: try get jsonParse error
        //TODO: test for existence of property
        //TODO: maybe replace it to save/load controller
        std::ifstream file(saveLoad->GetPathToFile(SAVE_FILE_NAME));
        json j = json::parse(file);
        mShouldRenderHelpWindow = j["shouldRenderHelpWindow"].get<bool>();
        mSelectedDayIndex = j["selectedDayIndex"].get<int>();
        mIsFirstLaunch = j["firstLaunch"].get<bool>();
        file.close();
    }
}
