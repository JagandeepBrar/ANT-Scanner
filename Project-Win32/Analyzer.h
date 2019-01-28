#pragma once

#include <windows.h>  

#include "resource.h"
#include "initialize.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
const char szTitle[] = "ANT+ Analyzer";
const char szWindowClass[] = "analyzer";

LRESULT CALLBACK WndProc(
	HWND	hwnd,
	UINT	uMsg,
	WPARAM wParam,
	LPARAM lParam
);

void refresh();