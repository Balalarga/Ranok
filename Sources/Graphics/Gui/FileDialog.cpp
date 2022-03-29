#include "FileDialog.h"

#include <windows.h>


std::string FileDialog::GetFilepath(FileMode mode, const std::string filter)
{
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    if (filter.empty())
        ofn.lpstrFilter = NULL;
    else
        ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName);
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    std::string fileNameStr;

    if ( (mode == FileMode::Open && GetOpenFileName(&ofn)) ||
         (mode == FileMode::Save && GetSaveFileName(&ofn)) )
        fileNameStr = fileName;

    return fileNameStr;
}
