#include "paintHandler.h"

//  Internal functions

//  InvalideRectProcess
//
//  Function:
//  calculate part of invalid rectangle, corresponding to specific viewer field
//
//  IN:
//  viewer_r - viewer field rectangle
//  invalid_r - invalid rectangle
//
//  OUT:
//  invalid_r - modified rectangle

static void InvalideRectProcess(RECT *invalid_r, const RECT *viewer_r)
{
    if (!invalid_r || !viewer_r)
    {
        return;
    }
    if (invalid_r->top < viewer_r->top)
    {
        invalid_r->top = 0;
    }
    else
    {
        invalid_r->top -= viewer_r->top;
    }
    if (invalid_r->left < viewer_r->left)
    {
        invalid_r->left = 0;
    }
    else
    {
        invalid_r->left -= viewer_r->left;
    }
    if (invalid_r->right > viewer_r->right)
    {
        invalid_r->right = viewer_r->right;
    }
    invalid_r->right -= viewer_r->left;
    if (invalid_r->bottom > viewer_r->bottom)
    {
        invalid_r->bottom = viewer_r->bottom;
    }
    invalid_r->bottom -= viewer_r->top;
}

//  CharacterRectProcess
//
//  Function:
//  calculate character rectangle, corresponding to rectangle in logical units
//
//  IN:
//  p_viewer - pointer to viewer structure
//  invalid_r - invalid rectangle
//
//  OUT:
//  char_rect - character rectangle

static void CharacterRectProcess(const RECT *invalid_rect, RECT *char_rect, struct Viewer_t *p_viewer)
{
    if (!invalid_rect || !char_rect || !p_viewer)
    {
        return;
    }
    char_rect->top = invalid_rect->top / p_viewer->char_height;
    char_rect->left = invalid_rect->left / p_viewer->char_width;
    char_rect->bottom = invalid_rect->bottom / p_viewer->char_height +
                        ((invalid_rect->bottom % p_viewer->char_height) ? 1 : 0);

    if(!p_viewer->isWrapped)
        char_rect->bottom = min(char_rect->bottom, p_viewer->p_text_model->strings_count - p_viewer->current_str);

    char_rect->right = invalid_rect->right / p_viewer->char_width +
                       ((invalid_rect->right % p_viewer->char_width) ? 1 : 0);
}

//  Functions

//  RenderWindow
//
//  Function:
//  draw invalidated content of viewer
//
//  IN:
//  p_viewer - pointer to viewer structure
//  invalid_r - invalid rectangle
//  hdc - device context handle

void RenderWindow(RECT invalid_rect, HDC hdc, struct Viewer_t *p_viewer)
{
    if (!hdc || !p_viewer || !p_viewer->p_text_model)
    {
        return;
    }

    RECT viewer_rect = (RECT)
    {
        p_viewer->area_x_pos,
                 p_viewer->area_y_pos,
                 p_viewer->area_x_pos + p_viewer->area_width,
                 p_viewer->area_y_pos + p_viewer->area_height
    };

    InvalideRectProcess(&invalid_rect, &viewer_rect);
    RECT char_rect;
    CharacterRectProcess(&invalid_rect, &char_rect, p_viewer);

    long buffer_str=0;

    long wrapped_str_offset = 0;
    long cur_char_in_str = p_viewer->current_char;
    //printf("Cur_str = %li, char = %li\n", p_viewer->current_str,p_viewer->current_char);
    //printf("Cur_coef = %li\n", p_viewer->compress_coef);
    //printf("top %li, bot %li\n", char_rect.top,char_rect.bottom);


    for (long screen_str = char_rect.top; screen_str< char_rect.bottom; screen_str++)
    {
        long str_length;
        long length;

        if (!p_viewer->isWrapped)
        {
            if ((screen_str + p_viewer->current_str + 1) < p_viewer->p_text_model->strings_count)
            {
                str_length = p_viewer->p_text_model->strings[screen_str + p_viewer->current_str + 1];
            }
            else if( (screen_str + p_viewer->current_str + 1) == p_viewer->p_text_model->strings_count)
            {
                str_length = p_viewer->p_text_model->buff_size;
            }
            else
                return;

            str_length -= p_viewer->p_text_model->strings[screen_str + p_viewer->current_str];
            length = min(str_length - char_rect.left - p_viewer->current_char, char_rect.right - char_rect.left);


            if (length > 0)
            {
                TextOut(hdc,
                        p_viewer->area_x_pos + char_rect.left * p_viewer->char_width,
                        p_viewer->area_y_pos + screen_str*p_viewer->char_height,
                        p_viewer->p_text_model->buff +
                        p_viewer->p_text_model->strings[screen_str + p_viewer->current_str] +
                        char_rect.left + p_viewer->current_char,
                        length
                        );
            }

        }
        else
        {
            if ((buffer_str + p_viewer->current_str + 1) < p_viewer->p_text_model->strings_count)
            {
                str_length = p_viewer->p_text_model->strings[buffer_str + p_viewer->current_str + 1];
            }
            else if((buffer_str + p_viewer->current_str + 1) == p_viewer->p_text_model->strings_count)
            {
                str_length = p_viewer->p_text_model->buff_size;
            }
            else
                return;

            str_length -= ( p_viewer->p_text_model->strings[buffer_str + p_viewer->current_str] + cur_char_in_str);

            long strs_count = str_length/(char_rect.right - char_rect.left);




            length = char_rect.right - char_rect.left;

            for( wrapped_str_offset = 0; wrapped_str_offset < strs_count && screen_str < char_rect.bottom; wrapped_str_offset++)
            {
                TextOut(hdc,
                        p_viewer->area_x_pos + char_rect.left * p_viewer->char_width,
                        p_viewer->area_y_pos + (screen_str)*p_viewer->char_height,
                        p_viewer->p_text_model->buff +
                        p_viewer->p_text_model->strings[buffer_str + p_viewer->current_str] +
                        char_rect.left + cur_char_in_str,
                        length
                        );

                cur_char_in_str += length;
                screen_str++;
            }

            length = min(str_length - strs_count* length - char_rect.left, char_rect.right - char_rect.left);
    //
            if(length>=0)
            {
                TextOut(hdc,
                    p_viewer->area_x_pos + char_rect.left * p_viewer->char_width,
                    p_viewer->area_y_pos + (screen_str)*p_viewer->char_height,
                    p_viewer->p_text_model->buff +
                    p_viewer->p_text_model->strings[buffer_str + p_viewer->current_str] +
                    char_rect.left + cur_char_in_str,
                    length
                    );
            }
            buffer_str++;
            cur_char_in_str = 0;

        }

    }
}
