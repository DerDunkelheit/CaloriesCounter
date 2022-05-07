#pragma once

#include "Debug/DebugData.h"
#include "Controllers/UserController.h"

class Day;

namespace GuiUtils
{
    void OpenFile();

    void RenderHelpWindow();
    void RenderDebugWindow(DebugData* debugData, int selectedDayIndex, UserController& userController);
}
