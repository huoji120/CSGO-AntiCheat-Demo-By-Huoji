// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>



// TODO:  在此处引用程序需要的其他头文件
typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
}
UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	DWORD Length; // +0x00  
	bool Initialized; // +0x04  
	PVOID SsHandle; // +0x08  
	LIST_ENTRY InLoadOrderModuleList; // +0x0c  
	LIST_ENTRY InMemoryOrderModuleList; // +0x14  
	LIST_ENTRY InInitializationOrderModuleList;// +0x1c  
} PEB_LDR_DATA, *PPEB_LDR_DATA; // +0x24  

typedef struct _LDR_MODULE
{
	LIST_ENTRY          InLoadOrderModuleList;
	LIST_ENTRY          InMemoryOrderModuleList;
	LIST_ENTRY          InInitializationOrderModuleList;
	void*               BaseAddress;
	void*               EntryPoint;
	ULONG               SizeOfImage;
	UNICODE_STRING   FullDllName;
	UNICODE_STRING      BaseDllName;
	ULONG               Flags;
	SHORT               LoadCount;
	SHORT               TlsIndex;
	HANDLE              SectionHandle;
	ULONG               CheckSum;
	ULONG               TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

//需要隐藏的DLL句柄 