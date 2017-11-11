#include "common.hpp"
#include "Overlay.h"




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
		if (!wcscmp(processName, pe32.szExeFile)) {
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

	Overlay * p_esp_overlay = new Overlay();
	cout << "Overlay Created" << endl;
	p_esp_overlay->run();

	return 0;
	PGameMemHelper *pMem = new PGameMemHelper();

	DWORD gamePid = WaitProcessID(L"TslGame.exe");
	if (!pMem->InitGameMem(gamePid)) {
		cout << "Init Memory failed" << endl;
		return 1;
	}

	
	system("pause");
	return 0;
}