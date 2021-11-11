#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>
#include "errorHandler.h"
#define MAX_NUM_LINES 1<<16


//  Text_Model_t - structure with information text
struct Text_Model_t
{
    char *buff;//text buffer
    long *strings;//array with strings start coords
    long buff_size;//size of buffer
    long strings_count;//quantity of strings
    long max_length;//max length of string on text
};

My_error_t CreateTextModel(struct Text_Model_t *p_model, const char *filename);

void DeleteTextModel(struct Text_Model_t *p_model);
#endif // MODEL_H
