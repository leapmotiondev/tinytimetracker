#pragma once

// windows api headers
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <commctrl.h>

// link libraries
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib") 
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "comctl32.lib")

// set subsystem to windows
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
