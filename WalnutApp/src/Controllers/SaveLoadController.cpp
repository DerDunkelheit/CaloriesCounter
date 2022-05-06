#include "pch.h"

#include "SaveLoadController.h"


const std::string SaveLoadController::SAVE_FOLDER_NAME = "SavedData";
const std::string SaveLoadController::SAVE_EXTENSION = ".json";

SaveLoadController::SaveLoadController()
{
    //Path to exe.//TODO: get it from plfatform specific interface.
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string fullPath = std::string(buffer);
    std::string::size_type pos = fullPath.find_last_of("\\/");
    std::string exeLocationPath = fullPath.substr(0, pos);

    EXE_DIRECTORY = exeLocationPath;
}

void SaveLoadController::Initialize()
{
    if (!std::filesystem::exists(EXE_DIRECTORY / SAVE_FOLDER_NAME))
    {
        std::filesystem::create_directory(EXE_DIRECTORY / SAVE_FOLDER_NAME);

        std::cout << "Save directory is Created" << std::endl;
    }
    //std::ofstream outfile(finalPath + "/TestFolder/NestedFolder/testTxt.txt");
    //  outfile << "File created" << std::endl;
}

bool SaveLoadController::IsSaveFileExist(const std::string& fileName)
{
    return (std::filesystem::exists(EXE_DIRECTORY / SAVE_FOLDER_NAME / fileName += SAVE_EXTENSION));
}

bool SaveLoadController::IsEmpty(const std::string& fileName)
{
    return (std::filesystem::is_empty(EXE_DIRECTORY / SAVE_FOLDER_NAME / fileName += SAVE_EXTENSION));
}

const std::filesystem::path SaveLoadController::GetPathToFile(const std::string& fileName)
{
    //TODO: check for valid input?
    return EXE_DIRECTORY / SAVE_FOLDER_NAME / fileName += SAVE_EXTENSION;
}

void SaveLoadController::SaveJsonToFile(const nlohmann::json& jsonFile, const std::string& fileName)
{
    std::ofstream out(EXE_DIRECTORY / SAVE_FOLDER_NAME / fileName += SAVE_EXTENSION);
    out << jsonFile;
    out.close();
}

void SaveLoadController::CreateSaveFile(const std::string& fileName)
{
    std::ofstream outfile(EXE_DIRECTORY / SAVE_FOLDER_NAME / fileName += SAVE_EXTENSION);
}
