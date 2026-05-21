#pragma once

#include "UIScene.h"

class UIScene_SettingsUIMenu : public UIScene
{
protected:
	static int m_iControlTypeSettingA[6];
private:
	enum EControls
	{
		eControl_DisplayHUD,
		eControl_DisplayHand,
		eControl_SensitivityInMenu,
		eControl_InterfaceOpacity,
		eControl_ShowTooltips,
		eControl_DisplayAnimatedCharacter,
		eControl_InGameGamertags,
		eControl_ShowSplitscreenGamertags,
		eControl_ShowClassicCrafting,
		eControl_HideSaveSizeBar,
		eControl_UISize,
		eControl_UISizeSplitscreen,
		eControl_ControlType,
	};

	UIControl_CheckBox m_checkboxDisplayHUD, m_checkboxDisplayHand, m_checkboxDisplayAnimatedCharacter, m_checkboxShowSplitscreenGamertags, m_checkboxShowClassicCrafting, m_checkboxShowTooltips, m_checkboxInGameGamertags, m_checkboxHideLoadCreateJoinSaveSizeBar; // Checkboxes
	UIControl_Slider m_sliderInterfaceOpacity, m_sliderSensitivityInMenu, m_sliderUISize, m_sliderUISizeSplitscreen, m_sliderControlType; // Sliders
	bool m_bControlTypeChanged;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_checkboxDisplayHUD, "DisplayHUD")
		UI_MAP_ELEMENT( m_checkboxDisplayHand, "DisplayHand")
		UI_MAP_ELEMENT( m_sliderInterfaceOpacity, "InterfaceOpacity")

		UI_MAP_ELEMENT( m_checkboxShowTooltips, "ShowTooltips")
		UI_MAP_ELEMENT( m_checkboxDisplayAnimatedCharacter, "DisplayAnimatedCharacter")
		UI_MAP_ELEMENT( m_sliderSensitivityInMenu, "SensitivityInMenu")
		
		UI_MAP_ELEMENT( m_checkboxInGameGamertags, "InGameGamertags")
		UI_MAP_ELEMENT( m_checkboxShowSplitscreenGamertags, "ShowSplitscreenGamertags")
		UI_MAP_ELEMENT(m_checkboxShowClassicCrafting, "ShowClassicCrafting")
		UI_MAP_ELEMENT(m_checkboxHideLoadCreateJoinSaveSizeBar, "LoadCreateJoinSaveSizeBar")

		UI_MAP_ELEMENT( m_sliderUISize, "UISize")
		UI_MAP_ELEMENT( m_sliderUISizeSplitscreen, "UISizeSplitscreen")
		UI_MAP_ELEMENT( m_sliderControlType, "ControlType")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	bool m_bNotInGame;
public:
	UIScene_SettingsUIMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsUIMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsUIMenu;}
	
	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void handleSliderMove(F64 sliderId, F64 currentValue);
};
