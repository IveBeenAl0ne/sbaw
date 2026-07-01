#pragma once

#include "UIScene.h"
#include "UIControl_Button.h"
#include "UIControl_CheckBox.h"

class UIScene_KeyboardSettings : public UIScene
{
private:
    enum EControls {
        eControl_Button0=0,
		eControl_Button1,
		eControl_Button2,
		eControl_Button3,
		eControl_Button4,
		eControl_Button5,
		eControl_Button6,
		eControl_Button7,
		eControl_isWaiting,
        eControl_SwapActionUse
    };
    
    UIControl_Button m_buttonLayouts[8];
    UIControl_Button m_isWaiting;
    UIControl_CheckBox m_checkboxSwapActionUse;

	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_checkboxSwapActionUse, "SwapActionUse")
		UI_MAP_ELEMENT( m_buttonLayouts[0], "Button1")
		UI_MAP_ELEMENT( m_buttonLayouts[1], "Button2")
		UI_MAP_ELEMENT( m_buttonLayouts[2], "Button3")
		UI_MAP_ELEMENT( m_buttonLayouts[3], "Button4")
		UI_MAP_ELEMENT( m_buttonLayouts[4], "Button5")
		UI_MAP_ELEMENT( m_buttonLayouts[5], "Button6")
		UI_MAP_ELEMENT( m_buttonLayouts[6], "Button7")
		UI_MAP_ELEMENT( m_buttonLayouts[7], "Button8")
		UI_MAP_ELEMENT( m_isWaiting, "IsWaiting")
	UI_END_MAP_ELEMENTS_AND_NAMES()
	
    bool m_bNotInGame;

    int m_iPad;

    bool m_waitingForKeypress;
    int m_controlToBind;
public:
	UIScene_KeyboardSettings(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_KeyboardSettings();
    
	virtual EUIScene getSceneType() { return eUIScene_KeyboardSettings;}

	virtual void tick();
	
	virtual void updateComponents();
	
	virtual void updateTooltips();
    
protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();
    
public:
	virtual void handleReload();
    
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	
	virtual void handleCheckboxToggled(F64 controlId, bool selected);
	// virtual void handlePress(F64 controlId, F64 childId);
	
protected:
    void handlePress(F64 controlId, F64 childId);

    void handleKeyRebind(int controlId);
    void registerNames();
};