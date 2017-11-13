#include "common.hpp"

#include "Overlay.h"
#include "PMemHelper.h"
#include "GameInfo.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"Dwmapi.lib")
   
namespace Overlay {

	typedef enum {
		OP_NONE,
		OP_INFOR,
		OP_SKELETON
	}PLAYER_SHOW_OPTION;

	PGameMemHelper *pMem;
	HWND twnd;  //Game Window handle
	HWND hWnd;  //Overlay Window handle
	int s_width;
	int s_height;

	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 d3ddev;
	LPD3DXFONT pFont;
	ID3DXLine* d3dLine;

	//User setting for the ESP
	PLAYER_SHOW_OPTION o_player = OP_SKELETON;
	bool o_vehicle;
	bool o_loot;
	bool o_menu;
	bool debug;
	GameInfo *pGame;

	void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)
	{
		char buf[2048] = { 0 };
		va_list va_alist;
		RECT FontPos = { x, y, x + 140, y + 17 };
		va_start(va_alist, fmt);
		vsprintf_s(buf, fmt, va_alist);
		va_end(va_alist);
		g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
	}
	void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a)
	{
		D3DXVECTOR2 vLine[2];
		d3dLine->SetWidth(w);
		vLine[0].x = x + w / 2;
		vLine[0].y = y;
		vLine[1].x = x + w / 2;
		vLine[1].y = y + h;

		d3dLine->Begin();
		d3dLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
		d3dLine->End();
	}
	void DrawHealthBar(int x, int y, int health)
	{
		float l, r, g;
		g = health * 2.55;
		r = 255 - g;
		l = (int)(health / 2.5f);
		FillRGB(x - 1, y - 1, l + 2 + 6, 5, 0, 0, 0, 40);
		FillRGB(x, y, l + 6, 3, r, g, 0, 150);
	}
	void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)
	{
		D3DXVECTOR2 dLine[2];
		d3dLine->SetWidth(1.f);
		dLine[0].x = x;
		dLine[0].y = y;
		dLine[1].x = xx;
		dLine[1].y = yy;
		d3dLine->Draw(dLine, 2, color);
	}
	void DrawBox(float x, float y, float width, float height, D3DCOLOR color)
	{
		D3DXVECTOR2 points[5];
		points[0] = D3DXVECTOR2(x, y);
		points[1] = D3DXVECTOR2(x + width, y);
		points[2] = D3DXVECTOR2(x + width, y + height);
		points[3] = D3DXVECTOR2(x, y + height);
		points[4] = D3DXVECTOR2(x, y);
		d3dLine->SetWidth(1);
		d3dLine->Draw(points, 5, color);
	}

	void initD3D(HWND hWnd)
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
	void SetupWindow()
	{
		RECT rc;
		while (!twnd) {
			twnd = FindWindow(L"UnrealWindow", 0);
			Sleep(500);
		}

		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;

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
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MARGINS  margin = { 0,0,s_width,s_height };
		switch (message)
		{
		case WM_PAINT:
			render();
			break;
		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &margin);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}




	D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
	{
		float radPitch = (rot.x * float(M_PI) / 180.f);
		float radYaw = (rot.y * float(M_PI) / 180.f);
		float radRoll = (rot.z * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	Vector3 WorldToScreen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL)
	{
		Vector3 Screenlocation = Vector3(0, 0, 0);
		auto POV = CameraCacheL.POV;
		Vector3 Rotation = POV.Rotation; // FRotator

		D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

		Vector3 vAxisX, vAxisY, vAxisZ;
		vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
		vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);



		Vector3 vDelta = WorldLocation - POV.Location;
		Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		float FovAngle = POV.FOV;
		float ScreenCenterX = s_width / 2.0f;
		float ScreenCenterY = s_height / 2.0f;

		Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
		Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

		return Screenlocation;
	}

	void inline PlayerEsp() {

		DWORD color = 0;

		for (int i = 0; i < pGame->player_num; i++) {
			Vector3 spos = WorldToScreen(pGame->player[i].pos, pGame->cameracache);
			
			if (pGame->player[i].distance > 800.f)
				continue;

			if (pGame->player[i].distance <= 150.f)
				color = COLOR_YELLOW;
			else if (pGame->player[i].distance > 150.f && pGame->player[i].distance <= 300.f)
				color = COLOR_GREEN;
			else
				color = COLOR_WHITE;

			DrawString((int)spos.x + 25, (int)spos.y, color, pFont, "[%3d : %3d]", (int)pGame->player[i].distance, (int)pGame->player[i].health);
			DrawHealthBar((int)spos.x + 25, (int)spos.y + 16, (int)pGame->player[i].health);
		}
		
	}
	void inline VehicleEsp() {

	}
	void inline LootEsp() {

	}
	void inline ShowMenu() {

	}

	
	void UpdateUserSetting() {

	}

	
	void esp_test() {
		DrawString(40, 40, COLOR4, pFont, "人物显示[F2]:%s", "打开");
		DrawHealthBar(100, 100, 100);
		DrawHealthBar(100, 120, 60);
		DrawHealthBar(100, 140, 20);
		DrawLine(200, 200, 200, 400, COLOR_GREEN);
		DrawBox(500, 500, 300, 100, COLOR_RED);

	}
	void ESP() {
		if (o_menu)
			ShowMenu();

		if (o_player != OP_NONE)
			PlayerEsp();

		if (o_vehicle)
			VehicleEsp();

		if (o_loot)
			LootEsp();
	}

	void render()
	{
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0); // clear the window alpha
		d3ddev->BeginScene();    // begins the 3D scene	
		UpdateUserSetting(); //TODO
		
		pGame->update();
		ESP();

		//esp_test();
		d3ddev->EndScene();    // ends the 3D scene
		d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
	}

	void run() {
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
	void init(GameInfo *pG) {
		pGame = pG;
		SetupWindow();
	}
};








