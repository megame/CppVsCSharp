/*
	Paths.h/cpp: contains functions for processing file system paths

	This is a port of part of the C# PathEx class.
	Also, CombinePaths() works like the .NET function Path.Combine().
	
  Author: David Piepgrass
  Copyright 2006-11 Mentor Engineering, Inc.
*/

#ifndef _COMMON_PATHS_H
#define _COMMON_PATHS_H

#include <string>

inline bool IsPathSeparator(int c)
	{ return c == '\\' || c == '/'; }

/// <summary>Behaves like <see cref="GetDriveOrDomain"/>, except that
/// this function returns the length of the drive or volume string
/// rather than the string itself.
/// </summary>
/// <param name="wantTrailingSlash">If true, then the trailing slash on 
/// a computer or domain name is included; otherwise, it is omitted.
/// This parameter does not apply if if includeDomain is false.</param>
/// <returns>The number of characters at the beginning of the path
/// which represent the drive or volume name</returns>
int GetDriveOrDomainLen (std::string path, bool includeDomain = false, bool wantTrailingSlash = false);

/// <summary>
/// Determines how much of the beginning of the path string 
/// represents a drive, volume, protocol, or domain name, and returns
/// it, including trailing slashes.  If the path does not appear to be
/// an absolute path, then an empty string (not null) is returned.
/// </summary>
/// <param name="includeDomain">If this is true, then the return value 
/// includes the computer or domain name; if false, then the return 
/// value includes only the drive or protocol specification and the
/// initial slash(es).
/// <param name="wantTrailingSlash">If true, then the trailing slash on 
/// a computer or domain name is included; otherwise, it is omitted.
/// This parameter does not apply if if includeDomain is false.</param>
/// </param>
/// <remarks>
/// This function assumes that the path is well-formed.  In other
/// words, you can expect garbage-in-garbage-out.  The includeDomain
/// parameter has an effect only if the function believes that the
/// path includes a computer or domain name, which is indicated by
/// the presence of more than one slash.  For example, in
/// "file://A/B", there are two slashes, so A is assumed to be
/// the "volume name".  On the other hand, in "C:/A/B", there is only
/// one slash, so A is considered to be a folder name.  See the 
/// examples below for the consequences of this interpretation.
/// <p>Examples (where includeDomain and wantTrailingSlash are true 
/// unless said otherwise):</p>
/// <code>
/// \Test                             => \
/// \\ComputerName                    => \\ComputerName
/// \\ComputerName                    => \\ (if !includeDomain)
/// \\ComputerName\Test               => \\ComputerName\
/// C:\Windows                        => C:\
/// http://www.google.com/index.html  => http://www.google.com/
/// http://www.google.com/index.html  => http://www.google.com (if !wantTrailingSlash)
/// file:///C:/Windows                => file:///C:/
/// file:///C:/Windows                => file:///C:/ (if !wantTrailingSlash)
/// file:///C:/Windows                => file:///    (if !includeDomain)
/// C:D:E                             => C:
/// C:D/E                             => C:
/// file///C:/Windows                 => (returns String.Empty)
/// Relative/Path                     => (returns String.Empty)
/// Hello                             => (returns String.Empty)
/// </code>
/// </remarks>
inline std::string GetDriveOrDomain (std::string path, bool includeDomain, bool wantTrailingSlash)
{
	return path.substr(0, GetDriveOrDomainLen (path, includeDomain, wantTrailingSlash));
}

/// Gets a value indicating whether the specified path string contains an 
/// absolute or relative path.
inline bool IsPathRooted(const std::string& path) { return GetDriveOrDomainLen(path) > 0; }

int FindBeginningOfPriorPath (std::string path, int startAt, bool stopAtColon);

/// <summary>
/// Equivalent to FindBeginningOfPriorPath(path, path.size()).
/// </summary>
inline int FindBeginningOfLastPath (std::string path, bool stopAtColon)
{
	return FindBeginningOfPriorPath(path, (int)path.size(), stopAtColon);
}

