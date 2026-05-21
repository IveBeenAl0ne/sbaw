#include "stdafx.h"
#include "../App_enums.h"
#include "UI.h"
#include "UIScene_SettingsUIMenu.h"

int UIScene_SettingsUIMenu::m_iControlTypeSettingA[6]=
{
	IDS_CONTROLTYPE_KBM,
	IDS_CONTROLTYPE_XBOXONE,
	IDS_CONTROLTYPE_XBOX360,
	// IDS_CONTROLTYPE_VITA,
	IDS_CONTROLTYPE_PLAYSTATION3,
	IDS_CONTROLTYPE_PLAYSTATION4,
	IDS_CONTROLTYPE_WIIU,
	// IDS_CONTROLTYPE_SWITCH,
};

UIScene_SettingsUIMenu::UIScene_SettingsUIMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();
	m_bControlTypeChanged = false;

	m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	m_checkboxDisplayHUD.init(app.GetString(IDS_CHECKBOX_DISPLAY_HUD),eControl_DisplayHUD,(app.GetGameSettings(m_iPad,eGameSetting_DisplayHUD)!=0));
	m_checkboxDisplayHand.init(app.GetString(IDS_CHECKBOX_DISPLAY_HAND),eControl_DisplayHand,(app.GetGameSettings(m_iPad,eGameSetting_DisplayHand)!=0));

	m_checkboxShowTooltips.init(IDS_IN_GAME_TOOLTIPS,eControl_ShowTooltips,(app.GetGameSettings(m_iPad,eGameSetting_Tooltips)!=0));
	m_checkboxDisplayAnimatedCharacter.init(app.GetString(IDS_CHECKBOX_ANIMATED_CHARACTER),eControl_DisplayAnimatedCharacter,(app.GetGameSettings(m_iPad,eGameSetting_AnimatedCharacter)!=0));
	
	m_checkboxInGameGamertags.init(IDS_IN_GAME_GAMERTAGS,eControl_InGameGamertags,(app.GetGameSettings(m_iPad,eGameSetting_GamertagsVisible)!=0));
	m_checkboxShowSplitscreenGamertags.init(app.GetString(IDS_CHECKBOX_DISPLAY_SPLITSCREENGAMERTAGS),eControl_ShowSplitscreenGamertags,(app.GetGameSettings(m_iPad,eGameSetting_DisplaySplitscreenGamertags)!=0));
	m_checkboxShowClassicCrafting.init(app.GetString(IDS_CHECKBOX_CLASSICCRAFTING), eControl_ShowClassicCrafting, (app.GetGameSettings(m_iPad, eGameSetting_ClassicCrafting) != 0));
	// label is hardcoded for now (no IDS_* yet)

	m_checkboxHideLoadCreateJoinSaveSizeBar.init(L"Hide world disk space bar", eControl_HideSaveSizeBar, (app.GetGameSettings(m_iPad, eGameSetting_HideSaveSizeBar) != 0));

	WCHAR TempString[256];

	swprintf( TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_INTERFACEOPACITY ),app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));	
	m_sliderInterfaceOpacity.init(TempString,eControl_InterfaceOpacity,0,100,app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));
	
	swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INMENU ),app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu));	
	m_sliderSensitivityInMenu.init(TempString,eControl_SensitivityInMenu,0,200,app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu));

	swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZE ),app.GetGameSettings(m_iPad,eGameSetting_UISize)+1);	
	m_sliderUISize.init(TempString,eControl_UISize,1,3,app.GetGameSettings(m_iPad,eGameSetting_UISize)+1);

	swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZESPLITSCREEN ),app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1);	
	m_sliderUISizeSplitscreen.init(TempString,eControl_UISizeSplitscreen,1,3,app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1);

	swprintf( (WCHAR *)TempString, 256, L"%ls: %ls", app.GetString( IDS_SLIDER_CONTROLTYPE ),app.GetString(m_iControlTypeSettingA[app.GetGameSettings(m_iPad,eGameSetting_ControlType)]));	
	m_sliderControlType.init(TempString,eControl_ControlType,0,5,app.GetGameSettings(m_iPad,eGameSetting_ControlType));

	doHorizontalResizeCheck();

	bool bInGame=(Minecraft::GetInstance()->level!=nullptr);
	bool bPrimaryPlayer = ProfileManager.GetPrimaryPad()==m_iPad;
	bool bRemoveInGameGamertags=false;

	if(!bPrimaryPlayer)
	{
		bRemoveInGameGamertags=true;
	}

	// if we're not in the game, we need to use basescene 0 
	if(bInGame)
	{
		// If the game has started, then you need to be the host to change the in-game gamertags
		if(!bPrimaryPlayer)
		{	
			// hide things we don't want the splitscreen player changing
			removeControl(&m_checkboxShowSplitscreenGamertags, true);
		}
		removeControl(&m_sliderControlType, true);
	}

	if(bRemoveInGameGamertags)
	{
		removeControl(&m_checkboxInGameGamertags, true);
	}

	if(app.GetLocalPlayerCount()>1)
	{
#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad);
#endif
	}
}

