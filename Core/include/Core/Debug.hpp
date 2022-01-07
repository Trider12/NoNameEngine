#pragma once

#include <stdio.h>
#include <string>

#if defined(_DEBUG) || !defined(NDEBUG)
#define DEBUG 1
#else
#define DEBUG 0
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _WIN32

#define PRINT_ERROR(x, ...) do { if (DEBUG) fputs("Error: ", stderr); fprintf(stderr, x, __VA_ARGS__); fputs("\n", stderr); } while (0)

#define PRINT_WARNING(x, ...) do { if (DEBUG) fputs("Warning: ", stderr); fprintf(stderr, x, __VA_ARGS__); fputs("\n", stderr); } while (0)

class Debug final
{
public:
	template<typename... Args>
	static void Error(const std::string& fmt, Args... args) { PRINT_ERROR(fmt.c_str(), args...); }

	template<typename... Args>
	static void Warning(const std::string& fmt, Args... args) { PRINT_WARNING(fmt.c_str(), args...); }

	static void SwitchConsole()
	{
#ifdef _WIN32
#ifdef DEBUG
		::ShowWindow(::GetConsoleWindow(), SW_SHOW);
#else
		::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif // DEBUG
#endif // _WIN32
	}
};