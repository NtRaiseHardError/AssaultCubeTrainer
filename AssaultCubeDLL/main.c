#include <stdio.h>
#include <Windows.h>

#include "trainer.h"
#include "Globals.h"

BOOL
APIENTRY
DllMain(
	_In_ HINSTANCE InstDll,
	_In_ DWORD Reason,
	_In_ LPVOID Reserved
)
{
	FILE* fp;

	if (Reason == DLL_PROCESS_ATTACH) {
		//
		// Create console for debugging.
		//
		AllocConsole();
		freopen_s(&fp, "CONOUT$", "w", stdout);

		//
		// Start trainer.
		//
		g_Thread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)TrainerMain,
			NULL,
			0,
			NULL
		);
		if (!g_Thread) {
			return FALSE;
		}
	}

	return TRUE;
}