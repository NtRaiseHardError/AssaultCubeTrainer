#include <Windows.h>

#include "hooks.h"

static
BOOL
GrowHookList(

)
{
	PVOID NewList = NULL;

	if (g_HookList) {
		NewList = HeapReAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			g_HookList,
			g_HookListSize * 2
		);
		if (!NewList) {
			return FALSE;
		}

		g_HookList = NewList;
		g_AvailableSize += g_HookListSize;
		g_HookListSize *= 2;
	}

	return TRUE;
}

BOOL
InitHooks(

)
{
	g_HookList = HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(PHOOK_CTX) * DEFAULT_HOOK_LIST_SIZE
	);
	if (!g_HookList) {
		return FALSE;
	}

	for (ULONG i = 0; i < DEFAULT_HOOK_LIST_SIZE; i++) {
		g_HookList[i] = HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			sizeof(HOOK_CTX)
		);

		if (!g_HookList[i]) {
			goto fail;
		}
	}

	g_AvailableSize = DEFAULT_HOOK_LIST_SIZE;
	g_HookListSize = DEFAULT_HOOK_LIST_SIZE;

	return TRUE;

fail:
	if (g_HookList) {
		for (ULONG i = 0; i < DEFAULT_HOOK_LIST_SIZE; i++) {
			if (g_HookList[i]) {
				HeapFree(
					GetProcessHeap(),
					0,
					g_HookList[i]
				);
			}
		}

		HeapFree(
			GetProcessHeap(),
			0,
			g_HookList
		);
	}

	return FALSE;
}

BOOL
AddHook(
	_In_ PVOID TargetAddress,
	_In_ PVOID HookAddress
)
{
	BOOL ReturnSuccess = FALSE;
	BOOL Success;
	BYTE HookInstructions[HOOK_SIZE];
	PVOID JumpDistance = (PVOID)((ULONG_PTR)HookAddress - (ULONG_PTR)TargetAddress - HOOK_SIZE);

	//
	// Check if hook list needs to grow.
	//
	if (!g_AvailableSize) {
		Success = GrowHookList(g_HookList);
		if (!Success) {
			//
			// List can't grow and it's full.
			//
			return FALSE;
		}
	}

	//
	// Do hook.
	//
	for (SIZE_T i = 0; i < g_HookListSize; i++) {
		//
		// If the target address is already hooked, fail.
		//
		if (TargetAddress == g_HookList[i]->TargetAddress) {
			break;
		}

		//
		// If target address is NULL, insert new hook.
		//
		if (g_HookList[i]->TargetAddress == NULL) {
			g_HookList[i]->TargetAddress = TargetAddress;
			g_HookList[i]->HookAddress = HookAddress;

			//
			// Save original instructions in case they should
			// be restored.
			//
			memcpy(
				g_HookList[i]->OriginalInstructions,
				TargetAddress,
				HOOK_SIZE
			);

			//
			// Create new instructions to copy.
			//
			ZeroMemory(HookInstructions, HOOK_SIZE);

			HookInstructions[0] = 0xE9;	// JMP.
			HookInstructions[1] = (ULONG_PTR)JumpDistance & 0xFF;
			HookInstructions[2] = ((ULONG_PTR)JumpDistance >> 8) & 0xFF;
			HookInstructions[3] = ((ULONG_PTR)JumpDistance >> 16) & 0xFF;
			HookInstructions[4] = ((ULONG_PTR)JumpDistance >> 24) & 0xFF;

			//
			// Insert hook.
			//
			WriteProcessMemory(
				GetCurrentProcess(),
				TargetAddress,
				HookInstructions,
				HOOK_SIZE,
				NULL
			);


			g_AvailableSize--;
			ReturnSuccess = TRUE;
		}
	}

	return ReturnSuccess;
}

BOOL
RemoveHook(
	_In_ PVOID TargetAddress
)
{
	BOOL ReturnSuccess = FALSE;

	for (SIZE_T i = 0; i < g_HookListSize; i++) {
		if (g_HookList[i]->TargetAddress == TargetAddress) {
			//
			// Do unhook.
			//
			WriteProcessMemory(
				GetCurrentProcess(),
				TargetAddress,
				g_HookList[i]->OriginalInstructions,
				HOOK_SIZE,
				NULL
			);

			ZeroMemory(g_HookList[i], sizeof(HOOK_CTX));
			g_AvailableSize++;

			ReturnSuccess = TRUE;
		}
	}

	return ReturnSuccess;
}