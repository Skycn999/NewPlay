#pragma once

#include <windows.h>

#pragma comment(lib,"Version.lib")

enum Windows {
	WINDOWS7,
	WINDOWS8,
	WINDOWS81,
	WINDOWS10,
	UNSUPPORTED
};
Windows getVersion();