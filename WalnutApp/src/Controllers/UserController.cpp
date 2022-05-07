#include "pch.h"

#include <nlohmann/json.hpp>

#include "UserController.h"
#include "SaveLoadController.h"
#include "AppManager.h"
#include "Data/Day.h"

const std::string UserController::SAVE_FILE_NAME = "UserControllerData";

UserController::UserController()
{

}

void UserController::Initialize()
{
    //Load data
    LoadData();

    std::cout<<"UserController is initialized" << std::endl;
}

void UserController::AddDay()
{
    mTotalDays.push_back(Day());

    SaveData();
}

void UserController::RemoveDayByIndex(int index)
{
    if(index < 0 || index > mTotalDays.size())
    {
        //TODO: create custom asserts
    }

    mTotalDays.erase(mTotalDays.begin() + index);

    SaveData();
}

int UserController::GetTotalDays() const
{
    return static_cast<int>(mTotalDays.size());
}

const std::vector<Day>& UserController::GetAllDays() const
{
    return mTotalDays;
}

Day* UserController::GetDay(int index)
{
    if (index < mTotalDays.size())
    {
        return &mTotalDays[index];
    }

    return nullptr;
}

void UserController::LoadData()
{
    using namespace nlohmann;

    SaveLoadController* saveLoad = AppManager::GetController<SaveLoadController>();
    if (saveLoad->IsSaveFileExist(SAVE_FILE_NAME) && !saveLoad->IsEmpty(SAVE_FILE_NAME))
    {
        std::ifstream file(saveLoad->GetPathToFile(SAVE_FILE_NAME));
        json j = json::parse(file);
        for (const auto& element : j["Days"])
        {
            Day parsedDay;
            // if eleement is null, it means day contains no meals.
            if (element != nullptr)
            {
                for (const auto& meal : element["Meals"])
                {
                    int proteins = meal["Proteins"].get<int>();
                    int carbohydrates = meal["Carbohydrates"].get<int>();
                    int fats = meal["Fats"].get<int>();
                    parsedDay.AddMeal(proteins, carbohydrates, fats);
                }
            }

            mTotalDays.push_back(parsedDay);
        }
    }
}

void UserController::SaveData()
{
    nlohmann::json save;
    for (Day& day : mTotalDays)
    {
        nlohmann::json dayJson;

        if (day.GetMealCout() > 0)
        {
            for (const Meal& meal : day)
            {
                dayJson["Meals"].push_back({ {"Carbohydrates",meal.GetCarbohydrates()}, {"Proteins",meal.GetProteins()}, {"Fats",meal.GetFats()} });
            }
        }

        save["Days"].push_back(dayJson);
    }

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
