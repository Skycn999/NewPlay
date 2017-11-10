#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>


#define COLOR_GREEN           D3DCOLOR_ARGB(255, 0, 255, 0)
#define COLOR_YELLOW          D3DCOLOR_ARGB(255, 255, 255, 0)
#define COLOR_RED             D3DCOLOR_ARGB(255, 255, 0, 0)
#define COLOR_PINK            D3DCOLOR_ARGB(255, 255, 20, 147)
#define COLOR_WHITE           D3DCOLOR_ARGB(255, 255, 255, 255)

#define COLOR_VEHICLE         D3DCOLOR_ARGB(255, 179, 238, 58)
#define COLOR_LOOT            D3DCOLOR_ARGB(255, 0, 191, 255)
#define COLOR_WEAPEN          D3DCOLOR_ARGB(255, 255, 165, 0)
#define COLOR_OTHER           D3DCOLOR_ARGB(255, 0, 238, 118)
#define COLOR_BULLET          D3DCOLOR_ARGB(255, 255, 165, 0)

#define COLOR4            D3DCOLOR_ARGB(255, 20, 255, 100)
#define COLOR5            D3DCOLOR_ARGB(255, 238, 0, 238)
#define COLOR2            D3DCOLOR_ARGB(255, 0, 255, 255)

typedef enum {
	OP_NONE,
	OP_INFOR,
	OP_SKELETON
}PLAYER_SHOW_OPTION;

class Overlay
{
public:
	Overlay();
	~Overlay();
	void run();

private:
	void SetupWindow();
	void initD3D(HWND hWnd);
	void render();
	void ESP();
	void inline PlayerEsp();
	void inline VehicleEsp();
	void inline LootEsp();
	void inline ShowMenu();


	void GetAllObjects(); //Get all the objects need to ESP
	void UpdateUserSetting(); //Get the user setting

private:
	PGameMemHelper *pMem;
	HWND twnd;  //Game Window handle
	HWND hWnd;  //Overlay Window handle
	int s_width;  
	int s_height;

	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 d3ddev;
	MARGINS  margin = { 0,0,s_width,s_height };
	LPD3DXFONT pFont;
	ID3DXLine* d3dLine;

    //User setting for the ESP
	PLAYER_SHOW_OPTION o_player;
	bool o_vehicle;
	bool o_loot;
	bool o_menu;
	bool debug;
};

