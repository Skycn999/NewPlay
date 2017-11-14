#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <xstring>
#include <vector>
#include "Util.h"
#include "STR_E.h"

#define NAME_OFFSET 0x36e9790
#define WORLD_OFFSET 0x37e6988

#define OVERLAY_UPDATE_TIME  10

#define COLOR_GREEN           D3DCOLOR_ARGB(255, 0, 255, 0)
#define COLOR_YELLOW          D3DCOLOR_ARGB(255, 255, 255, 0)
#define COLOR_RED             D3DCOLOR_ARGB(255, 255, 0, 0)
#define COLOR_PINK            D3DCOLOR_ARGB(255, 255, 20, 147)
#define COLOR_WHITE           D3DCOLOR_ARGB(255, 255, 255, 255)

#define COLOR_VEHICLE         D3DCOLOR_ARGB(255, 179, 238, 58)
#define COLOR_LOOT  COLOR_YELLOW          D3DCOLOR_ARGB(255, 0, 191, 255)
#define COLOR_WEAPEN          D3DCOLOR_ARGB(255, 255, 165, 0)
#define COLOR_OTHER           D3DCOLOR_ARGB(255, 0, 238, 118)
#define COLOR_BULLET          D3DCOLOR_ARGB(255, 255, 165, 0)

#define COLOR4            D3DCOLOR_ARGB(255, 20, 255, 100)
#define COLOR5            D3DCOLOR_ARGB(255, 238, 0, 238)
#define COLOR2            D3DCOLOR_ARGB(255, 0, 255, 255)

#define TEXT_FONT FW_MEDIUM

#define PLAYER_FAR         800.f
#define PLAYER_NORMAL      300.f
#define PLAYER_NEAR        150.f
#define COLOR_PLAYER_FAR      COLOR_WHITE
#define COLOR_PLAYER_NORMAL   COLOR_GREEN
#define COLOR_PLAYER_NEAR     COLOR_YELLOW
#define M_PI	3.14159265358979323846264338327950288419716939937510



using namespace std;