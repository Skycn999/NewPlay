#pragma once
#include "GameInfo.h"

namespace Overlay {
	void run();
	void init(GameInfo *pG);

	void render();
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
