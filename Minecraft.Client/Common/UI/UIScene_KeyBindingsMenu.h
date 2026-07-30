#pragma once

#include "UIScene.h"
#include "UIControl_MultiList.h"

// Rebinding screen, on the same generic MultilistMenu layout as the settings menus.
class UIScene_KeyBindingsMenu : public UIScene
{
private:
	enum EControls
	{
		eControl_MultiList     = 0,
		eControl_ResetDefaults = 1,

		// Row id for action a is eControl_FirstAction + a.
		eControl_FirstAction   = 100,
	};

	UIControl_MultiList m_multiList;
	bool m_bNeedsMultiListPopulate;
	int  m_iCapturingAction; // -1 when idle
	bool m_bDirty;

	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
	UI_END_MAP_ELEMENTS_AND_NAMES()

	wstring BuildRowLabel(int action);

public:
	UIScene_KeyBindingsMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_KeyBindingsMenu();

	virtual EUIScene getSceneType() { return eUIScene_KeyBindingsMenu; }

	virtual void tick();

	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handlePress(F64 controlId, F64 childId);
	virtual void handleGainFocus(bool navBack);
};
