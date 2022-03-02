#pragma once

#include <string>


class FileDialog
{
public:
    FileDialog() = delete;

    static std::string GetFilepath(const std::string filter = "All Files(*.*)");


};

