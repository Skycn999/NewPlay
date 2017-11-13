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
		if (wcsstr(pe32.szExeFile, processName)) {
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
	while (!pid) {
		std::cout << "Waiting for game start..." << std::endl;
		Sleep(1000);
		pid = GetProcessID(processName);
	}
	return pid;
}


int main() {	

	PGameMemHelper *pMem = new PGameMemHelper();

	DWORD gamePid = WaitProcessID(L"ame.exe");
	if (!pMem->InitGameMem(gamePid)) {
		cout << "内存模块初始化失败" << endl;
		return 1;
	}

	GameInfo * pGame = new GameInfo(pMem);

	Sleep(30000);
	cout << "Try to get Name" << endl;
	pGame->CacheNames();
	cout << "Get Name Done" << endl;

	Overlay::init(pGame);
	Overlay::run();

	return 0;
}