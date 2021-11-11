#pragma once
#include <windows.h>
#include <stdbool.h>

void FileOpeningDialogInit();

bool FileOpeningDialogCall(HWND hwnd, char *buff, int size);
