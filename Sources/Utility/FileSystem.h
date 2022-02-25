#include <string>

#pragma once


class FileSystem
{
public:
    FileSystem() = delete;

    /// Set default Asset folder path
    static void SetAssetFolder(const std::string& newAssetFolder);

    /// Path to folder without / at end
    static std::string GetAssetFolder();

    /// Read file with some relative or absolute path
    static std::string ReadSomeFile(const std::string& path);

    /// Read file relative to AssetFolder
    static std::string ReadAssetFile(const std::string& path);


private:
    static std::string AssetFolder;
};

