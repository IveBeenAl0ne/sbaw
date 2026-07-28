#include "stdafx.h"
#include "PCInput.h"
#include "../../Minecraft.h"

#ifdef _WINDOWS64
#include "InputBindingsStore.h"
#include "../UI/UI.h" // ui.GetMenuDisplayed, for the menu-suppression guard
#include "../../Windows64/KeyboardMouseInput.h"
#endif

namespace
{
	// InputManager.Initialise was given MINECRAFT_ACTION_MAX as its action count.
	bool HasPadMapping(int action)
	{
		return action >= 0 && action < MINECRAFT_ACTION_MAX;
	}

#ifdef _WINDOWS64
	bool s_capturing = false;

	bool IsPrimaryPad(int iPad)
	{
		return iPad == ProfileManager.GetPrimaryPad();
	}

	// Peek, never GetMouseWheel() - that consumes, and actions get queried many times a
	// frame. Call sites own consumption.
	bool WheelMatches(int code)
	{
		const int wheel = g_KBMInput.PeekMouseWheel();
		return (code > 0 && wheel > 0) || (code < 0 && wheel < 0);
	}

	bool BindingDown(const Binding &b)
	{
		switch (b.src)
		{
		case eBind_Key:         return g_KBMInput.IsKeyDown(b.code);
		case eBind_MouseButton: return g_KBMInput.IsMouseButtonDown(b.code);
		case eBind_MouseWheel:  return WheelMatches(b.code); // momentary: down == pressed
		default:                return false;
		}
	}

	bool BindingPressed(const Binding &b)
	{
		switch (b.src)
		{
		case eBind_Key:         return g_KBMInput.IsKeyPressed(b.code);
		case eBind_MouseButton: return g_KBMInput.IsMouseButtonPressed(b.code);
		case eBind_MouseWheel:  return WheelMatches(b.code);
		default:                return false;
		}
	}

	bool BindingReleased(const Binding &b)
	{
		switch (b.src)
		{
		case eBind_Key:         return g_KBMInput.IsKeyReleased(b.code);
		case eBind_MouseButton: return g_KBMInput.IsMouseButtonReleased(b.code);
		// A wheel tick has no release edge. Synthesising one would fire the action twice.
		case eBind_MouseWheel:  return false;
		default:                return false;
		}
	}

	bool KBMOwnsAction(int iPad, int action)
	{
		// Swallowed while binding a key, so picking one does not also fire whatever it was
		// bound to. PollCapture skips Escape for the same reason.
		if (s_capturing && action != ACTION_MENU_CANCEL)
			return false;

		if (!IsPrimaryPad(iPad) || !g_KBMInput.IsKBMActive())
			return false;

		if (InputBindings::IsMenuAction(action))
			return true; // menus run with the cursor free, so no grab requirement

		// CInput returns 0 for gameplay actions while a menu is up, so a pad cannot drive
		// the world through an open container. Mirror it.
		if (ui.GetMenuDisplayed(iPad))
			return false;

		return g_KBMInput.IsMouseGrabbed();
	}

	bool KBMActionDown(int iPad, int action)
	{
		if (!KBMOwnsAction(iPad, action))
			return false;

		for (int s = 0; s < eBindSlot_Count; ++s)
		{
			if (BindingDown(InputBindings::Get(action, s)))
				return true;
		}
		return false;
	}

	bool KBMActionPressed(int iPad, int action)
	{
		if (!KBMOwnsAction(iPad, action))
			return false;

		for (int s = 0; s < eBindSlot_Count; ++s)
		{
			if (BindingPressed(InputBindings::Get(action, s)))
				return true;
		}
		return false;
	}

	bool KBMActionReleased(int iPad, int action)
	{
		if (!KBMOwnsAction(iPad, action))
			return false;

		for (int s = 0; s < eBindSlot_Count; ++s)
		{
			if (BindingReleased(InputBindings::Get(action, s)))
				return true;
		}
		return false;
	}
#endif // _WINDOWS64
}

void PCInput::Init()
{
#ifdef _WINDOWS64
	InputBindingsStore::Load();
#endif
}

bool PCInput::ActionDown(int iPad, int action)
{
	const bool pad = HasPadMapping(action) && InputManager.ButtonDown(iPad, action);
#ifdef _WINDOWS64
	return pad || KBMActionDown(iPad, action);
#else
	return pad;
#endif
}

bool PCInput::ActionPressed(int iPad, int action)
{
	const bool pad = HasPadMapping(action) && InputManager.ButtonPressed(iPad, action);
#ifdef _WINDOWS64
	return pad || KBMActionPressed(iPad, action);
#else
	return pad;
#endif
}

bool PCInput::ActionReleased(int iPad, int action)
{
	const bool pad = HasPadMapping(action) && InputManager.ButtonReleased(iPad, action);
#ifdef _WINDOWS64
	return pad || KBMActionReleased(iPad, action);
#else
	return pad;
#endif
}

