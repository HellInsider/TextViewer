#include "openFileDlg.h"
#include <commdlg.h>

//  ofn - variable for creating open dialog and retriving file name
static OPENFILENAME ofn;

//  Functions

//  FileOpeningDialogInit
//
//  Function:
//  initialize fields of open file structure

void FileOpeningDialogInit()
{
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = "Text files\0*.txt\0\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = NULL;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "txt";
    ofn.lCustData = 0;
    ofn.lpTemplateName = NULL;
}

//  FileOpeningDialogCall
//
//  Function:
//  initialize fields of open file structure and call dialog
//
//  IN:
//  hwnd - window handle
//  buff - text buffer, where path of file will be stored
//  size - size of buffer
//
//  OUT:
//  returning value - result of function GetOpenFileName

bool FileOpeningDialogCall(HWND hwnd, char *buff, int size)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = buff;
    ofn.nMaxFile = size;
    ofn.Flags = OFN_HIDEREADONLY;
    return GetOpenFileName(&ofn);
}
