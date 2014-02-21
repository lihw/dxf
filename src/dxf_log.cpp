// -------------------------------------------------------------- 
// glf_log.cpp
// Logging
//
// An OpenGL framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_log.h"

DXF_NAMESPACE_BEGIN

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <Windows.h>
#include <DbgHelp.h>


static LogOutputEnum g_output = LOG_OUTPUT_DEFUALT;
static LogLevelEnum g_verbosity = LOG_VERBOSITY_DEFAULT;
static FILE* g_logFile = NULL;

void logSetOutput(LogOutputEnum output)
{
    g_output = output;
}

LogOutputEnum logGetOutput()
{
    return g_output;
}

void logSetVerbosity(LogLevelEnum verbosity)
{
    g_verbosity = verbosity;
}

LogLevelEnum logGetVerbosity()
{
    return g_verbosity;
}

static void logOutput(const char* message, int length)
{
    switch (g_output)
    {
        case LOG_OUTPUT_CONSOLE:
            printf("%s\n", message);
            break;
        case LOG_OUTPUT_DEBUG:
            OutputDebugStringA(message);
            break;
        case LOG_OUTPUT_FILE:
            {
                if (g_logFile == NULL)
                {
                    fopen_s(&g_logFile, "log.txt", "wb");
                    if (g_logFile == NULL)
                    {
                        // Change the log output to console when
                        // log.txt can't be created or written.
                        g_output = LOG_OUTPUT_CONSOLE;
                        printf("%s\n", message);
                        break;
                    }
                }
                fprintf(g_logFile, "%s\n", message);
            }
            break;
        default:
            break;
    }
}

void log(LogLevelEnum level, const char* file, int line, const char* format, ...)
{
    // The global verbosity value controls which kinds of log can be 
    // printed.
    if (level < g_verbosity)
    {
        return ;
    }

    static const char* verbosityText[] = 
    {
        "(debug)",
        "(info)",
        "(warning)",
        "(error)",
        "(fatal)",
    };

    va_list arguments;

    va_start(arguments, format);

    if (strlen(format) > 16384)
    {
        char message[1024];
        int nchars = sprintf_s(message, 1024, 
                "(warning)%s:%d,log buffer for formatting data is too small",
                __FILE__, __LINE__);
        logOutput(message, nchars);
    }
    else
    {
        char msg[32767]; 
        int nchars;
        if (level == LOG_ERROR || level == LOG_WARNING)
        {
            char fmt[16384];
            sprintf_s(fmt, 16384, "%s%s:%d,%s.", 
                verbosityText[level], file, line, format);
            nchars = vsprintf_s(msg, 32767, fmt, arguments);
        }
        else 
        {
            nchars = vsprintf_s(msg, 32767, format, arguments);
        }

        logOutput(msg, nchars);
    }
}

void logPlain(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    char msg[4000]; 
    int nchars;
    nchars = vsprintf_s(msg, 4000, format, arguments);
    logOutput(msg, nchars);
}

DXF_NAMESPACE_END

