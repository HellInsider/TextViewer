#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

char*  DLL_EXPORT ReadFromFile(long* res, long* size, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