/// <summary>Removes the last path component.  Returns the same
/// string if the path string ends in multiple slashes.  Also, if 
/// stopAtColon is true, returns the same path if the final path
/// component ends in a colon (e.g. X/Y/Z: or C:\)
/// </summary>
/// <remarks>
/// Equivalent to path.substr(0, FindBeginningOfLastPath(path)).
/// Examples (assuming stopAtColon is true):
/// <code>
/// C:\                                 => C:\
/// C:\                                 => (empty string if !stopAtColon)
/// http://                             => http://
/// test//                              => test//
/// test/                               => (empty string)
/// http://www.google.ca                => http://
/// http://www.google.ca/               => http://
/// http://www.google.ca/index.html     => http://www.google.ca/
/// C:\Test                             => C:\
/// C:\A\B                              => C:\A\
/// C:\A\B\                             => C:\A\
/// </code></remarks>
inline std::string RemoveLastPath (std::string path, bool stopAtColon)
{
	return path.substr(0, FindBeginningOfLastPath(path, stopAtColon));
}
inline std::string RemoveLastPath (std::string path)
{
	return path.substr(0, FindBeginningOfLastPath(path, false));
}

/// <summary>
/// Returns the index of the dot (.) that is the beginning of the extension
/// in the specified path, or -1 if the filename has no extension.
/// </summary>
/// <param name="allowAtBeginning">If true, a dot at the beginning of a 
/// filename is not considered an extension (filenames that begin with dots 
/// represent hidden files on Unix and Linux.)</param>
/// <remarks>FindExtensionDot will only examine the last path component 
/// looking for an extension, and if the path ends with a slash or backslash,
/// it will always return -1.</remarks>
int FindExtensionDot(const std::string& path, bool allowAtBeginning);
inline int FindExtensionDot(std::string path) 
{
	return FindExtensionDot(path, false); 
}

/// <summary>Removes the extension from the filename part of the path.
/// Returns the same string if it has no extension.
/// </summary>
/// <remarks>
/// Equivalent to path.Substring(0, FindExtensionDot(path, evenFromBeginning))
/// if FindExtensionDot does not return -1. Examples (assuming 
/// evenFromBeginning is false):
/// <code>
/// test.jpg                            => test
/// http://www.google.ca/index.html     => http://www.google.ca/index
/// double extension.txt.zip            => double extension.txt
/// readme.txt                          => readme
/// /path/readme.txt                    => /path/readme
/// .readme                             => .readme
/// /path/.readme                       => /path/.readme
/// C:\                                 => C:\
/// a.b:c                               => a.b:c
/// a.b:.c                              => a.b:.c
/// </code></remarks>
std::string RemoveExtension(const std::string& path, bool evenFromBeginning);

/// GetExtension returns the extension of 'path', if any, including the dot
/// unless removeDot is true.
std::string GetExtension(const std::string& path, bool evenFromBeginning, bool removeDot = false);

/// Combines two path strings.
/// 
/// If one of the specified paths is a zero-length string, this method returns 
/// the other path. If path2 contains an absolute path, this method returns path2.
std::string CombinePaths(const std::string& path1, const std::string& path2);
inline std::string CombinePaths(const char* path1, const char* path2)
	{ std::string p1(path1), p2(path2); return CombinePaths(p1, p2); }

char GetPathSeparator(std::string path);

// Bonus functions :)

std::string GetPathOfEXE();

inline std::string GetFolderOfEXE()
{
	return RemoveLastPath(GetPathOfEXE());
}

extern std::string gHomeFolder; // "g" prefix for global variable

// This function provides a way for high-level code to give low-level code a
// default location for files it might create (e.g. log files). Low-level code
// should use this function rather than calling GetFolderOfExe(), so that
// high-level code can override the "home folder" if it wants to.
inline const std::string& GetHomeFolder()
{
	if (gHomeFolder.empty())
		gHomeFolder = GetFolderOfEXE();
	return gHomeFolder;
}

#endif