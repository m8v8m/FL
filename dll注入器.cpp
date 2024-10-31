#include "stdio.h"  
#include "windows.h"  
#include "tlhelp32.h"  
#include "winbase.h"  
#include "tchar.h"  
#include<iostream>
using namespace std;

BOOL InjectDll(DWORD dwPID, LPCSTR szDllName)
{
	HANDLE hProcess, hThread;
	LPVOID pRemoteBufferData; 
	DWORD dwBufSize = (DWORD)(strlen(szDllName) + 1) * sizeof(CHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	pRemoteBufferData = VirtualAllocEx(hProcess, 0, dwBufSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pRemoteBufferData, (LPVOID)szDllName, dwBufSize, NULL);
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBufferData, 0, 0);
	return TRUE;
}

BOOL EjectDll(DWORD dwPID, LPCSTR szDllName)
{
	HANDLE hSnapshot, hProcess, hThread;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	Module32First(hSnapshot, &me);
	while (Module32Next(hSnapshot, &me))
	{if (!strcmp(me.szModule, szDllName) || !strcmp(me.szExePath, szDllName)){break;}}
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
	return TRUE;
}
int _tmain()
{

	DWORD pid;
	const CHAR* dll="Z:\\desktop\\code\\hid64.dll";
	pid=26544;
	//EjectDll(pid,dll);
	InjectDll(pid,dll);
	return 0;
}
