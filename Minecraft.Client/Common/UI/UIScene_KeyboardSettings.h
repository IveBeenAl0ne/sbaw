#pragma once

#include "UIScene.h"

// #define BUTTON_HAO_CHANGESKIN			0
// #define BUTTON_HAO_HOWTOPLAY			1
// #define BUTTON_HAO_CONTROLS				2
// #define BUTTON_HAO_SETTINGS				3
// #define BUTTON_HAO_CREDITS				4
// #define BUTTON_HAO_REINSTALL			5
// #define BUTTON_HAO_DEBUG				6
// #define BUTTON_HAO_KEYBOARD				7
#define BUTTONS_HAO_MAX			        0// + 1

class UIScene_KeyboardSettings : public UIScene
{
public:
	UIScene_KeyboardSettings(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_KeyboardSettings();
    
	virtual EUIScene getSceneType() { return eUIScene_KeyboardSettings;}
	
	virtual void updateTooltips();
	virtual void updateComponents();
    
protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();
    
public:
	virtual void handleReload();
    
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
    
protected:
	void handlePress(F64 controlId, F64 childId);
};