// -------------------------------------------------------------- 
// dxf_assert.cpp
// Assert
//
// An DirectX framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_assert.h" 

#include "dxf_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <dbghelp.h>

DXF_NAMESPACE_BEGIN

AssertStyleEnum g_assertStyle = DXF_ASSERT_STYLE_DEFAULT;

void DXF_APIENTRY assertFunction(const char* condition, const char* file, int line, const AssertStyleEnum style)
{
    static char buffer[1024];

    switch (style) 
    {
        case DXF_ASSERT_STYLE_LOGANDEXIT:
            DXF_LOGINFO("Assert: %s!", condition);
            abort();
            break;
        case DXF_ASSERT_STYLE_DEBUG_BREAK:
            sprintf_s(buffer, 1024, "Assert: %s\n", condition);
            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
            __debugbreak();
            break;
        case DXF_ASSERT_STYLE_DEBUG_INFO:                                                                              
            sprintf_s(buffer, 1024, "Assert: %s\nPosition: %s(%d)\n", condition, file, line);
            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
            break;
        case DXF_ASSERT_STYLE_POPUP_BOX:
            sprintf_s(buffer, 1024, "Assert: %s\nPosition: %s(%d)\n", condition, file, line);
            MessageBoxA(NULL, buffer, "Error", MB_YESNO | MB_ICONERROR | MB_TASKMODAL);
            break;
        default:
            // ??
            break;
    }
}

void DXF_APIENTRY assertSetStyle(AssertStyleEnum style)
{
    g_assertStyle = style;
}

DXF_NAMESPACE_END