bool PCInput::IsKBMDriving(int iPad)
{
#ifdef _WINDOWS64
	return !s_capturing
		&& IsPrimaryPad(iPad)
		&& g_KBMInput.IsKBMActive()
		&& g_KBMInput.IsMouseGrabbed();
#else
	(void)iPad;
	return false;
#endif
}

float PCInput::GetMoveX(int iPad)
{
#ifdef _WINDOWS64
	if (!IsKBMDriving(iPad))
		return 0.0f;

	float x = 0.0f;
	// 4J-PB: minecraft movement seems to be the wrong way round, so left is positive.
	if (KBMActionDown(iPad, MINECRAFT_ACTION_LEFT))  x += 1.0f;
	if (KBMActionDown(iPad, MINECRAFT_ACTION_RIGHT)) x -= 1.0f;
	return x;
#else
	(void)iPad;
	return 0.0f;
#endif
}

float PCInput::GetMoveY(int iPad)
{
#ifdef _WINDOWS64
	if (!IsKBMDriving(iPad))
		return 0.0f;

	float y = 0.0f;
	if (KBMActionDown(iPad, MINECRAFT_ACTION_FORWARD))  y += 1.0f;
	if (KBMActionDown(iPad, MINECRAFT_ACTION_BACKWARD)) y -= 1.0f;
	return y;
#else
	(void)iPad;
	return 0.0f;
#endif
}

void PCInput::GetLookDelta(int iPad, float &tx, float &ty)
{
	tx = 0.0f;
	ty = 0.0f;

#ifdef _WINDOWS64
	if (!IsKBMDriving(iPad))
		return;

	const float sensitivity =
		static_cast<float>(app.GetGameSettings(iPad, eGameSetting_Sensitivity_InGame)) / 100.0f;
	const float mouseLookScale = 5.0f;
	const float scale = sensitivity * mouseLookScale;

	tx =  static_cast<float>(g_KBMInput.GetMouseDeltaX()) * scale;
	ty = -static_cast<float>(g_KBMInput.GetMouseDeltaY()) * scale;

	// 4J: WESTY : Invert look Y if required.
	if (app.GetGameSettings(iPad, eGameSetting_ControlInvertLook))
		ty = -ty;
#else
	(void)iPad;
#endif
}

int PCInput::GetScrollDelta(int iPad)
{
#ifdef _WINDOWS64
	if (KBMActionDown(iPad, MINECRAFT_ACTION_LEFT_SCROLL))  return  1;
	if (KBMActionDown(iPad, MINECRAFT_ACTION_RIGHT_SCROLL)) return -1;
#else
	(void)iPad;
#endif
	return 0;
}

int PCInput::GetHotbarSlotPressed(int iPad)
{
#ifdef _WINDOWS64
	for (int slot = 0; slot < 9; ++slot)
	{
		if (ActionPressed(iPad, PC_ACTION_HOTBAR_1 + slot))
			return slot;
	}
#else
	(void)iPad;
#endif
	return -1;
}

void PCInput::BeginCapture()
{
#ifdef _WINDOWS64
	s_capturing = true;
	g_KBMInput.ClearAllState(); // drop the Enter/click that opened the row
#endif
}

void PCInput::CancelCapture()
{
#ifdef _WINDOWS64
	s_capturing = false;
#endif
}

bool PCInput::IsCapturing()
{
#ifdef _WINDOWS64
	return s_capturing;
#else
	return false;
#endif
}

bool PCInput::PollCapture(Binding &out)
{
#ifdef _WINDOWS64
	if (!s_capturing)
		return false;

	// From 7: VK_LBUTTON(1)..VK_XBUTTON2(6) are mouse codes WM_KEYDOWN never delivers.
	for (int vk = 7; vk < KeyboardMouseInput::MAX_KEYS; ++vk)
	{
		if (vk == VK_ESCAPE)
			continue; // Escape cancels; the scene handles it.

		// The message pump only ever stores the L/R codes, but IsKeyPressed synthesises
		// the generic ones by folding L||R. Skip them or a left-shift press captures as
		// VK_SHIFT, since 16/17/18 sort ahead of 160-165.
		if (vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU)
			continue;

		if (g_KBMInput.IsKeyPressed(vk))
		{
			out = Binding(eBind_Key, vk);
			s_capturing = false;
			return true;
		}
	}

	for (int mb = 0; mb < KeyboardMouseInput::MAX_MOUSE_BUTTONS; ++mb)
	{
		if (g_KBMInput.IsMouseButtonPressed(mb))
		{
			out = Binding(eBind_MouseButton, mb);
			s_capturing = false;
			return true;
		}
	}

	const int wheel = g_KBMInput.PeekMouseWheel();
	if (wheel != 0)
	{
		g_KBMInput.ConsumeMouseWheel();
		out = Binding(eBind_MouseWheel, wheel > 0 ? 1 : -1);
		s_capturing = false;
		return true;
	}
#else
	(void)out;
#endif
	return false;
}
