#pragma once

#include <string>
#include <Ranok/LanguageCore/Utils.h>


class FileSystem
{
public:
    FileSystem() = delete;

    /// Set default Asset folder path
    static void SetAssetFolder(const std::string& newAssetFolder);

    /// Path to folder without / at end
    static std::string GetAssetFolder();

    /// Read file with some relative or absolute path
    static CheckedResult<std::string> ReadSomeFile(const std::string& path);

    /// Write to file with some relative or absolute path
    static bool WriteSomeFile(const std::string &path, const std::string& text);

    /// Read file relative to AssetFolder
    static CheckedResult<std::string> ReadAssetFile(const std::string& path);

    static bool HasFile(const std::string& path);


private:
    static std::string AssetFolder;
};

