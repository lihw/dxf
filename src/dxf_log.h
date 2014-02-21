// -------------------------------------------------------------- 
// dxf_log.h
// Logging
//
// An DirectX framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_LOG_H
#define DXF_LOG_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

//
// The levels of the log
enum LogLevelEnum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,

    LOG_LEVEL_LOWEST = LOG_DEBUG,
    LOG_LEVEL_HIGHEST = LOG_ERROR,

    LOG_VERBOSITY_DEFAULT = LOG_LEVEL_LOWEST
};

enum LogOutputEnum
{
    LOG_OUTPUT_CONSOLE,
    LOG_OUTPUT_DEBUG,
    LOG_OUTPUT_FILE,

    LOG_OUTPUT_DEFUALT = LOG_OUTPUT_CONSOLE
};

void logSetOutput(LogOutputEnum output);
LogOutputEnum logGetOutput();
// Only allow log with level higher than given verbosity.
void logSetVerbosity(LogLevelEnum verbosity);
LogLevelEnum logGetVerbosity();

// Output the log in the formatted way
// \param level the level of this log message
// \param file at which file this log message is generated.
// \param line at which line of the file this log message is created.
// \param format the format specifier as the one in printf.
void log(LogLevelEnum level, const char* file, int line, const char* format, ...);

#define DXF_LOGERROR(fmt, ...) \
    do { dxf::log(dxf::LOG_ERROR, __FILE__, __LINE__, fmt, __VA_ARGS__); } while (0)
#define DXF_LOGWARNING(fmt, ...) \
    do { dxf::log(dxf::LOG_WARNING, __FILE__, __LINE__, fmt, __VA_ARGS__); } while (0)
#define DXF_LOGINFO(fmt, ...) \
    do { dxf::log(dxf::LOG_INFO, NULL, 0, fmt, __VA_ARGS__); } while (0)

#if defined DEBUG
# define DXF_LOGDEBUG(fmt, ...) \
     do { dxf::log(dxf::LOG_DEBUG, __FILE__, __LINE__, fmt, __VA_ARGS__); } while (0)
# define DXF_LOGDEBUGIF(cond, fmt, ...) \
     { if ((cond)) { dxf::log(dxf::LOG_DEBUG, __FILE__, __LINE__, fmt, __VA_ARGS__); } }
#else
#   define DXF_LOGDEBUG(fmt, ...) 
#   define DXF_LOGDEBUGIF(cond, fmt, ...) 
#endif

void logPlain(const char* format, ...);

DXF_NAMESPACE_END

#endif // !DXF_LOG_H
