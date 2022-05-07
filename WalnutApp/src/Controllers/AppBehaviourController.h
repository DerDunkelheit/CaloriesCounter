#pragma once

#include <string>

#include "IController.h"

class AppBehaviourController : public IController
{
public:
    AppBehaviourController();

    void Initialize() override;

    void SetRenderHelpWindowBool(bool newValue);
    void SetSelectedDayIndex(int newIndex);
    void SetShouldRenderAddNewMealWindow(bool newValue);

    inline bool ShouldRenderHelpWindow() const { return mShouldRenderHelpWindow; }
    inline int GetSelectedDayIndex() const { return mSelectedDayIndex; }
    inline bool GetShouldRenderAddNewMealWindow() const { return mShouldRenderAddNewMealWindow; }

private:
    void SaveData();
    void LoadData();

private:
    static const std::string SAVE_FILE_NAME;

    bool mShouldRenderHelpWindow = false;
    bool mShouldRenderAddNewMealWindow = false;
    //TODO: marko for default int value??
    int mSelectedDayIndex = -1;

    bool mIsFirstLaunch = true;
};
