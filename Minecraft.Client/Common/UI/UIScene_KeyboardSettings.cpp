#include "stdafx.h"
#include "UI.h"
#include "UIScene_KeyboardSettings.h"
#include "../../Minecraft.h"

UIScene_KeyboardSettings::UIScene_KeyboardSettings(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();
}

UIScene_KeyboardSettings::~UIScene_KeyboardSettings()
{
}

wstring UIScene_KeyboardSettings::getMoviePath()
{
	return L"KeyboardSettings";
}

void UIScene_KeyboardSettings::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed && !repeat)
		{
			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
	//CD - Added for audio
	if(pressed)
	{
		ui.PlayUISFX(eSFX_Press);
	}

	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}