#pragma once

#include "InputBindings.h"

// Input entry point for game and UI code. On Windows64 the keyboard/mouse binding drives
// the action and the pad is OR-ed in as a secondary source; elsewhere it forwards straight
// to InputManager. The IsMouseGrabbed/IsKBMActive/primary-pad guards live in here.
namespace PCInput
{
	void Init();

	bool ActionPressed(int iPad, int action);
	bool ActionDown(int iPad, int action);
	bool ActionReleased(int iPad, int action);

	float GetMoveX(int iPad);
	float GetMoveY(int iPad);
	void  GetLookDelta(int iPad, float &tx, float &ty);

	// Peeks the wheel, does not consume it. Callers own consumption so a scroll the UI
	// does not claim can still reach gameplay.
	int GetScrollDelta(int iPad);

	int GetHotbarSlotPressed(int iPad); // -1, or 0..8

	bool IsKBMDriving(int iPad);

	// While capturing, KBM actions all read as unpressed, so picking a key does not also
	// fire whatever it was bound to.
	void BeginCapture();
	bool PollCapture(Binding &out);
	void CancelCapture();
	bool IsCapturing();
}
