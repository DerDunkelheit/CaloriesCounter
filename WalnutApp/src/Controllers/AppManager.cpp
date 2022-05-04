#include "AppManager.h"
#include "UserController.h"
#include "AppBehaviourController.h"
#include "SaveLoadController.h"

void AppManager::Init()
{
    std::vector<IController*>& controllers = Get().mControllers;
    controllers.push_back(new UserController());
    controllers.push_back(new AppBehaviourController());
    //TODO: maybe come up with something.
    Get().mUserController = dynamic_cast<UserController*>(controllers[0]);
    Get().mAppBehaviourController = dynamic_cast<AppBehaviourController*>(controllers[1]);
    controllers.push_back(new SaveLoadController());

    for (IController* controller : controllers)
    {
        controller->Initialize();
    }

    std::cout << "AppManager is initialized" << std::endl;
}
