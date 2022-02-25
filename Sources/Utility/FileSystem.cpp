#include "FileSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


std::string FileSystem::AssetFolder = "../Assets";


void FileSystem::SetAssetFolder(const std::string& newAssetFolder)
{
    if (newAssetFolder.back() == '\\' ||
            newAssetFolder.back() == '/')
    {
        AssetFolder = newAssetFolder.substr(0, newAssetFolder.size()-1);
    }
    else
    {
        AssetFolder = newAssetFolder;
    }
}

string FileSystem::GetAssetFolder()
{
    return AssetFolder;
}

std::string FileSystem::ReadSomeFile(const std::string &path)
{
    ifstream file(path);
    if (!file)
    {
        cout<<"File "<< path <<" opening error\n";
        return "";
    }
    stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

std::string FileSystem::ReadAssetFile(const std::string &path)
{
    return ReadSomeFile(AssetFolder + "/" + path);
}
