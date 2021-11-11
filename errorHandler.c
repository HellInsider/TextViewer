#include "errorHandler.h"

//  log file
static FILE *log_file = NULL;

//  Functions

//  SetLogsFile
//
//  Function:
//  sets log file
//
//  IN:
//  output - log file stream
//
//  OUT:
//  returning value - previous log file stream

FILE *SetLogsFile(FILE *output)
{
    FILE * buff = log_file;
    log_file = output;
    return buff;
}

//  GetLogFile
//
//  Function:
//  returns log file stream
//
//  OUT:
//  returning value - log file stream

FILE *GetLogFile()
{
    return log_file;
}

//  PrintError
//
//  Function:
//  prints to log file information about error
//
//  IN:
//  error - error code
//  filename - file where error occurred
//  line - line in file where error occurred

void PrintError(My_error_t error, const char *filename, int line)
{
    static char* error_descriptions[] = {
        "No error\n",
        "Internal error: wrong arguments passed\n",
        "Internal error: cannot allocate memory\n",
        "File cannot be opened\n"
        "Cannot read from file\n"
    };
    if (log_file)
    {
        fprintf(log_file, "%s in file: %s, at %i line\n", error_descriptions[(int)error], filename, line);
    }
}
