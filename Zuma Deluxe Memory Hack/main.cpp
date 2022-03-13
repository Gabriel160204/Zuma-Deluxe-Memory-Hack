#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>

DWORD GetModule(LPCWSTR ModuleName, DWORD pID)
{
	DWORD dGetModule = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 Module = { 0 };
	Module.dwSize = sizeof(Module);

	if (hSnapShot != INVALID_HANDLE_VALUE)
	{
		if (Module32First(hSnapShot, &Module))
		{
			do
			{
				if (wcscmp(Module.szModule, ModuleName) == 0)
				{
					dGetModule = (DWORD)Module.modBaseAddr;
					CloseHandle(hSnapShot);
					break;
				}
			} while (Module32Next(hSnapShot, &Module));
		}
	}
	else { std::wcerr << "INVALID HANLDE VALUE\n"; Sleep(3000); exit(-1); }
	return dGetModule;
}

DWORD GetPtrAdress(HANDLE phandle, std::vector<DWORD> offsets, DWORD ModuleNameAdress, DWORD Adress)
{
	DWORD PtrAdress = ModuleNameAdress + Adress;
	for (int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(phandle, (LPVOID)PtrAdress, &PtrAdress, sizeof(PtrAdress), 0);
		PtrAdress += offsets[i];
	}
	return PtrAdress;
}

int main(void)
{
	HWND hwnd = FindWindow(NULL, L"Zuma Deluxe 1.0");
	if (hwnd == NULL)
	{
		std::wcerr << "Cannot find the window\n";
		Sleep(3000);
		exit(-1);
	}
	else
	{
		DWORD pID;
		GetWindowThreadProcessId(hwnd, &pID);
		HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);
		if (phandle == NULL || phandle == INVALID_HANDLE_VALUE)
		{
			std::wcerr << "Open Process Error\n";
			Sleep(3000);
			exit(-1);
		}
		else
		{
			LPCWSTR modulename = L"popcapgame1.exe";
			DWORD ScoreAdress = 0x00186F18;
			std::vector<DWORD> offsets = { 0x1A0, 0x2C, 0x160, 0xE8 };

			DWORD ModuleAdress = GetModule(modulename, pID);
			DWORD PTRAdress = GetPtrAdress(phandle, offsets, pID, ModuleAdress, ScoreAdress);

			std::cout << "Pointer Adress: " << std::hex << PTRAdress << std::endl << std::endl;
			while (true)
			{
				unsigned long int NewValue = 0, OldValue = 0;
				std::wcout << "Enter a new Value: "; std::cin >> NewValue;

				ReadProcessMemory(phandle, (LPVOID)PTRAdress, &OldValue, sizeof(OldValue), 0);
				WriteProcessMemory(phandle, (LPVOID)PTRAdress, &NewValue, sizeof(NewValue), 0);

				std::wcout << "Old Value: " << OldValue << std::endl;
				std::wcout << "New Value: " << NewValue << std::endl << std::endl;
			}
		}
	}
	return 0;
}
