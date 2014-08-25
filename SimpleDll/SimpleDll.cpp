// SimpleDll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "Oleauto.h"

#ifdef UNDER_CE
BOOL APIENTRY DllMain(HANDLE hModule,
#else
BOOL APIENTRY DllMain(HMODULE hModule,
#endif
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SIMPLEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SIMPLEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SIMPLEDLL_EXPORTS
#define SIMPLEDLL_API __declspec(dllexport)
#else
#define SIMPLEDLL_API __declspec(dllimport)
#endif

////////////////////////////////////////////////////////////////////////////////
// Exported methods ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int gCounter = 0;

extern "C" {

// This is an example of an exported function.
SIMPLEDLL_API int NextInt(void)
{
	return ++gCounter;
}
SIMPLEDLL_API void SetNextInt(int x)
{
	gCounter = x - 1;
}

SIMPLEDLL_API int AddInts(int a, int b)
{
	return a+b;
}
SIMPLEDLL_API int AddFourInts(int a, int b, int c, int d)
{
	return a+b+c+d;
}

SIMPLEDLL_API double AddDoubles(double x, double y)
{
	return x+y;
}
SIMPLEDLL_API double* AddDoublesIndirect(double* x, double* y)
{
	static double z;
	z = *x + *y;
	return &z;
}
SIMPLEDLL_API char CharStringArgument(const char* string)
{
	return string[0];
}
SIMPLEDLL_API wchar_t WCharStringArgument(const wchar_t* string)
{
	return string[0];
}
SIMPLEDLL_API const char* ReturnString()
{
	return "Returning a static string!";
}
SIMPLEDLL_API const wchar_t* ReturnWString()
{
	return L"Returning a static widestr!";
}
SIMPLEDLL_API wchar_t* ReturnBSTR()
{
	return SysAllocString(L"Returning BSTR (SysAllocString)");
}

SIMPLEDLL_API POINT MakePoint(int x, int y)
{
	POINT p = { x, y };
	return p;
}
SIMPLEDLL_API POINT* MakePointIndirect(int x, int y)
{
	static POINT p;
	p.x = x;
	p.y = y;
	return &p;
}

SIMPLEDLL_API int GetPointY(POINT p)
{
	return p.y;
}
SIMPLEDLL_API int GetPointYIndirect(POINT* p)
{
	return p->y;
}

SIMPLEDLL_API int SumIntegers(int* array, int size)
{
	int sum = 0;
	for (int i = 0; i < size; i++)
		sum += array[i];
	return sum;
}

SIMPLEDLL_API void NoOp() { }

}