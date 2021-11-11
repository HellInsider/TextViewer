#pragma once
#include <stdbool.h>
#include "model.h"
#include "errorHandler.h"

//  Viewer_t - structure with viewing information about text
struct Viewer_t
{
    struct Text_Model_t *p_text_model;//stored model
    long current_str;//current string index
    long current_char;//current char offset index
    int char_height;//height of char
    int char_width;//width of character
    int area_x_pos;//x position of viewer (logical units)
    int area_y_pos;//y position of viewer (logical units)
    int area_width;//width of viewer (logical units)
    int area_height;//height of viewer (logical units)
    int vis_str_quantity;//maximum visible strings
    int vis_str_length;//maximum visible characters
    bool isWrapped;//state (wrapped/unwrapped)
    int compress_coef;// value for scrolling big files
};

void ViewerInit(struct Viewer_t *p_viewer, int char_width, int char_hight, int x, int y, int width, int height);

void SetTextModel(struct Viewer_t *p_viewer, struct Text_Model_t *p_model);

void ChangeViewerSize(struct Viewer_t *p_viewer, int width, int height);

void ResetViewer(struct Viewer_t *p_viewer);

void SetWrapped(struct Viewer_t *p_viewer);

void SetUnwrapped(struct Viewer_t *p_viewer);
