#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "../OpenFIle/OpenFile.h"

//  Internal functions

//  CalculateStrsCount
//
//  Function:
//  count quantity of strings in text buffer
//
//  IN:
//  text - text buffer
//  size - size of text buffer
//
//  OUT:
//  returning value - quantity of strings

static long CalculateStrsCount(const char *text, const long size)
{
    if (!text)
    {
        return 0;
    }

    if (!size)
    {
        return 1;
    }

    long q = 0;
    for (long i = 0; i < size; i++)
    {
        if (text[i] == '\n')
        {
            q++;
        }
    }

    if (text[size - 1] != '\n')
    {
        q++;
    }

    return q;
}

//  StrParse
//
//  Function:
//  count offsets of strings beginnings, and lengths of strings in text buffer
//
//  IN:
//  text - text buffer
//  size - size of text buffer
//  str_quantity - quantity of strings in text buffer
//
//  OUT:
//  strs - array of structures, where string information will be stored
//  max_length - pointer to variable, where maximum length of strings will be stored

static void StrParse(long *strs, long *max_length, const int str_quantity, const char *text, const int size)
{
    if (!strs || !max_length || !text)
    {
        return;
    }
    long current_str = 0;
    long offset = 0;
    long max_len = 0;
    strs[current_str] = offset;
    for (; offset < size; offset++)
    {
        if (text[offset] == '\r')
        {
            current_str++;
            if (current_str < str_quantity)
            {
                strs[current_str] = offset + 2;
            }
            long length = offset;
            if (current_str >= 1)
            {
                length -= strs[current_str - 1];
            }
            max_len = (max_len > length) ? max_len : length;
            offset++;
        }
    }
    if (text[offset - 1] != '\n')
    {
        long length = offset;
        if (current_str >= 1)
        {
            length -= strs[current_str];
        }
        max_len = (max_len > length) ? max_len : length;
    }
    *max_length = max_len;
}

//  BufferParse
//
//  Function:
//  parse text stored in model
//  calculate text metrics like quantity of strings, stings beginnings, lengths, max string length
//
//  IN:
//  p_model - pointer to structure with text metrics
//
//  OUT:
//  returning value - error code from error handler


static My_error_t BufferParse(struct Text_Model_t *p_model)
{
    if (!p_model)
    {
        return INCORRECT_ARGS;
    }
    const char *text = p_model->buff;
    const long size = p_model->buff_size;
    const long q = CalculateStrsCount(text, size);
    long *strs = NULL;

    if ((q == 0) || !(strs = calloc(q, sizeof(long))))
    {
        return MEMORY_ERR;
    }

    long max_length = 0;
    StrParse(strs, &max_length, q, text, size);
    p_model->strings = strs;
    p_model->strings_count = q;
    p_model->max_length = max_length;
    return SUCCESS;
}


//  Functions

//  CreateTextModel
//
//  Function:
//  initialize model structure with text from file
//  Uses function from .dll called "ReadFromFile"
//
//  IN:
//  p_model - pointer to structure with text metrics
//  filename - name of file
//
//  OUT:
//  returning value - error code from error handler

My_error_t CreateTextModel(struct Text_Model_t *p_model, const char *filename)
{
    if (!p_model || !filename)
    {
        return INCORRECT_ARGS;
    }
    My_error_t e = ReadFromFileDLL(p_model, filename);

    if(e!=SUCCESS)
    {
        MyPrintError(e);
        return e;
    }

    if ((e = BufferParse(p_model)) != SUCCESS)
    {
        free(p_model->buff);
        *p_model = (struct Text_Model_t)
        {
            NULL, NULL, 0, 0
        };
        MyPrintError(e);
        return e;
    }

    return SUCCESS;
}

//  DeleteTextModel
//
//  Function:
//  delete model: set all metrics to zero and return resources to system
//
//  IN:
//  p_model - pointer to structure with text metrics

void DeleteTextModel(struct Text_Model_t *p_model)
{
    if (!p_model)
    {
        return;
    }

    free(p_model->buff);
    free(p_model->strings);

    *p_model = (struct Text_Model_t)
    {
        NULL, NULL, 0, 0
    };
}
