#include "stdio.h"  
#include "windows.h"  
#include "tlhelp32.h"  
#include "winbase.h"  
#include "tchar.h"  
#include<iostream>
using namespace std;
int FindProcessPid(LPCSTR ProcessName)
{
	DWORD dwPid;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}
	do
	{
		if (!strcmp(ProcessName, pe32.szExeFile))
		{
			dwPid = pe32.th32ProcessID;
			cout<<dwPid;
			break;
		}
		
	} while (Process32Next(hProcessSnap, &pe32));
	
	CloseHandle(hProcessSnap);
	return dwPid;
}
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
	
	const CHAR* dll="C:\\Users\\Administrator\\Desktop\\126\\jump.dll";
	LPCSTR exename="Taskmgr.exe";
	//EjectDll(pid,dll);
	InjectDll(FindProcessPid(exename),dll);
	return 0;
}
