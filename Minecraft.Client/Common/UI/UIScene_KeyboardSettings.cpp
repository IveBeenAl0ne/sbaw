#include "stdafx.h"
#include "UI.h"
#include "UIScene_KeyboardSettings.h"
#include "../../Minecraft.h"

#include "../../Options.h"
#include "../../Windows64/KeyboardMouseInput.h"

#define castKeycode(key) std::basic_string<wchar_t>(1, static_cast<wchar_t>(key))

UIScene_KeyboardSettings::UIScene_KeyboardSettings(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	Minecraft *pMinecraft = Minecraft::GetInstance();
	
	m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	m_checkboxSwapActionUse.init(IDS_SWAP_AU,eControl_SwapActionUse,Minecraft::GetInstance()->options->swapActionUse);

	m_isWaiting.init(L"Press a key...", eControl_isWaiting);
	m_isWaiting.setVisible(false);

	registerNames();

	m_iPad = iPad;
}

UIScene_KeyboardSettings::~UIScene_KeyboardSettings()
{
}

wstring UIScene_KeyboardSettings::getMoviePath()
{
	return L"KeyboardSettings";
}

void UIScene_KeyboardSettings::updateComponents()
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

void UIScene_KeyboardSettings::handleReload()
{
    m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);
   
	m_checkboxSwapActionUse.init(IDS_SWAP_AU,eControl_SwapActionUse,Minecraft::GetInstance()->options->swapActionUse);
}

void UIScene_KeyboardSettings::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_KeyboardSettings::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			navigateBack();
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
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_KeyboardSettings::handlePress(F64 controlId, F64 childId)
{
	switch(static_cast<int>(controlId))
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
        handleKeyRebind(static_cast<int>(controlId));
        m_isWaiting.setVisible(true);
	    break;
	}
}

void UIScene_KeyboardSettings::handleKeyRebind(int controlId)
{
    switch (controlId) {
    case 0:
        m_controlToBind = 0;
        break;
    case 1:
        m_controlToBind = 1;
        break;
    case 2:
        m_controlToBind = 2;
        break;
    case 3:
        m_controlToBind = 3;
        break;
    case 4:
        m_controlToBind = 7;
        break;
    case 5:
        m_controlToBind = 8;
        break;
    case 6:
        m_controlToBind = 9;
        break;
    case 7:
        m_controlToBind = 11;
        break;
    }
    m_waitingForKeypress = true;
}

void UIScene_KeyboardSettings::registerNames()
{
    Minecraft *pMinecraft = Minecraft::GetInstance();
    
    m_buttonLayouts[0].init(L"Forward: " + castKeycode(pMinecraft->options->keyboardBindings[0]), eControl_Button0);
	m_buttonLayouts[1].init(L"Backward: " + castKeycode(pMinecraft->options->keyboardBindings[1]), eControl_Button1);
	m_buttonLayouts[2].init(L"Left: " + castKeycode(pMinecraft->options->keyboardBindings[2]), eControl_Button2);
	m_buttonLayouts[3].init(L"Right: " + castKeycode(pMinecraft->options->keyboardBindings[3]), eControl_Button3);
	m_buttonLayouts[4].init(L"Inventory: " + castKeycode(pMinecraft->options->keyboardBindings[7]), eControl_Button4);
	m_buttonLayouts[5].init(L"Drop: " + castKeycode(pMinecraft->options->keyboardBindings[8]), eControl_Button5);
	m_buttonLayouts[6].init(L"Crafting: " + castKeycode(pMinecraft->options->keyboardBindings[9]), eControl_Button6);
	m_buttonLayouts[7].init(L"Chat: " + castKeycode(pMinecraft->options->keyboardBindings[11]), eControl_Button7);
}

void UIScene_KeyboardSettings::tick()
{
    UIScene::tick();
    
    if(m_waitingForKeypress && m_controlToBind != -1)
    {
        wchar_t key;
        if(!g_KBMInput.ConsumeChar(key)) { return; }
        key = toupper(key);
        
        Minecraft::GetInstance()->options->keyboardBindings[m_controlToBind] = key;
        app.SetGameSettings(m_iPad, eGameSetting_KeyboardBinding, 0);
        
        registerNames();
        
        m_isWaiting.setVisible(false);
        
        m_controlToBind = -1;
        m_waitingForKeypress = false;
    }
}

void UIScene_KeyboardSettings::handleCheckboxToggled(F64 controlId, bool selected)
{
	switch(static_cast<int>(controlId))
	{
	case eControl_SwapActionUse:
        Minecraft::GetInstance()->options->swapActionUse = !Minecraft::GetInstance()->options->swapActionUse;
        app.SetGameSettings(m_iPad, eGameSetting_KeyboardBinding, 0);
		break;
	};
}

// void UIScene_KeyboardSettings::handlePress(F64 controlId, F64 childId)
// {
// 	switch(static_cast<int>(controlId))
// 	{
// 	}
// }