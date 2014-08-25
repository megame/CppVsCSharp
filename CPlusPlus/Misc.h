#ifndef _PRINTSTRING_ETC_H
#define _PRINTSTRING_ETC_H

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include "num_traits.h"

#ifndef OUT
#define OUT /* marks output parameters */
#endif

// I prefer to format strings the old-fashioned C way
std::string printstring(const char* fmt, ...);
std::string vprintstring(const char* fmt, va_list l);

int GetCursorLeft();
int GetCursorTop();
void SetCursorPosition(int x, int y);

namespace Math
{
	/// <summary>
	/// Returns the number of 'on' bits in x
	/// </summary>
	int CountOnes(uint8 x);
	int CountOnes(uint16 x);
	int CountOnes(uint32 x);
	int CountOnes(uint64 x);

	/// <summary>
	/// Returns the floor of the base-2 logarithm of x. e.g. 1024 -> 10, 1000 -> 9
	/// </summary><remarks>
	/// The return value is -1 for an input of zero (for which the logarithm is 
	/// technically undefined.)
	/// </remarks>
	int Log2Floor(uint32 x);
	int Log2Floor(uint64 x);
	int Log2Floor(uint16 x);

	int64 Sqrt(int64 value);
	int32 Sqrt(int32 value);
	uint32 Sqrt(uint64 value);
	uint32 Sqrt(uint32 value);

	bool IsPrime(int candidate);
	int GetNextPrime(int min);
}

#endif