void UIScene_SettingsUIMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsUIMenu::updateComponents()
{
	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);
	if(bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);

		if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
		else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);

	}
}

UIScene_SettingsUIMenu::~UIScene_SettingsUIMenu()
{
}

wstring UIScene_SettingsUIMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsUIMenuSplit";
	}
	else
	{
		return L"SettingsUIMenu";
	}
}

void UIScene_SettingsUIMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			const bool reloadControlTypeSkin = m_bControlTypeChanged;

			// check the checkboxes
			app.SetGameSettings(m_iPad,eGameSetting_DisplayHUD,m_checkboxDisplayHUD.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_DisplayHand,m_checkboxDisplayHand.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_Tooltips,m_checkboxShowTooltips.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_AnimatedCharacter,m_checkboxDisplayAnimatedCharacter.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_GamertagsVisible,m_checkboxInGameGamertags.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_DisplaySplitscreenGamertags,m_checkboxShowSplitscreenGamertags.IsChecked()?1:0);
			app.SetGameSettings(m_iPad, eGameSetting_ClassicCrafting, m_checkboxShowClassicCrafting.IsChecked() ? 1 : 0);
			app.SetGameSettings(m_iPad, eGameSetting_HideSaveSizeBar, m_checkboxHideLoadCreateJoinSaveSizeBar.IsChecked() ? 1 : 0);

			navigateBack();
			if(reloadControlTypeSkin)
			{
				ui.ReloadSkin();
			}
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
		sendInputToMovie(key, repeat, pressed, released);
		break;
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_SettingsUIMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	int value = static_cast<int>(currentValue);
	switch(static_cast<int>(sliderId))
	{
	case eControl_InterfaceOpacity:
		m_sliderInterfaceOpacity.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_InterfaceOpacity,value);
		swprintf( TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_INTERFACEOPACITY ),value);	
		m_sliderInterfaceOpacity.setLabel(TempString);

		break;
	case eControl_SensitivityInMenu:
		m_sliderSensitivityInMenu.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INMENU ),value);	
		m_sliderSensitivityInMenu.setLabel(TempString);

		break;
	case eControl_UISize:
		m_sliderUISize.handleSliderMove(value);

		swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZE ),value);		
		m_sliderUISize.setLabel(TempString);

		// is this different from the current value?
		if(value != app.GetGameSettings(m_iPad,eGameSetting_UISize)+1)
		{
			app.SetGameSettings(m_iPad,eGameSetting_UISize,value-1);
			// Apply the changes to the selected text position
			ui.UpdateSelectedItemPos(m_iPad);
		}

		break;
	case eControl_UISizeSplitscreen:
		m_sliderUISizeSplitscreen.handleSliderMove(value);

		swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZESPLITSCREEN ),value);			
		m_sliderUISizeSplitscreen.setLabel(TempString);

		if(value != app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1)
		{
			// slider is 1 to 3
			app.SetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen,value-1);
			// Apply the changes to the selected text position
			ui.UpdateSelectedItemPos(m_iPad);
		}

		break;
	case eControl_ControlType:
		m_sliderControlType.handleSliderMove(value);
		app.SetGameSettings(m_iPad,eGameSetting_ControlType,value);
		m_bControlTypeChanged = true;

		swprintf( (WCHAR *)TempString, 256, L"%ls: %ls", app.GetString( IDS_SLIDER_CONTROLTYPE ),app.GetString(m_iControlTypeSettingA[value]));
		m_sliderControlType.setLabel(TempString);
		
		break;
	}
}
