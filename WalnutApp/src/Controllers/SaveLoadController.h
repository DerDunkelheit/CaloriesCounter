#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "IController.h"

class SaveLoadController : public IController
{
public:
    SaveLoadController();

    virtual void Initialize() override;

    bool IsSaveFileExist(const std::string& fileName);
    bool IsEmpty(const std::string& fileName);
    const std::filesystem::path GetPathToFile(const std::string& fileName);
    void SaveJsonToFile(const nlohmann::json& jsonFile, const std::string& fileName);
    void CreateSaveFile(const std::string& fileName);

private:
    static const std::string SAVE_FOLDER_NAME;
    static const std::string SAVE_EXTENSION;

    //TODO: use document or AppData path??
    std::filesystem::path EXE_DIRECTORY;
};
