#include "common.hpp"
#include "Overlay.h"
#include "GameInfo.h"
#include <TlHelp32.h>


DWORD GetProcessID(WCHAR* processName)
{
	DWORD processID = 0;
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = 0;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == 0)
		goto EXIT;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe32))
		goto EXIT;
	do
	{
		if (!wcscmp(wstrEncrypt(pe32.szExeFile,2), processName)) {
			processID = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
EXIT:
	if (hSnapshot)
		CloseHandle(hSnapshot);
	return processID;
}

DWORD WaitProcessID(WCHAR* processName) {
	DWORD pid = GetProcessID(processName);
	int i = 0;
	while (!pid) {
		std::cout << "Waiting for game start:" <<i++<< std::endl;
		Sleep(10000);
		pid = GetProcessID(processName);
	}
	return pid;
}


int main() {	
	PGameMemHelper *pMem = new PGameMemHelper();
	
	DWORD gamePid = WaitProcessID(GAME_NAME);
	cout << "进程ID:" << gamePid << endl;

	if (!pMem->InitGameMem(gamePid)) {
		cout << "内存模块初始化失败" << endl;
		return 1;
	}

	GameInfo * pGame = new GameInfo(pMem);
	cout << "Wait 30s for Game start up" << endl;
	Sleep(30000);
	cout << "Try to get Name" << endl;
	pGame->CacheNames();
	cout << "Get Name Done" << endl;

	cout << "启动覆盖" << endl;
	Overlay::init(pGame);
	cout << "进入循环" << endl;
	Overlay::run();

	return 0;
}