#include "common.hpp"
#include "Overlay.h"

#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"Dwmapi.lib")
   

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		//render();
		break;
	case WM_CREATE:
		//DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

Overlay::Overlay()
{
	this->SetupWindow();
}

void Overlay::SetupWindow()
{

	RECT rc;
	while (!twnd)
		twnd = FindWindow(L"UnrealWindow", 0);

	if (twnd != NULL)
	{
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
		cout << "OL Close with ERROR:" << GetLastError() << endl;
		Sleep(3000);
		ExitProcess(0);
	}

	
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = L"NewPlay";
	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, wc.lpszClassName, L"", WS_POPUP, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, SW_SHOW);
	initD3D(hWnd);
}

void Overlay::initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information
	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferWidth = s_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = s_height;    // set the height of the buffer

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	D3DXCreateFontA(d3ddev, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
	D3DXCreateLine(d3ddev, &d3dLine);
}



void Overlay::UpdateUserSetting() {
}
void Overlay::GetAllObjects() {
}


void inline Overlay::PlayerEsp() {

}

void inline Overlay::VehicleEsp() {

}

void inline Overlay::LootEsp() {

}

void inline Overlay::ShowMenu() {

}
void Overlay::ESP() {
	UpdateUserSetting();
	GetAllObjects();
	
	if (o_menu)
		ShowMenu();

	if (o_player != OP_NONE)
		PlayerEsp();

	if (o_vehicle)
		VehicleEsp();

	if (o_loot)
		LootEsp();
}
void Overlay::render()
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0); // clear the window alpha
	d3ddev->BeginScene();    // begins the 3D scene				 
	ESP();
	d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void Overlay::run() {
	MSG msg;
	RECT rc;
	while (TRUE)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			exit(0);

		twnd = NULL;
		twnd = FindWindow(L"UnrealWindow", 0);
		if (!twnd)
		{
			cout << "OL shutting" << endl;
			ExitProcess(0);
		}
		ZeroMemory(&rc, sizeof(RECT));
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
		MoveWindow(hWnd, rc.left, rc.top, s_width, s_height, true);

		//render your esp
		render();
		Sleep(OVERLAY_UPDATE_TIME);
	}
}



Overlay::~Overlay()
{
}