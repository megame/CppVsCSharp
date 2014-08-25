//
// Paths.cpp
// Author: David Piepgrass
// Copyright 2006-11 Mentor Engineering, Inc.
//
#include "StdAfx.h"
#include "Paths.h"
#include "Misc.h"
#define new NEW
using namespace std;

int GetDriveOrDomainLen (string path, bool includeDomain, bool wantTrailingSlash)
{
	int slashes = -1;
	int i;

	for (i = 0; ; i++) {
		if (i == path.size()) {
			// There are no colons or slashes found, so there is NO 
			// drive/protocol/domain in it.
			return 0;
		}
		if (path[i] == ':')
			break;
		if (IsPathSeparator(path[i])) {
			if (i == 0) {
				slashes++;
				break;
			} else
				return 0; // Relative path detected
		}
	}
	do {
		if (++i == path.size()) return i;
		slashes++;
	} while (IsPathSeparator(path[i]));

	if (slashes > 1 && includeDomain) {
		// Multiple slashes indicates a domain (or volume) name.

		// Find out where the name of the volume/domain ends.
		do
			if (++i == path.size()) return i;
		while (!IsPathSeparator(path[i]));
		
		if (wantTrailingSlash)
			++i;
	}
	return i;
}

int FindBeginningOfPriorPath (string path, int startAt, bool stopAtColon)
{
	int i = startAt - 1;
	if (i < 0 || (path[i] == ':' && stopAtColon))
		return startAt;
	if (IsPathSeparator(path[i])) {
		i--;
		if (i < 0 || IsPathSeparator(path[i]) 
			|| (path[i] == ':' && stopAtColon))
			return startAt;
	}
	for (i--; i >= 0 && !IsPathSeparator(path[i]); i--) { }
	return i + 1;
}

int FindExtensionDot(const string& path, bool allowAtBeginning)
{
	size_t stopAt = allowAtBeginning ? 0 : 1;
	for (size_t i = path.size() - 1; i >= stopAt; i--) 
	{
		if (IsPathSeparator(path[i]) || path[i] == ':')
			return -1;
		if (path[i] == '.') {
			if (allowAtBeginning || (!IsPathSeparator(path[i - 1]) && path[i - 1] != ':'))
				return (int)i;
			else
				return -1;
		}
	}
	return -1;
}

string RemoveExtension(const string& path, bool evenFromBeginning)
{
	int i = FindExtensionDot(path, evenFromBeginning);
	if (i == -1)
		return path;
	else
		return path.substr(0, i);
}

string GetExtension(const string& path, bool evenFromBeginning, bool removeDot)
{
	int i = FindExtensionDot(path, evenFromBeginning);
	if (i == -1)
		return string();
	else {
		assert (path[i] == '.');
		return path.substr(i + (int)removeDot);
	}
}

string CombinePaths(const string& path1, const string& path2)
{
	if (IsPathRooted(path2) || path1.empty())
		return path2;
	else if (path2.empty())
		return path1;
	else if (IsPathSeparator(path1[path1.size() - 1]))
		return path1 + path2;
	else {
		string out(path1);
		out.append(1, GetPathSeparator(path1));
		out.append(path2);
		return out;
	}
}

char GetPathSeparator(string path)
{
	for (int i = 0; i < (int)path.size(); i++)
		if (IsPathSeparator(path[i])) return path[i];
	return '\\';
}

////////////////////////////////////////////////////////////////////////////////
// UTF16 to UTF8 conversion
////////////////////////////////////////////////////////////////////////////////

uint32 NextCharFromUTF16(const wchar_t*& src)
{
	if ((src[0] & 0xFC00) == 0xD800 &&
		(src[1] & 0xFC00) == 0xDC00) {
			uint32 result = ((uint32)(src[0] & 0x3FF) << 10)
				+ (src[1] & 0x3FF) + 0x10000;
			src += 2;
			return result;
		} else
			return *src++;
}

int GetSizeAsUTF8(const wchar_t* src)
{
	if (src == NULL)
		return 1;

	int size = 0;
	uint32 c;
	do {
		c = NextCharFromUTF16(src);
		size++;
		if (c >= 0x80) {
			size++;
			if (c >= 0x800) {
				size++;
				if (c >= 0x10000)
					size++;
			}
		}
	} while (c != '\0');
	return size;
}

int UTF16toUTF8(const wchar_t* src, char* dest)
{
	char* dest_start = dest;
	
	if (src == NULL)
		*dest++ = '\0';
	else {
		uint32 c;
		do {
			c = NextCharFromUTF16(src);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (c >= 0x10000) {
						dest[3] = 0x80 | (c & 0x3F);
						dest[2] = 0x80 | ((c >> 6) & 0x3F);
						dest[1] = 0x80 | ((c >> 12) & 0x3F);
						dest[0] = 0xF0 | (c >> 18);
						dest += 4;
					} else {
						dest[2] = 0x80 | (c & 0x3F);
						dest[1] = 0x80 | ((c >> 6) & 0x3F);
						dest[0] = 0xE0 | (c >> 12);
						dest += 3;
					}
				} else {
					dest[1] = 0x80 | (c & 0x3F);
					dest[0] = 0xC0 | (c >> 6);
					dest += 2;
				}
			} else
				*dest++ = c;
		} while (c != '\0');
	}
	return (int)(dest - dest_start);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string GetPathOfEXE()
{
	wchar_t exeFn[MAX_PATH+1];
	GetModuleFileNameW(NULL, exeFn, MAX_PATH);
	char exeFn2[MAX_PATH*3];
	UTF16toUTF8(exeFn, exeFn2);
	return string(exeFn2);
}

string gHomeFolder;
