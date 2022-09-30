#pragma once
#include <string>
namespace Ranok
{
std::string OpenFileDialog();


#if _WINDOWS
#include <windows.h>
std::string OpenFileDialog(const std::string& filters)
{
	std::string result;
	OPENFILENAME ofn = {sizeof(ofn)};
	char szFile[300];
	ofn.lStructSize = sizeof( ofn );
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	const char * filter = filters.c_str();
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir=NULL;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
	
	if(GetOpenFileNameA(&ofn))
		result = ofn.lpstrFile;
	return result;
}

#endif
}
