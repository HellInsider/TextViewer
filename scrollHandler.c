#include "scrollHandler.h"

//  Internal functions

//  InvalidRectCreate
//
//  Function:
//  calculates invalid rectangle based on viewer area and vector (magnitude and direction) of scrolling
//
//  IN:
//  viewer_r - pointer to viewer area rectangle
//  shift - signed scroll magnitude
//  vertical - boolean flag defining the axis of scroll
//
//  OUT:
//  invalid_r - pointer to invalid rectangle, where calculated values will be stored

static void InvalidRectCreate(const RECT *viewer_r, RECT *invalid_r, int shift, bool vertical)
{
    if (!viewer_r || !invalid_r)
    {
        return;
    }

    *invalid_r = (RECT)
    {
        viewer_r->left,
                 viewer_r->top,
                 viewer_r->right,
                 viewer_r->bottom
    };

    if (shift > 0)
    {
        if (vertical)
        {
            invalid_r->bottom = shift;
        }
        else
        {
            invalid_r->right = shift;
        }
    }
    else
    {
        if (vertical)
        {
            invalid_r->top = invalid_r->bottom + shift;
        }
        else
        {
            invalid_r->right = invalid_r->left + shift;
        }
    }
}

//  ScreenScrollProcess
//
//  Function:
//  perform scroll of specific part of window defined by viewer through a specific value
//
//  IN:
//  hwnd - window handle
//  p_viewer - pointer to viewer structure
//  vertical - boolean flag defining the axis of scroll
//  difference - signed quantity of strings or characters, that we need to scroll (sign defines direction of scroll)

static void ScreenScrollProcess(HWND hwnd, struct Viewer_t *p_viewer, long difference, bool vertical)
{
    if (!hwnd || !p_viewer)
    {
        return;
    }
    long old_pos;
    long *p_old_pos;
    long max_pos;
    int scroll_type;
    int logic_units_per_pos;
    int critical_shift;

    if(p_viewer->isWrapped)
    {

        old_pos = p_viewer->current_str;
        p_old_pos = &p_viewer->current_str;
        max_pos = p_viewer->p_text_model->strings_count - 1;
        scroll_type = SB_VERT;
        logic_units_per_pos = p_viewer->char_height;
        critical_shift = p_viewer->area_height;

    }
    else
    {
        if (vertical)
        {
            old_pos = p_viewer->current_str;
            p_old_pos = &p_viewer->current_str;
            max_pos = p_viewer->p_text_model->strings_count - p_viewer->vis_str_quantity;
            scroll_type = SB_VERT;
            logic_units_per_pos = p_viewer->char_height;
            critical_shift = p_viewer->area_height;
        }
        else
        {
            old_pos = p_viewer->current_char;
            p_old_pos = &p_viewer->current_char;
            max_pos = p_viewer->p_text_model->max_length - p_viewer->vis_str_length;
            scroll_type = SB_HORZ;
            logic_units_per_pos = p_viewer->char_width;
            critical_shift = p_viewer->area_width;
        }

    }




    long new_pos = max(min(old_pos + difference, max_pos), 0);
    //printf(" new pos: %li + %li == %li\n",old_pos, difference, new_pos);
    SetScrollPos(hwnd, scroll_type, new_pos/p_viewer->compress_coef, TRUE);

    int lenght = abs(p_viewer->p_text_model->strings[old_pos] - p_viewer->p_text_model->strings[new_pos])/p_viewer->vis_str_length + 1;
    int shift = (old_pos - new_pos) * logic_units_per_pos;

    if(p_viewer->isWrapped)
    {
        if(lenght >= p_viewer->vis_str_quantity)
        {
            *p_old_pos = new_pos;
        }
        else
        {
            //printf(" cur_str %li, cur_char %li, - > ",p_viewer->current_str, p_viewer->current_char);
            for(int t = 0; t <difference;t++)
            {
                if(p_viewer->current_str < p_viewer->p_text_model->strings_count - 1)
                {

                    if(p_viewer->vis_str_length >= p_viewer->p_text_model->strings[p_viewer->current_str+1]
                        - p_viewer->p_text_model->strings[p_viewer->current_str] - p_viewer->current_char )
                    {
                         p_viewer->current_char = 0;
                         p_viewer->current_str++;
                    }
                    else
                    {
                        p_viewer->current_char+= p_viewer->vis_str_length+1;
                    }
                }
            }

            for(int t = 0; t < -difference; t++)
            {
                if(p_viewer->current_str > 0 || p_viewer->current_char >= 0)
                {

                    if(p_viewer->current_char <= 1 && p_viewer->current_str > 0)
                    {

                        p_viewer->current_char = (p_viewer->p_text_model->strings[p_viewer->current_str] -
                                                  p_viewer->p_text_model->strings[p_viewer->current_str-1]) -
                                                  (p_viewer->p_text_model->strings[p_viewer->current_str] -
                                                  p_viewer->p_text_model->strings[p_viewer->current_str-1]) % (p_viewer->vis_str_length + 1);
                        p_viewer->current_str--;

                    }
                    else
                    {
                        p_viewer->current_char -= p_viewer->vis_str_length + 1;
                        if(p_viewer->current_char<0)
                        {
                            p_viewer->current_char = 0;
                        }
                    }
                }
            }
        }
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        return;
    }
    else
    {
        *p_old_pos = new_pos;
    }


    if (abs(shift) >= critical_shift)
    {
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        return;
    }


    RECT viewer_rect = (RECT)
    {
        p_viewer->area_x_pos,
                 p_viewer->area_y_pos,
                 p_viewer->area_x_pos + p_viewer->area_width,
                 p_viewer->area_y_pos + p_viewer->area_height
    };

    RECT invalid_rect;
    InvalidRectCreate(&viewer_rect, &invalid_rect, shift, vertical);
    ScrollWindow(hwnd, vertical ? 0 : shift, vertical ? shift : 0, &viewer_rect, NULL);
    InvalidateRect(hwnd, &invalid_rect, TRUE);
    UpdateWindow(hwnd);
}

