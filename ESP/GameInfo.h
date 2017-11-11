#pragma once
#include "PhysicalMemory.h"
#include "PMemHelper.h"

class GameInfo
{
public:
	GameInfo(PGameMemHelper *pMem);
	~GameInfo();

private:
	PGameMemHelper *pMem;
};

