#include "stdafx.h"
#include "Misc.h"
using namespace std;

string printstring(const char* fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	string s = vprintstring(fmt, l);
	va_end(l);
	return s;
}

string vprintstring(const char* fmt, va_list l)
{
#if !defined(WIN32) || defined(UNDER_CE)
	// _vscprintf will not exist.
	static char buf[2048];
	vsprintf(buf, fmt, l);
	string s(buf);
#else
	int c = _vscprintf(fmt, l);
	string s(c, '\0');
	int c2 = vsprintf_s(&*s.begin(), c + 1, fmt, l);
	assert (c == c2);
#endif
	return s;
}

#if defined(UNDER_CE) || !defined(WIN32)

// Not supported in WinCE; just let the data scroll by....
int GetCursorTop() { return 0; }
int GetCursorLeft() { return 0; }
void SetCursorPosition(int x, int y) { }

#else

// I only know how the cursor is controlled in Win32
COORD GetCursorPosition()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &info);
	return info.dwCursorPosition;
}
int GetCursorTop()
{
	return GetCursorPosition().Y;
}
int GetCursorLeft()
{
	return GetCursorPosition().X;
}
void SetCursorPosition(int x, int y)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y }; 
	SetConsoleCursorPosition( hStdout, position );
}

#endif

namespace Math
{
	int CountOnes(uint8 x)
	{
		x -= ((x >> 1) & 0x55);
		x = (((x >> 2) & 0x33) + (x & 0x33));
		return (int)((x & 0x0F) + (x >> 4));
	}
	inline int CountOnes(uint16 x)
	{
		x -= ((x >> 1) & 0x5555);
		x = (((x >> 2) & 0x3333) + (x & 0x3333));
		x = (((x >> 4) + x) & 0x0f0f);
		x += (x >> 8);
		return (int)(x & 0x001f);
	}
	inline int CountOnes(uint32 x)
	{
		/* 
		 * 32-bit recursive reduction using SWAR... but first step 
		 * is mapping 2-bit values into sum of 2 1-bit values in 
		 * sneaky way
		 */
		x -= ((x >> 1) & 0x55555555);
		x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
		x = (((x >> 4) + x) & 0x0f0f0f0f);
		x += (x >> 8);
		x += (x >> 16);
		return (int)(x & 0x0000003f);
	}
	int CountOnes(uint64 x)
	{
		x -= ((x >> 1) & 0x5555555555555555u);
		x = (((x >> 2) & 0x3333333333333333u) + (x & 0x3333333333333333u));
		x = (((x >> 4) + x) & 0x0f0f0f0f0f0f0f0fu);
		x += (x >> 8);
		x += (x >> 16);
		int32 x32 = int32(x) + int32(x >> 32);
		return (int)(x32 & 0x0000007f);
	}

	int Log2Floor(uint32 x)
	{
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return (CountOnes(x) - 1);
	}
	int Log2Floor(uint64 x)
	{
		uint32 xHi = uint32(x >> 32);
		if (xHi)
			return 32 + Log2Floor(xHi);
		return Log2Floor((uint32)x);
	}
	int Log2Floor(uint16 x)
	{
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		return (CountOnes(x) - 1);
	}

	int64 Sqrt(int64 value)
	{
		if (value < 0)
			throw exception("Cannot take square root of a negative integer");
		return Sqrt((uint64)value);
	}

	uint32 Sqrt(uint64 value)
	{
		if (value == 0)
			return 0;

		uint g = 0;
		int bshft = Log2Floor(value) >> 1;
		uint b = 1u << bshft;
		do {
			uint64 temp = ((uint64)(g + g + b) << bshft);

			if (value >= temp)
			{
				g += b;
				value -= temp;
			}
			b >>= 1;
		} while (bshft-- > 0);

		return g;
	}

	int32 Sqrt(int32 value)
	{
		if (value < 0)
			throw exception("Cannot take square root of a negative integer");
		return Sqrt((uint32)value);
	}

	uint32 Sqrt(uint32 value)
	{
		if (value != 0) {
			uint32 g = 0;
			// bshift must be at least this value, but it will work if greater
			int32 bshft = Log2Floor(value) >> 1;
			uint32 b = 1u<<bshft;
			do {
				uint32 temp = (g+g+b)<<bshft;
				if (value >= temp) {
					g += b;
					value -= temp;
				}
				b>>=1;
			} while (bshft-- != 0);

			return g;
		}
		else
			return 0;
	}

	bool IsPrime(int candidate)
	{
		assert(candidate >= 0);
		if (!(candidate & 1))
			return candidate == 2;

		int root = Sqrt(candidate);
		for (int i = 3; i <= root; i += 2)
			if ((candidate % i) == 0)
				return false;

		return candidate > 1;
	}
	int GetNextPrime(int min)
	{
		int i;
		for (i = min | 1; i < INT_MAX; i += 2)
			if (IsPrime(i))
				break;
		return i;
	}
}