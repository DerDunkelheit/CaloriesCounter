#pragma once

class IController
{
public:
    virtual ~IController() {};
    virtual void Initialize() = 0;
};