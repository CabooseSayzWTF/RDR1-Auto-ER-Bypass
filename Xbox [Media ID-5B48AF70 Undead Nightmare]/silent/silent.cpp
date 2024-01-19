#include "stdafx.h"
int StoreGameVersion;
BOOL bbInitialized = FALSE;
DWORD (__cdecl *XamGetCurrentTitleID)() = (DWORD (__cdecl *)())ResolveFunction("xam.xex", 0x1CF);
void Reset()
{
	bbInitialized = FALSE;
}
void xui(PLDR_DATA_TABLE_ENTRY ModuleHandle){
	for (;;)
	{
		if (XamGetCurrentTitleId() == 0x5454082B) // RDR Undead RF Non GOTY
		{
			if (bbInitialized == FALSE)
			{
				Sleep(1000);//small wait to grab game version
				StoreGameVersion = *(int*)0x820184CC;
				if (StoreGameVersion == 10)//10
				{
					Sleep(4000);
					*(int*)0x82642838 = 0x4E800020;
					//*(int*)0x826429A0 = 0x4E800020;
					NotifyPopup(L"ER Bypassed By CabooseSayzWTF!");
					bbInitialized = TRUE;
				}
				else
				{
					NotifyPopup2(L"You Are Not On a Compatible Game Version");
					bbInitialized = TRUE;
				}
			}
		}
		else
		{
			Reset();
		};
		Sleep(100);
	}

}
BOOL WINAPI DllMain(HANDLE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH://Run Thread 
		HANDLE hThread; DWORD hThreadID;
		ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)xui, NULL, 0x2);
		XSetThreadProcessor(hThread, 4);
		ResumeThread(hThread);
		CloseHandle(hThread);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}