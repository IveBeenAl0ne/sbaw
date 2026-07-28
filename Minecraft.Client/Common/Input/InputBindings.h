#pragma once

#include "../App_enums.h"
#include <string>

// PC-only actions, continuing EControllerActions. Values run 52..67, so they overflow
// (1LL << action) in ullButtonsPressed and sit past InputManager's MINECRAFT_ACTION_MAX.
// Only ever query these through PCInput.
enum EPCActions
{
	PC_ACTION_HOTBAR_1 = MINECRAFT_ACTION_SCREENSHOT + 1,
	PC_ACTION_HOTBAR_2,
	PC_ACTION_HOTBAR_3,
	PC_ACTION_HOTBAR_4,
	PC_ACTION_HOTBAR_5,
	PC_ACTION_HOTBAR_6,
	PC_ACTION_HOTBAR_7,
	PC_ACTION_HOTBAR_8,
	PC_ACTION_HOTBAR_9,
	PC_ACTION_SPRINT,
	PC_ACTION_CHAT,
	PC_ACTION_TOGGLE_HUD,
	PC_ACTION_FULLSCREEN,
	PC_ACTION_DEBUG_INFO,
	PC_ACTION_DEBUG_CONSOLE,
	PC_ACTION_HOST_SETTINGS,

	PC_ACTION_MAX
};

enum EBindSource
{
	eBind_None = 0,
	eBind_Key,          // code = Win32 VK code
	eBind_MouseButton,  // code = KeyboardMouseInput::MOUSE_*
	eBind_MouseWheel    // code = +1 wheel up, -1 wheel down
};

struct Binding
{
	EBindSource src;
	int         code;

	Binding() : src(eBind_None), code(0) {}
	Binding(EBindSource s, int c) : src(s), code(c) {}

	bool IsBound() const { return src != eBind_None; }
	bool operator==(const Binding &o) const { return src == o.src && code == o.code; }
	bool operator!=(const Binding &o) const { return !(*this == o); }
};

enum EBindSlot
{
	eBindSlot_Primary = 0,
	eBindSlot_Secondary,
	eBindSlot_Count
};

namespace InputBindings
{
	void ResetToDefaults();

	const Binding &Get(int action, int slot);
	void           Set(int action, int slot, const Binding &b);
	void           Clear(int action, int slot);

	// Conflicting action in forAction's own context, or -1. Menu and gameplay are searched
	// separately because the defaults overlap: Tab is menu_y and host settings, Escape is
	// menu_cancel and pause.
	int  FindActionBoundTo(const Binding &b, int forAction);
	bool IsMenuAction(int action);

	std::wstring GetDisplayName(int action, int slot);
	std::wstring GetActionLabel(int action);

	// Stable JSON keys. Never localised, never renamed once shipped.
	const char *GetActionName(int action);
	int         GetActionByName(const char *name);

	const int *GetBindableActions(int &countOut);
}
