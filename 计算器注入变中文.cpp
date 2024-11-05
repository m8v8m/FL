// include  
#include "stdio.h"  
#include "wchar.h"  
#include "windows.h"  


// typedef  
typedef BOOL(WINAPI *PFSETWINDOWTEXTW)(HWND hWnd, LPWSTR lpString); //SetWindowsTextW()的地址


// 原函数地址
FARPROC g_pOrginalFunction = NULL;


BOOL WINAPI MySetWindowTextW(HWND hWnd, LPWSTR lpString)
{
	wchar_t* pNum = L"零一二三四五六七八九";
	wchar_t temp[2] = { 0, };
	int i = 0, nLen = 0, nIndex = 0;
    wchar_t J[] = L"加";
    wchar_t j[]=L"+";
	nLen = wcslen(lpString);
	for (i = 0; i < nLen; i++)
	{
		if (L'5' == lpString[i])
		{
			temp[0] = lpString[i];
			nIndex = _wtoi(temp);
			lpString[i] = pNum[nIndex];
		}else if(j[0] == lpString[i]){

			lpString[i] = J[0];
        }
	}

	return ((PFSETWINDOWTEXTW)g_pOrginalFunction)(hWnd, lpString);

}


BOOL hook_iat(LPCSTR szDllName, PROC pfnOrg, PROC pfnNew)
{
	HMODULE hMod;
	LPCSTR szLibName;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect, dwRVA;
	PBYTE pAddr;

	// hMod, pAddr = ImageBase of calc.exe  
	//             = VA to MZ signature (IMAGE_DOS_HEADER)  
	hMod = GetModuleHandle(NULL);
	pAddr = (PBYTE)hMod;

	// pAddr = VA to PE signature (IMAGE_NT_HEADERS)  
	pAddr += *((DWORD*)&pAddr[0x3C]);

	// dwRVA = RVA to IMAGE_IMPORT_DESCRIPTOR Table  
	dwRVA = *((DWORD*)&pAddr[0x80]);

	// pImportDesc = VA to IMAGE_IMPORT_DESCRIPTOR Table  
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hMod + dwRVA);

	for (; pImportDesc->Name; pImportDesc++)
	{
		// szLibName = VA to IMAGE_IMPORT_DESCRIPTOR.Name  
		szLibName = (LPCSTR)((DWORD)hMod + pImportDesc->Name);
		if (!_stricmp(szLibName, szDllName))
		{
			// pThunk = IMAGE_IMPORT_DESCRIPTOR.FirstThunk  
			//        = VA to IAT(Import Address Table)  
			pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod +
				pImportDesc->FirstThunk);

			// pThunk->u1.Function = VA to API  匹配成功
			for (; pThunk->u1.Function; pThunk++)
			{
				if (pThunk->u1.Function == (DWORD)pfnOrg)
				{
					// 更改为可读写模式  
					VirtualProtect((LPVOID)&pThunk->u1.Function,
						4,
						PAGE_EXECUTE_READWRITE,
						&dwOldProtect);

					// 修改IAT的值  
					pThunk->u1.Function = (DWORD)pfnNew;

					//修改完成后，恢复原保护属性
					VirtualProtect((LPVOID)&pThunk->u1.Function,
						4,
						dwOldProtect,
						&dwOldProtect);

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// 保存原始API的地址  
		g_pOrginalFunction = GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetWindowTextW");

		// # hook  
		//   用hookiat.MySetWindowText钩取user32.SetWindowTextW  
		hook_iat("user32.dll", g_pOrginalFunction, (PROC)MySetWindowTextW);
		break;

	case DLL_PROCESS_DETACH:
		// # unhook  
		//   将calc.exe的IAT恢复原值  
		hook_iat("user32.dll", (PROC)MySetWindowTextW, g_pOrginalFunction);
		break;
	}

	return TRUE;
}

