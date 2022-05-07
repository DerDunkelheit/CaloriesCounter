#pragma once

#include <vector>

#include "IController.h"
#include "Data/Day.h"

class UserController : public IController
{
public:
    UserController();

    virtual void Initialize() override;

    void AddDay();
    void RemoveDayByIndex(int index);
    int GetTotalDays() const;
    const std::vector<Day>& GetAllDays() const;
    Day* GetDay(int index);

    //Test
    Day& GetFirstDay() { return mTotalDays[0]; }

    void SaveData();
private:
    void LoadData();

private:
    static const std::string SAVE_FILE_NAME;

    std::vector<Day> mTotalDays;
};
