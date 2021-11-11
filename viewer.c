#include "viewer.h"

//  Functions

//  ViewerInit
//
//  Function:
//  initialize fields of viewer structure
//
//  IN:
//  char_width - character width
//  char_hight - string height
//  x - x position in client area
//  y - y position in client area
//  width - width in client area
//  height - in client area
//
//  OUT:
//  p_viewer - pointer to viewer structure

void ViewerInit(struct Viewer_t *p_viewer, int char_width, int char_hight, int x, int y, int width, int height)
{
    if (!p_viewer)
    {
        return;
    }
    p_viewer->p_text_model = NULL;
    p_viewer->char_height = char_hight;
    p_viewer->char_width = char_width;
    p_viewer->current_str = 0;
    p_viewer->current_char = 0;
    p_viewer->isWrapped = false;
    p_viewer->area_x_pos = x;
    p_viewer->area_y_pos = y;
    p_viewer->area_width = width;
    p_viewer->area_height = height;
    p_viewer->vis_str_length = width / char_width;
    p_viewer->vis_str_quantity = height / char_hight;
    p_viewer->compress_coef = 1;
}

//  SetTextModel
//
//  Function:
//  set model to viewer
//
//  IN:
//  p_viewer - pointer to viewer structure
//  p_model - pointer to model structure

void SetTextModel(struct Viewer_t *p_viewer, struct Text_Model_t *p_model)
{
    if (!p_viewer && !p_model)
    {
        return;
    }
    p_viewer->p_text_model = p_model;
}

//  ChangeViewerSize
//
//  Function:
//  set new viewer size and updates related information
//
//  IN:
//  p_viewer - pointer to viewer structure
//  width - new viewer width
//  height - new viewer height

void ChangeViewerSize(struct Viewer_t *p_viewer, int width, int height)
{
    if (!p_viewer)
    {
        return;
    }

    p_viewer->area_width = width;
    p_viewer->area_height = height;
    p_viewer->vis_str_length = width / p_viewer->char_width;
    p_viewer->vis_str_quantity = height / p_viewer->char_height;

}

//  ResetViewer
//
//  Function:
//  set to default fields connected with model
//
//  IN:
//  p_viewer - pointer to viewer structure

void ResetViewer(struct Viewer_t *p_viewer)
{
    if (!p_viewer)
    {
        return;
    }
    p_viewer->current_char = 0;
    p_viewer->current_str = 0;
    p_viewer->isWrapped = false;
    p_viewer->p_text_model = NULL;
    p_viewer->compress_coef = 1;
}

//  MySetWarped
//
//  Function:
//  set state to Wrapped
//
//  IN:
//  p_viewer - pointer to viewer structure

void SetWrapped( struct Viewer_t *p_viewer)
{
    if (!p_viewer)
    {
        return;
    }

    if (p_viewer->p_text_model)
    {
        p_viewer->vis_str_quantity = p_viewer->area_height / p_viewer->char_height + 1;
        p_viewer->isWrapped = true;
    }
}

//  MySetWarped
//
//  Function:
//  set state to unwrapped
//
//  IN:
//  p_viewer - pointer to viewer structure

void SetUnwrapped(struct Viewer_t *p_viewer)
{
    if (!p_viewer)
    {
        return;
    }

    if (p_viewer->p_text_model)
    {
        p_viewer->current_char = 0;
        p_viewer->isWrapped = false;
        p_viewer->vis_str_quantity = p_viewer->area_height / p_viewer->char_height + 1;
    }
}
