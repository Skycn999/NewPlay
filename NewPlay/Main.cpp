#include "common.hpp"
#include "Overlay.h"


DWORD getGamePid() {
	return 0;
}


int main() {	
	PGameMemHelper *pMem = new PGameMemHelper();

	DWORD gamePid = getGamePid();
	if (!pMem->InitGameMem(gamePid)) {
		cout << "Init Memory failed" << endl;
		return 1;
	}

	Overlay * p_esp_overlay = new Overlay(pMem);
	p_esp_overlay->run();
	
	system("pause");
	return 0;
}