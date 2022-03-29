#pragma once

#include <string>


class FileDialog
{
public:
    FileDialog() = delete;
    enum class FileMode
    { Open, Save };
    static std::string GetFilepath(FileMode mode, const std::string filter = "");


};

