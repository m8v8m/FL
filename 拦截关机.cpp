#include <Windows.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <sddl.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <tchar.h>
const wchar_t* ex=L"main.exe";
//#pragma comment (lib,"Shlwapi.lib"),编译需要添加命令-lShlwapi

LPVOID _copyNtShutdownSystem = NULL;
LPVOID _ExitWindowsExAddTwoByte = NULL;



void MyExitWindowsEx()
{
MessageBoxW(GetConsoleWindow(), L"use ExitWindowsEx", L"tips", MB_OK);
}

typedef BOOL(WINAPI* FuncExitWindowsEx)(_In_ UINT uFlags, _In_ DWORD dwReason);
FuncExitWindowsEx _OldExitWindowsEx = NULL;

//这是 win7 x86上的 Iniline Hook------另外一种方法


BYTE* getNtHdrs(BYTE* pe_buffer)
{
    if (pe_buffer == NULL) return NULL;

    IMAGE_DOS_HEADER* idh = (IMAGE_DOS_HEADER*)pe_buffer;
    if (idh->e_magic != IMAGE_DOS_SIGNATURE) {
        return NULL;
    }
    const LONG kMaxOffset = 1024;
    LONG pe_offset = idh->e_lfanew;
    if (pe_offset > kMaxOffset) return NULL;
    IMAGE_NT_HEADERS32* inh = (IMAGE_NT_HEADERS32*)((BYTE*)pe_buffer + pe_offset);
    if (inh->Signature != IMAGE_NT_SIGNATURE) return NULL;
    return (BYTE*)inh;
}

IMAGE_DATA_DIRECTORY* getPeDir(PVOID pe_buffer, size_t dir_id)
{
    if (dir_id >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return NULL;

    BYTE* nt_headers = getNtHdrs((BYTE*)pe_buffer);
    if (nt_headers == NULL) return NULL;

    IMAGE_DATA_DIRECTORY* peDir = NULL;

    IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)nt_headers;
    peDir = &(nt_header->OptionalHeader.DataDirectory[dir_id]);

    if (peDir->VirtualAddress == NULL) {
        return NULL;
    }
    return peDir;
    
}

