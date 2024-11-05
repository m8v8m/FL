#include <Windows.h>
#include <stdio.h>

HMODULE g_hDll;
HHOOK g_hHook;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
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
	if (nCode >= 0)
	{
		if (!(lParam & 0x80000000))
		{
            
			GetModuleFileNameA(NULL, szPath, MAX_PATH);
			p = strrchr(szPath, '\\');
			if (!_stricmp(p + 1, "notepad.exe"))//只对notepad进程拦截
			{
				return 1;
			}
		}
	}
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
