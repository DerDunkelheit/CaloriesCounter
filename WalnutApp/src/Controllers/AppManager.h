#pragma once

#include <iostream>
#include <vector>

class IController;
class UserController;
class AppBehaviourController;

class AppManager
{
public:
    AppManager(const AppManager&) = delete;

    static void Init();

    static AppManager& Get()
    {
        static AppManager instance;
        return instance;
    }

    //TODO: const return type???
    template<typename T>
    static T* GetController()
    {
        for(IController* controller : Get().mControllers)
        {
            T* dyn = dynamic_cast<T*>(controller);
            if (dyn != nullptr)
            {
                return dyn;
            }
        }

        return nullptr;
    }

    static AppBehaviourController* GetAppBehController()
    {
        return Get().mAppBehaviourController;
    }

    static UserController* GetUserController()
    {
        return Get().mUserController;
    }

private:
    AppManager() {};

    std::vector<IController*> mControllers;
    //Put here controllers, that are frequently used

    UserController* mUserController = nullptr;
    AppBehaviourController* mAppBehaviourController = nullptr;

};