bool FixDelayIATHook(PVOID modulePtr)
{    
    IMAGE_DATA_DIRECTORY* importsDir = getPeDir(modulePtr, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
    if (importsDir == NULL) return false;

    size_t maxSize = importsDir->Size;
    size_t impAddr = importsDir->VirtualAddress;

    IMAGE_DELAYLOAD_DESCRIPTOR* lib_desc = NULL;
    size_t parsedSize = 0;
    bool bFound = TRUE;
    
    size_t addrExitWindowsEx = (size_t)GetProcAddress(GetModuleHandleW(L"User32"), "ExitWindowsEx");    
    
    for (; parsedSize < maxSize; parsedSize += sizeof(IMAGE_DELAYLOAD_DESCRIPTOR)) {
        
        lib_desc = (IMAGE_DELAYLOAD_DESCRIPTOR*)(impAddr + parsedSize + (ULONG_PTR)modulePtr);
        
        if (lib_desc->ImportAddressTableRVA == NULL && lib_desc->ImportNameTableRVA == NULL) break;

        LPSTR lib_name = (LPSTR)((ULONGLONG)modulePtr + lib_desc->DllNameRVA);        

        size_t call_via = lib_desc->ImportAddressTableRVA;
        size_t thunk_addr = lib_desc->ImportNameTableRVA;
        if (thunk_addr == NULL) thunk_addr = lib_desc->ImportAddressTableRVA;

        size_t offsetField = 0;
        size_t offsetThunk = 0;        
        for (;; offsetField += sizeof(IMAGE_THUNK_DATA), offsetThunk += sizeof(IMAGE_THUNK_DATA))
        {
            IMAGE_THUNK_DATA* fieldThunk = (IMAGE_THUNK_DATA*)(size_t(modulePtr) + offsetField + call_via);
            IMAGE_THUNK_DATA* orginThunk = (IMAGE_THUNK_DATA*)(size_t(modulePtr) + offsetThunk + thunk_addr);

            if (0 == fieldThunk->u1.Function && 0 == orginThunk->u1.Function)
            {
                break;
            }            
            PIMAGE_IMPORT_BY_NAME by_name = NULL;
            LPSTR func_name = NULL;
            size_t addrOld = NULL;
            if (orginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 || orginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) // check if using ordinal (both x86 && x64)
            {
                addrOld = (size_t)GetProcAddress(LoadLibraryA(lib_name), (char*)(orginThunk->u1.Ordinal & 0xFFFF));//通过序号也可以获取到  获取低两个字节 也可以获取到函数地址
                //printf("        [V] API %x at %x\n", orginThunk->u1.Ordinal, addr);
                //fieldThunk->u1.Function = addr;                
                continue;
            }
            else
            {
                by_name = (PIMAGE_IMPORT_BY_NAME)(size_t(modulePtr) + orginThunk->u1.AddressOfData);
                func_name = (LPSTR)by_name->Name;
                addrOld = (size_t)GetProcAddress(LoadLibraryA(lib_name), func_name);
            }                        
            //printf("        [V] API %s at %x\n", func_name, addr);
            OutputDebugStringA("\r\n");
            OutputDebugStringA(func_name);
            //HOOK            
            if (strcmpi(func_name, "ExitWindowsEx") == 0)
            {                
                //DebugBreak();
                DWORD dOldProtect = 0;
                size_t* pFuncAddr = (size_t*)&fieldThunk->u1.Function;
                if (VirtualProtect(pFuncAddr, sizeof(size_t), PAGE_EXECUTE_READWRITE, &dOldProtect))
                {
                    fieldThunk->u1.Function = (size_t)MyExitWindowsEx;
                    VirtualProtect(pFuncAddr, sizeof(size_t), dOldProtect, &dOldProtect);
                    _OldExitWindowsEx = (FuncExitWindowsEx)addrExitWindowsEx;
                    bFound = true;
                    return bFound;
                }                
                break;
            }

        }
        
    }    
    return true;
}

bool FixIATHook(PVOID modulePtr)
{
    IMAGE_DATA_DIRECTORY* importsDir = getPeDir(modulePtr, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (importsDir == NULL) return false;

    size_t maxSize = importsDir->Size;
    size_t impAddr = importsDir->VirtualAddress;

    IMAGE_IMPORT_DESCRIPTOR* lib_desc = NULL;
    size_t parsedSize = 0;
    bool bFound = TRUE;    
    size_t addrExitWindowsEx = (size_t)GetProcAddress(GetModuleHandleW(L"User32"), "ExitWindowsEx");

    for (; parsedSize < maxSize; parsedSize += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        lib_desc = (IMAGE_IMPORT_DESCRIPTOR*)(impAddr + parsedSize + (ULONG_PTR)modulePtr);
        if (lib_desc->OriginalFirstThunk == NULL && lib_desc->FirstThunk == NULL)
            break;

        LPSTR lib_name = (LPSTR)((size_t)modulePtr + lib_desc->Name);

        size_t call_via = lib_desc->FirstThunk;
        size_t thunk_addr = lib_desc->OriginalFirstThunk;
        if (thunk_addr == NULL)
            thunk_addr = lib_desc->FirstThunk;

        size_t offsetField = 0;
        size_t offsetThunk = 0;
        
        for (;; offsetField += sizeof(IMAGE_THUNK_DATA), offsetThunk += sizeof(IMAGE_THUNK_DATA))
        {
            IMAGE_THUNK_DATA* fieldThunk = (IMAGE_THUNK_DATA*)(size_t(modulePtr) + offsetField + call_via);
            IMAGE_THUNK_DATA* orginThunk = (IMAGE_THUNK_DATA*)(size_t(modulePtr) + offsetThunk + thunk_addr);

            if (0 == fieldThunk->u1.Function && 0 == orginThunk->u1.Function)
            {
                break;
            }
            
            PIMAGE_IMPORT_BY_NAME by_name = NULL;
            LPSTR func_name = NULL;
            size_t addrOld = NULL;
            if (orginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 || orginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) // check if using ordinal (both x86 && x64)
            {
                addrOld = (size_t)GetProcAddress(LoadLibraryA(lib_name), (char*)(orginThunk->u1.Ordinal & 0xFFFF));//通过序号?
                //printf("        [V] API %x at %x\n", orginThunk->u1.Ordinal, addr);
                //fieldThunk->u1.Function = addr;    
                //DebugBreak();
                continue;
            }
            else
            {
                by_name = (PIMAGE_IMPORT_BY_NAME)(size_t(modulePtr) + orginThunk->u1.AddressOfData);
                func_name = (LPSTR)by_name->Name;
                addrOld = (size_t)GetProcAddress(LoadLibraryA(lib_name), func_name);
            }
            //printf("        [V] API %s at %x\n", func_name, addr);
            OutputDebugStringA("\r\n");
            OutputDebugStringA(func_name);
            //HOOK            
            if (strcmpi(func_name, "ExitWindowsEx") == 0)
            {                
                //DebugBreak();                                                
                DWORD dOldProtect = 0;
                size_t* pFuncAddr = (size_t*)&fieldThunk->u1.Function;
                if (VirtualProtect(pFuncAddr, sizeof(size_t), PAGE_EXECUTE_READWRITE, &dOldProtect))
                {
                    fieldThunk->u1.Function = (size_t)MyExitWindowsEx;
                    VirtualProtect(pFuncAddr, sizeof(size_t), dOldProtect, &dOldProtect);
                    _OldExitWindowsEx = (FuncExitWindowsEx)addrExitWindowsEx;
                    bFound = true;
                    return bFound;
                }                
            }
        }                
    }
    return true;    
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {        
        //DebugBreak(); 
        HMODULE exeModule = GetModuleHandle(NULL);        
        FixIATHook(exeModule);
        FixDelayIATHook(exeModule);                
        break;
    }        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

    return TRUE;
}
}
