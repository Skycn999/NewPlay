#pragma once
#include "GameInfo.h"

namespace Overlay {
	//Public interface
	void run();
	void init(GameInfo *pG);
	
	//Internal declearation
	void render();
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
