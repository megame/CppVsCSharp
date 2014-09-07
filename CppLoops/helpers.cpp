#include "pch.h"
#include "helpers.h"

using namespace std;

static string vprintstring(const char* fmt, va_list l)
{
	int c = _vscprintf(fmt, l);
	string s(c, '\0');
	int c2 = vsprintf_s(&*s.begin(), c + 1, fmt, l);

	return s;
}

static wstring vprintwstring(const wchar_t* fmt, va_list l)
{
	int c = _vscwprintf(fmt, l);
	wstring s(c, '\0');
	int c2 = vswprintf_s(&*s.begin(), c + 1, fmt, l);

	return s;
}

string printstring(const char* fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	auto s = vprintstring(fmt, l);
	va_end(l);
	return s;
}

wstring printwstring(const wchar_t* fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	auto s = vprintwstring(fmt, l);
	va_end(l);
	return s;
}
