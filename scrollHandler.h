#pragma once
#include <windows.h>
#include "viewer.h"



void ScrollBarProcess(HWND hwnd, WPARAM wParam, struct Viewer_t *p_viewer, bool vertical_scroll);

void KeyMessageProcess(HWND hwnd, WPARAM wParam, struct Viewer_t *p_viewer);
