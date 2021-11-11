#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <windows.h>
#include <stdlib.h>

#include "model.h"
#include "viewer.h"
#include "menuHandler.h"
#include "scrollHandler.h"
#include "paintHandler.h"
#include "openFileDlg.h"
#include "fileHandler.h"
#include "Menu.h"


LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    char *szClassName = "CodeBlocksWindowsApp";

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_OWNDC;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = "Menu";
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;

    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    if (!RegisterClassEx(&wincl))
    {
        return 0;
    }

    hwnd = CreateWindowEx (
               0,
               szClassName,
               "MyReader",
               WS_OVERLAPPEDWINDOW,
               CW_USEDEFAULT,
               CW_USEDEFAULT,
               600,
               400,
               HWND_DESKTOP,
               NULL,
               hThisInstance,
               lpszArgument);

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static struct Viewer_t viewer;
    static int client_area_width;
    static int client_area_height;
    switch (message)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);

        TEXTMETRIC tm;
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);

        ReleaseDC(hwnd, hdc);

        RECT rect;
        GetClientRect(hwnd, &rect);
        client_area_width = rect.right;
        client_area_height = rect.bottom;

        FileOpeningDialogInit();
        ViewerInit(&viewer, tm.tmAveCharWidth, tm.tmHeight + tm.tmExternalLeading, 0, 0, rect.right, rect.bottom);

        CREATESTRUCT *ptr = (CREATESTRUCT *)lParam;
        if (ptr && ptr->lpCreateParams)
        {
            FillViewerModel(hwnd, ptr->lpCreateParams, &viewer);
            HMENU hmenu = GetMenu(hwnd);
            if (viewer.p_text_model)
            {
                EnableMenuItem(hmenu, IDM_MENU_FILE_CLOSE,  MF_ENABLED);
                EnableMenuItem(hmenu, IDM_MENU_TEXT_WRAP, MF_ENABLED);
                EnableMenuItem(hmenu, IDM_MENU_FILE_OPEN,   MF_GRAYED);
            }
        }
        break;
    }

    case WM_COMMAND:
    {
        MenuMessageProcess(hwnd, LOWORD(wParam), &viewer);
        break;
    }

    case WM_HSCROLL:
    {
        ScrollBarProcess(hwnd, wParam, &viewer, false);
        break;
    }

    case WM_VSCROLL:
    {
        ScrollBarProcess(hwnd, wParam, &viewer, true);
        break;
    }

    case WM_KEYDOWN:
    {
        KeyMessageProcess(hwnd, wParam, &viewer);
    }
    break;

    case WM_SIZE:
    {
        long max_num_lines = MAX_NUM_LINES;
        client_area_height = HIWORD(lParam);
        client_area_width = LOWORD(lParam);
        ChangeViewerSize(&viewer, client_area_width, client_area_height);

        if (viewer.p_text_model)
        {
            viewer.compress_coef = viewer.p_text_model->strings_count / max_num_lines + 1;

            if(viewer.p_text_model->strings_count % max_num_lines == 0)
            {
                viewer.compress_coef++;
            }

            if(viewer.isWrapped)
            {

                //printf(" %li / %li  = %li\n",viewer.p_text_model->strings_count, max_num_lines, viewer.p_text_model->strings_count / max_num_lines);
                //printf("range = %li\n", viewer.p_text_model->strings_count/viewer.compress_coef);

                //SetScrollRange(hwnd, SB_VERT, 0, viewer.p_text_model->strings_count, TRUE);
                SetScrollRange(hwnd, SB_VERT, 0, viewer.p_text_model->strings_count/viewer.compress_coef, TRUE);
                SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
                SetScrollPos(hwnd, SB_VERT, viewer.current_str, TRUE);
            }
            else
            {
                //SetScrollRange(hwnd, SB_VERT, 0, viewer.p_text_model->strings_count - viewer.vis_str_quantity, TRUE);
                SetScrollRange(hwnd, SB_VERT, 0, (viewer.p_text_model->strings_count- viewer.vis_str_quantity)/viewer.compress_coef, TRUE);
                SetScrollRange(hwnd, SB_HORZ, 0, viewer.p_text_model->max_length - viewer.vis_str_length, TRUE);
                SetScrollPos(hwnd, SB_VERT, viewer.current_str, TRUE);
                SetScrollPos(hwnd, SB_HORZ, viewer.current_char, TRUE);
            }

        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RenderWindow(ps.rcPaint, hdc, &viewer);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
    {
        if (viewer.p_text_model)
        {
            DeleteTextModel(viewer.p_text_model);
        }
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
