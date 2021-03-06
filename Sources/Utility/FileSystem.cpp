#include "FileSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


string FileSystem::AssetFolder = "../Assets";


void FileSystem::SetAssetFolder(const string& newAssetFolder)
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

string FileSystem::GetAssetPath(const std::string &file)
{
    return AssetFolder + "/" + file;
}

CheckedResult<string> FileSystem::ReadSomeFile(const string &path)
{
    ifstream file(path);
    if (!file)
    {
        cout<<"File "<< path <<" opening error\n";
        return CheckedResult<std::string>();
    }
    stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

bool FileSystem::WriteSomeFile(const std::string &path, const std::string& text)
{
    ofstream file(path);
    if (!file)
    {
        cout<<"File "<< path <<" opening error\n";
        return false;
    }
    file << text;
    file.close();
    return true;
}

CheckedResult<string> FileSystem::ReadAssetFile(const string &path)
{
    return ReadSomeFile(AssetFolder + "/" + path);
}

bool FileSystem::HasFile(const std::string &path)
{
    ifstream file(path);
    return file.good();
}