//  Functions

//  ScrollBarProcess
//
//  Function:
//  calculate value of scroll defined by scrollbar message
//  perform scroll of specific part of window defined by viewer
//
//  IN:
//  hwnd - window handle
//  p_viewer - pointer to viewer structure
//  vertical - boolean flag defining the axis of scroll
//  wParam - information about scroll message

void ScrollBarProcess(HWND hwnd, WPARAM wParam, struct Viewer_t *p_viewer, bool vertical_scroll)
{
    if (!hwnd || !p_viewer || !p_viewer->p_text_model)
    {
        return;
    }

    long difference;

    switch (LOWORD(wParam))
    {
        case SB_TOP:
        {
            difference = vertical_scroll ? -p_viewer->current_str : -p_viewer->current_char;
        }
        break;
        case SB_BOTTOM:
        {
            difference = vertical_scroll ?
                   p_viewer->p_text_model->strings_count - p_viewer->current_str - p_viewer->vis_str_quantity :
                   p_viewer->p_text_model->max_length - p_viewer->current_char - p_viewer->vis_str_length;
        }
        break;
        case SB_LINEUP:
        {
            difference = -1;
        }
        break;
        case SB_LINEDOWN:
        {
            difference = 1;
        }
        break;
        case SB_PAGEDOWN:
        {
            difference = max(1, vertical_scroll ? p_viewer->vis_str_quantity : p_viewer->vis_str_length - 1);
        }
        break;
        case SB_PAGEUP:
        {
            difference = min(-1, (vertical_scroll ? -p_viewer->vis_str_quantity : -p_viewer->vis_str_length) + 1);
        }
        break;
        case SB_THUMBPOSITION:
        {
            difference = HIWORD(wParam)*p_viewer->compress_coef - (vertical_scroll ? p_viewer->current_str : p_viewer->current_char);
        }
        break;
        default:
            difference = 0;
    }

    if( !(difference>=-1 && difference <=1) && LOWORD(wParam) !=  SB_THUMBPOSITION )
    {
            difference *= p_viewer->compress_coef;
    }

    if (difference)
    {
        ScreenScrollProcess(hwnd, p_viewer, difference, vertical_scroll);
    }
}

//  KeyMessageProcess
//
//  Function:
//  calculate value and direction of difference for scrollbar
//
//  IN:
//  hwnd - window handle
//  p_viewer - pointer to viewer structure
//  wParam - information about keyboard message

void KeyMessageProcess(HWND hwnd, WPARAM wParam, struct Viewer_t *p_viewer)
{
    if (!hwnd || !p_viewer || !p_viewer->p_text_model)
    {
        return;
    }
    long difference;
    bool vertical;
    switch (wParam)
    {
    case VK_UP:
    {
        difference = -1;
        vertical = true;
    }
    break;
    case VK_DOWN:
    {
        difference = 1;
        vertical = true;
    }
    break;
    case VK_LEFT:
    {
        difference = -1;
        vertical = false;
    }
    break;
    case VK_RIGHT:
    {
        difference = 1;
        vertical = false;
    }
    break;
    case VK_NEXT:
    {
        difference = max(1, p_viewer->vis_str_quantity - 1);
        vertical = true;
    }
    break;
    case VK_PRIOR:
    {
        difference = min(-1, - p_viewer->vis_str_quantity + 1);
        vertical = true;
    }
    break;
    case VK_END:
    {
        difference = max(1, p_viewer->vis_str_length - 1);
        vertical = false;
    }
    break;
    case VK_HOME:
    {
        difference = min(-1, - p_viewer->vis_str_length + 1);
        vertical = false;
    }
    break;
    default:
        difference = 0;
    }
    if (difference)
    {
        ScreenScrollProcess(hwnd, p_viewer, difference, vertical);
    }
}
