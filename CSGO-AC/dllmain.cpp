// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <string>         // std::string
#include <psapi.h>
//����------------------------------------------------------------------------------------------------------------------------------------------
#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
using MsgFn = void(__cdecl*)(char const *, ...);
MsgFn PrintMessage = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");
template <typename T>
T* CaptureInterface(std::string strModule, std::string strInterface)
{
	typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule.c_str()), "CreateInterface");
	return CreateInterface(strInterface.c_str(), 0);
}
//�õ�VMT��Ĵ�С,��ʱû��ʲô��
DWORD GetVTCount(PDWORD pdwVMT) 
{
	DWORD dwIndex = 0;

	for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
	{
		if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
		{
			break;
		}
	}
	return dwIndex;
}

// learn_more
DWORD FindPattern(std::string moduleName, std::string pattern)
{

	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}

	return NULL;
}
// @https://bbs.pediy.com/thread-196402.htm
void Hide_Module(HMODULE hMod)
{
	PLIST_ENTRY Head, Cur;
	PPEB_LDR_DATA ldr;
	PLDR_MODULE ldm;
	__asm
	{
		mov eax, fs:[0x30]
		mov ecx, [eax + 0x0c] //Ldr    
		mov ldr, ecx
	}
	Head = &(ldr->InLoadOrderModuleList);
	Cur = Head->Flink;
	do
	{
		ldm = CONTAINING_RECORD(Cur, LDR_MODULE, InLoadOrderModuleList);
		if (hMod == ldm->BaseAddress)
		{
			ldm->InLoadOrderModuleList.Blink->Flink =
				ldm->InLoadOrderModuleList.Flink;
			ldm->InLoadOrderModuleList.Flink->Blink =
				ldm->InLoadOrderModuleList.Blink;
			ldm->InInitializationOrderModuleList.Blink->Flink =
				ldm->InInitializationOrderModuleList.Flink;
			ldm->InInitializationOrderModuleList.Flink->Blink =
				ldm->InInitializationOrderModuleList.Blink;
			ldm->InMemoryOrderModuleList.Blink->Flink =
				ldm->InMemoryOrderModuleList.Flink;
			ldm->InMemoryOrderModuleList.Flink->Blink =
				ldm->InMemoryOrderModuleList.Blink;
			break;
		}
		Cur = Cur->Flink;
	} while (Head != Cur);
}

//����------------------------------------------------------------------------------------------------------------------------------------------
//��ԴSDK����Ķ��� ��ʵҲֻ��һ��DWORD������ǹҵĻ�
class IVPanel;
class IVModelRender;
void AnTiCheatThread()
{
	PDWORD	oldVT_VPanel, oldVT_ModelRender;
	//��ҹ��ڻ�ͼ�Ĺ��ܵ�HOOK,�ȵõ�ԭ����,û�б�HOOK����!!!
	IVPanel* VPanel = CaptureInterface<IVPanel>("vgui2.dll", "VGUI_Panel009"); 
	//ModelRender ģ��Draw��
	IVModelRender* ModelRender = CaptureInterface<IVModelRender>("engine.dll", "VEngineModel016");
	//CM FSN PlayerSound.... �ȵ��Լ�ȥHOOK
	oldVT_VPanel = *(DWORD**)VPanel;
	oldVT_ModelRender = *(DWORD**)ModelRender;
	PrintMessage("AntiCheat Has Been Install: \n VPanel: %d \n ModelRender: %d \n",(int)oldVT_VPanel,(int)oldVT_ModelRender);
	//һ��С���� @5EWIN
	Hide_Module(GetModuleHandleA("d3d9.dll"));
	Hide_Module(GetModuleHandleA("d3dx9_43.dll"));
	Hide_Module(GetModuleHandleA("shaderapidx9.dll"));
	static bool Cheating = false;
	while (!Cheating)
	{
		//XD HAHAHAHAHAHAHAHAHA
		PDWORD VPanel = *(DWORD**)CaptureInterface<IVPanel>("vgui2.dll", "VGUI_Panel009");
		PDWORD ModelRend =  *(DWORD**)CaptureInterface<IVModelRender>("engine.dll", "VEngineModel016");
		if (oldVT_VPanel != VPanel)
			PrintMessage("\n \x02[!!!]VGUI Detectioned VMT Hook! [!!!] \n"),
			Cheating = true;
		if (ModelRend != oldVT_ModelRender)
			PrintMessage("\n \x02[!!!]ModelRender Detectioned VMT Hook! [!!!] \n"),
			Cheating = true;
		// Lazy :D
		Sleep(1000);
	}
	/*
	�������ֶ�:
	���ڸ��Ǽ�� -һ�����������С���ú�CSGO�Ĵ��ڵĴ�Сһ�� ������D3D����(GDIҲҪ���) �����ǽ��̴���CSGO�� -> һ������ҵĴ��ڳ��򸲸�
	ManualMap��� -B5������,���ܲ����� ����B5��������(�ǳ��������ֶ� ���ǻ�����һ������).
	*/
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AnTiCheatThread, NULL, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

