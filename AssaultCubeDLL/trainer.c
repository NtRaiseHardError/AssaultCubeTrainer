#include <stdio.h>
#include <Windows.h>

#include "trainer.h"
#include "hooks.h"

static
VOID
PrintPlayerStats(
	_In_ PTRAINER_CONTEXT g_TrainerCtx
)
{
	printf(
		"Displaying stats for %s:\n"
		"Health: %ul\n"
		"Armor: %ul\n"
		"Current ammo: %ul\n"
		"Remaining ammo: %ul\n"
		"Coord X, Y, Z: (%f, %f, %f)\n"
		"Crosshair X, Y: (%f, %f)\n",
		(LPCSTR)((LPBYTE)g_TrainerCtx->PlayerSelf + 0x225),
		g_TrainerCtx->PlayerSelf->Health,
		g_TrainerCtx->PlayerSelf->Armor,
		g_TrainerCtx->PlayerSelf->CurrentPrimaryAmmo,
		g_TrainerCtx->PlayerSelf->RemainingPrimaryAmmo,
		g_TrainerCtx->PlayerSelf->CoordX, g_TrainerCtx->PlayerSelf->CoordY, g_TrainerCtx->PlayerSelf->CoordZ,
		g_TrainerCtx->PlayerSelf->CrosshairX, g_TrainerCtx->PlayerSelf->CrosshairY
	);
}

static
VOID
FlyHacks(
	_In_ PTRAINER_CONTEXT g_TrainerCtx,
	_In_ BOOL Enabled
)
{
	if (Enabled) {
		*(LPBYTE)((LPBYTE)g_TrainerCtx->PlayerSelf + 0x338) = 5;
	} else {
		*(LPBYTE)((LPBYTE)g_TrainerCtx->PlayerSelf + 0x338) = 0;
	}
}

static
__declspec(naked)
VOID
GodModeHook(

)
{
	//
	// eax does not need to be preserved becase
	// it will receive edi.
	//
	// edi does not need to be preserved becase
	// it will be popped into after this code.
	//
	// esi does not need to be preserved because
	// it will be popped into after this code.
	//
	__asm {
			push 0
			call GetModuleHandleW
			push eax					// Store module base pointer.
			lea esi, [eax + PLAYER_SELF_ENTITY_ADDRESS_OFFSET]	// Get own player pointer.
			lea eax, [ebx - 0xF4]		// Resolve base of target player object.
			cmp eax, [esi]				// Get base of own player's object.
			jz done
			sub [ebx + 0x4], edi

		done:
			mov eax, edi				// From original game's code (overwritten by hook).
			pop esi						// Get module base pointer.
			add esi, GODMODE_ADDRESS_RETURN_OFFSET		// Resolve return address.
			jmp esi
	}
}

//static
BOOL
GodMode(
	_In_ BOOL Enabled
)
{
	return Enabled ? AddHook(GETGODMODEADDRESS(), GodModeHook)
		: RemoveHook(GETGODMODEADDRESS());
}

VOID
TrainerMain(

)
{
	InitHooks();

	//
	// Initialise trainer context.
	//
	ZeroMemory(&g_TrainerCtx, sizeof(TRAINER_CONTEXT));

	//
	// Discover players.
	//
	g_TrainerCtx.PlayerSelf = GETPLAYERSELFENTITY();
	g_TrainerCtx.PlayerList = GETPLAYERLISTENTITY()->Players;

	GodMode(TRUE);

	//
	// Maybe hook into game loop and update each new game?
	//
	while (WaitForSingleObject(GetCurrentProcess(), 1000) == WAIT_TIMEOUT) {
		PrintPlayerStats(&g_TrainerCtx);
		FlyHacks(&g_TrainerCtx, TRUE);
	}

	return;
}