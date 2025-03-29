
#include <Windows.h>
#include <conio.h>
#include <stdio.h>

typedef void(*HOOKSTART)();
typedef void(*HOOKSTOP)();

int main()
{
	HMODULE hDll = LoadLibraryW(L"one.dll");
	if (!hDll)
	{
		return 0;
	}

	HOOKSTART hookStart = (HOOKSTART)GetProcAddress(hDll, "SetHook");//要导入的函数
	if (!hookStart)
	{
		return 0;
	}
	HOOKSTOP hookStop = (HOOKSTOP)GetProcAddress(hDll, "UnsetHook");
	if (!hookStop)
	{
		return 0;
	}
	hookStart();
	printf("press 'q' to quit\n");

	while (_getch() != 'q')
	{
	}

	hookStop();

	FreeLibrary(hDll);
}
