#include "menuHandler.h"
#include "Menu.h"
#include "openFileDlg.h"
#include "fileHandler.h"

#define MY_MAX_FILE_PATH 300

//  Functions
//  process messages and makes actions
//  IN: hwnd, message id, pointer on viewer

void MenuMessageProcess(HWND hwnd, UINT id, struct Viewer_t *p_viewer)
{
    if (!hwnd || !p_viewer)
    {
        return;
    }

    HMENU hmenu = GetMenu(hwnd);
    switch (id)
    {
    case IDM_MENU_QUIT:
    {
        SendMessage(hwnd, WM_DESTROY, 0, 0);
    }

    break;
    case IDM_MENU_FILE_OPEN:
    {

        char file_path[MY_MAX_FILE_PATH] = {0};
        if (FileOpeningDialogCall(hwnd, file_path, MY_MAX_FILE_PATH))
        {
            FillViewerModel(hwnd, file_path, p_viewer);
            if (p_viewer->p_text_model)
            {
                EnableMenuItem(hmenu, IDM_MENU_FILE_CLOSE,  MF_ENABLED);
                EnableMenuItem(hmenu, IDM_MENU_TEXT_WRAP, MF_ENABLED);
                EnableMenuItem(hmenu, IDM_MENU_FILE_OPEN,   MF_GRAYED);
            }
        }
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    }
    break;

    case IDM_MENU_FILE_CLOSE:
    {
        EnableMenuItem(hmenu, IDM_MENU_FILE_CLOSE, MF_GRAYED);
        EnableMenuItem(hmenu, IDM_MENU_TEXT_WRAP, MF_GRAYED);
        EnableMenuItem(hmenu, IDM_MENU_TEXT_UNWRAP, MF_GRAYED);
        EnableMenuItem(hmenu, IDM_MENU_FILE_OPEN, MF_ENABLED);
        DeleteTextModel(p_viewer->p_text_model);
        ResetViewer(p_viewer);
        ShowScrollBar(hwnd, SB_BOTH, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    }
    break;

    case IDM_MENU_TEXT_UNWRAP:
    {
        EnableMenuItem(hmenu, IDM_MENU_TEXT_WRAP, MF_ENABLED);
        EnableMenuItem(hmenu, IDM_MENU_TEXT_UNWRAP, MF_GRAYED);
        SetUnwrapped( p_viewer);


        SetScrollRange(hwnd, SB_VERT, 0, (p_viewer->p_text_model->strings_count- p_viewer->vis_str_quantity)/p_viewer->compress_coef, TRUE);
        SetScrollRange(hwnd, SB_HORZ, 0, p_viewer->p_text_model->max_length - p_viewer->vis_str_length, TRUE);
        SetScrollPos(hwnd, SB_VERT, p_viewer->current_str, TRUE);
        SetScrollPos(hwnd, SB_HORZ, p_viewer->current_char, TRUE);

        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

    case IDM_MENU_TEXT_WRAP:
    {
        EnableMenuItem(hmenu, IDM_MENU_TEXT_WRAP, MF_GRAYED);
        EnableMenuItem(hmenu, IDM_MENU_TEXT_UNWRAP, MF_ENABLED);
        SetWrapped( p_viewer);
        SetScrollRange(hwnd, SB_VERT, 0, p_viewer->p_text_model->strings_count/p_viewer->compress_coef, TRUE);
        SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
        SetScrollPos(hwnd, SB_VERT, p_viewer->current_str, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

    }
}
