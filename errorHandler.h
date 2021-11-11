#pragma once
#include <stdio.h>

#define MyPrintError(error) PrintError(error, __FILE__, __LINE__)

typedef enum {
    SUCCESS,
    INCORRECT_ARGS,
    MEMORY_ERR,
    FILE_OPENING_ERR,
    FILE_READING_ERR
} My_error_t;

FILE *SetLogsFile(FILE *output);

void PrintError(My_error_t error, const char *filename, int line);

FILE *GetLogFile();
