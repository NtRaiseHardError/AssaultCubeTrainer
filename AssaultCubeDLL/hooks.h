#pragma once
#include <Windows.h>

#define DEFAULT_HOOK_LIST_SIZE 10
#define HOOK_SIZE 5

typedef struct _HOOK_CTX {
	PVOID TargetAddress;	// Address to hook.
	PVOID HookAddress;		// Address of the hook.
	BYTE OriginalInstructions[HOOK_SIZE];
} HOOK_CTX, *PHOOK_CTX;

//
// TODO: Change to linked list?
//
SIZE_T g_HookListSize;
SIZE_T g_AvailableSize;
PHOOK_CTX *g_HookList;

BOOL
InitHooks(

);

BOOL
AddHook(
	_In_ PVOID TargetAddress,
	_In_ PVOID HookAddress
);

BOOL
RemoveHook(
	_In_ PVOID TargetAddress
);