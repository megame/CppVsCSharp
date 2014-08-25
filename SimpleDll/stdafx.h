// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

// Mysteriously, the default comments beside the following constants in Visual 
// Studio say both that "the minimum required platform is Windows Vista" (WINVER=0x0600)
// AND "the minimum required platform is Windows 98". (0x0500 is Windows 2000)

#ifndef WINVER                // Specifies that the minimum required platform is Windows 2000.
#define WINVER 0x0500
#endif
#ifndef _WIN32_WINNT          // Specifies that the minimum required platform is Windows 2000.
#define _WIN32_WINNT 0x0500
#endif
#ifndef _WIN32_WINDOWS        // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410
#endif


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
