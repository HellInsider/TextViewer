#include "fileHandler.h"
#include "model.h"
#include "errorHandler.h"

//  Function FillTextModel

//Fills viewer model with text from file

//IN: hwnd, file path and pointer on viewer

void FillViewerModel(HWND hwnd, char *filename, struct Viewer_t *p_viewer)
{
    if (!hwnd || !filename || !p_viewer)
    {
        return;
    }
    static struct Text_Model_t model;
    My_error_t e = CreateTextModel(&model, filename);

    if (e == FILE_OPENING_ERR)
    {
        MyPrintError(e);
        MessageBox(hwnd, "File open error occurred!", NULL, MB_OK);
    }
    else if (e == FILE_READING_ERR)
    {
        MyPrintError(e);
        MessageBox(hwnd, "File read error occurred!", NULL, MB_OK);
    }
    else if (e != SUCCESS)
    {
        MyPrintError(e);
        MessageBox(hwnd, "Internal error while opening file!", NULL, MB_OK);
    }
    else
    {
        long max_num_lines = MAX_NUM_LINES;

        SetTextModel(p_viewer, &model);
        p_viewer->compress_coef = p_viewer->p_text_model->strings_count / max_num_lines + 1;

        if(p_viewer->p_text_model->strings_count % max_num_lines == 0)
        {
            //printf("Here\n");
            p_viewer->compress_coef++;
        }

        if(p_viewer->isWrapped)
        {
            SetScrollRange(hwnd, SB_VERT, 0, p_viewer->p_text_model->strings_count/p_viewer->compress_coef, TRUE);
            SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
        }
        else
        {
            SetScrollRange(hwnd, SB_VERT, 0, (p_viewer->p_text_model->strings_count - p_viewer->vis_str_quantity)/p_viewer->compress_coef, TRUE);
            SetScrollRange(hwnd, SB_HORZ, 0, p_viewer->p_text_model->max_length - p_viewer->vis_str_length, TRUE);
        }


        SetScrollPos(hwnd, SB_VERT, p_viewer->current_str, TRUE);
        SetScrollPos(hwnd, SB_HORZ, p_viewer->current_char, TRUE);
    }
}
