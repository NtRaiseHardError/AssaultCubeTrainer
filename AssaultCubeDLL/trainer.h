#pragma once
#include <Windows.h>

#include "Globals.h"

#define GETGAMEBASEADDR() GetModuleHandle(NULL)

#define PLAYER_SELF_ENTITY_ADDRESS_OFFSET 0x10F4F4
#define PLAYER_LIST_ENTITY_ADDRESS_OFFSET 0x10F4F8
#define GETPLAYERSELFENTITY() ((PPLAYER_ENTITY)((*(ULONG_PTR *)((LPBYTE)GETGAMEBASEADDR() + PLAYER_SELF_ENTITY_ADDRESS_OFFSET))))
#define GETPLAYERLISTENTITY() ((PPLAYER_ENTITY_LIST)(*(ULONG_PTR *)((LPBYTE)GETGAMEBASEADDR() + PLAYER_LIST_ENTITY_ADDRESS_OFFSET)))

#define GODMODE_ADDRESS_OFFSET 0x29D1F
#define GODMODE_ADDRESS_RETURN_OFFSET 0x29D24
#define GETGODMODEADDRESS() ((LPBYTE)GETGAMEBASEADDR() + GODMODE_ADDRESS_OFFSET)

typedef struct _PLAYER_ENTITY {
	/*   0 */ PVOID Vtable;
	/*   4 */ FLOAT MapX;
	/*   8 */ FLOAT MapY;
	/*   C */ FLOAT MapZ;			// View only.
	/*  10 */ PVOID Unk_0[6];
	/*  28 */ FLOAT CoordXDelta;	// Change in X pos value.
	/*  2C */ FLOAT CoordYDelta;	// Change in Y pos value.
	/*  30 */ FLOAT CoordZDelta;	// Change in Z pos value.
	/*  34 */ FLOAT CoordX;			// Real X pos value.
	/*  38 */ FLOAT CoordY;			// Real Y pos value.
	/*  3C */ FLOAT CoordZ;			// Real Z pos value; Z = Map height - View height (0.5 - 4.5 = -4)
	/*  40 */ FLOAT CrosshairX;		// 0 -> 360.
	/*  44 */ FLOAT CrosshairY;		// -90 -> 90.
	/*  48 */ PVOID Unk_1[5];
	/*  5C */ FLOAT ViewHeight;		// Relative to the bottom of the player (view only).
	/*  48 */ PVOID Unk_2[4];
	/*  6C */ ULONG Crouch;
	/*  70 */ PVOID Unk_3[3];
	/*  80 */ BYTE Unk_4[2];
	/*  82 */ BOOL PlayerState;		// 0: Alive; 1: Dead; 2: ?; 3: ?; 4: Edit; 5: Ghost/Spectator
	/*  83 */ BYTE Unk_5;			// 5.
	/*  70 */ PVOID Unk_6[29];
	/*  F8 */ ULONG Health;
	/*  FC */ ULONG Armor;
	/* 100 */ PVOID Unk_7[5];
	/* 114 */ ULONG RemainingSecondaryAmmo;
	/* 118 */ PVOID Unk_8[4];
	/* 128 */ ULONG RemainingPrimaryAmmo;
	/* 118 */ PVOID Unk_9[4];
	/* 13C */ ULONG CurrentSecondaryAmmo;
	/* 140 */ PVOID Unk_10[4];
	/* 150 */ ULONG CurrentPrimaryAmmo;
	/* 154 */ PVOID Unk_11[52];
	/* 224 */ BYTE Unk_12;
	/* 225 */ LPSTR Name;
	//
	// TODO: Add fly hacks member.
	//
} PLAYER_ENTITY, *PPLAYER_ENTITY;

typedef struct _PLAYER_ENTITY_LIST {
	PVOID Reserved;		// Zero.
	PLAYER_ENTITY Players[32];
} PLAYER_ENTITY_LIST, *PPLAYER_ENTITY_LIST;

typedef struct _TRAINER_CONTEXT {
	PPLAYER_ENTITY PlayerSelf;
	PPLAYER_ENTITY PlayerList;
} TRAINER_CONTEXT, *PTRAINER_CONTEXT;

TRAINER_CONTEXT g_TrainerCtx;

VOID
TrainerMain(

);