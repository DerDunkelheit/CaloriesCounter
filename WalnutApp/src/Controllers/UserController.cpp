#include "pch.h"

#include "UserController.h"
#include "Data/Day.h"

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
}

void UserController::RemoveDayByIndex(int index)
{
    if(index < 0 || index > mTotalDays.size())
    {
        //TODO: create custom asserts
    }

    mTotalDays.erase(mTotalDays.begin() + index);
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

}

void UserController::SaveData()
{
    //Create persistance controller
    //Create Directory managment in controller(define root saving folder, etc)
   
}
