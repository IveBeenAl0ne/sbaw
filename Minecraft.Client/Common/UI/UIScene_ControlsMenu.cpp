#include "stdafx.h"
#include "UI.h"
#include "UIScene_ControlsMenu.h"
#include "../../Minecraft.h"
#include "../../MultiPlayerLocalPlayer.h"

#include "../../Tesselator.h"
#include "../../GuiComponent.h"
#include "../../Options.h"
#include "../Consoles_App.h"

void fillRect(Tesselator *t, int x, int y, int w, int h, int r, int g, int b, int a)
{
    	t->begin();
    	t->color(r, g, b, a);
    	t->vertex(static_cast<float>(x), static_cast<float>(y), 0.0f);
    	t->vertex(static_cast<float>(x), static_cast<float>(y + h), 0.0f);
    	t->vertex(static_cast<float>(x + w), static_cast<float>(y + h), 0.0f);
    	t->vertex(static_cast<float>(x + w), static_cast<float>(y), 0.0f);
    	t->end();
};

void drawButton(GuiComponent gui, Font *font, int x, int y, wstring str, int color)
{
    gui.fill(x, y, x + 256, y + 32, color);
    glScalef(2, 2, 2);
	gui.drawCenteredString(font, str, x/2 + 64, y/2 + 6, 0xFFFFFFFF);
	glScalef(0.5, 0.5, 0.5);
}

void drawButton(GuiComponent gui, Font *font, int x, int y, wstring str) { drawButton(gui, font, x, y, str, 0xFF000000); }

void UIScene_ControlsMenu::render(S32 width, S32 height, C4JRender::eViewportType viewport)
{
    UIScene::render(width, height, viewport);
#ifndef _WINDOWS64
}
#endif
#ifdef _WINDOWS64
    Minecraft *pMinecraft = Minecraft::GetInstance();

    GuiComponent gui = GuiComponent();
    
    Tesselator *t = Tesselator::getInstance();

	ui.setupCustomDrawGameState();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -2000);

	// fillRect(t, width * 0.8, height * 0.5, 128, 64, 0, 0, 0, 255);
	// pMinecraft->font->drawShadow(L"test string", 2, 2 + 9 * 0, 0xffffff);

	drawButton(gui, pMinecraft->font, width * 0.8, height * 0.2, L"Keyboard Settings");

	if(m_keybindGuiOpen)
	{
        gui.fill(width * 0.2, height * 0.2, width * 0.8, height * 0.8, 0x7F000000);
        int color = 0xFF000000;
	    if(pMinecraft->options->swapActionUse)
		{
		    color = 0xFF777777;
		}
	    drawButton(gui, pMinecraft->font, width * 0.25, height * 0.25, L"Swap Action and Use", color);

		for(int i = 0; i < buttons_length; i++)
        {
            Button button = buttons[i];
            wchar_t key = pMinecraft->options->keyboardBindings[i];
            if(key == ' ')
            {
                drawButton(gui, pMinecraft->font, width * button.x, height * button.y, button.name + L" : Space");
            } else {
                drawButton(gui, pMinecraft->font, width * button.x, height * button.y, button.name + L" : " + key);
            }
        }

        if(m_waitingForKeypress)
        {
            drawButton(gui, pMinecraft->font, width * 0.5 - 128, height * 0.75, L"Press any key...", 0x00000000);
        }
	}

	m_width = width;
	m_height = height;
	
	ui.endCustomDrawGameState();
}

bool UIScene_ControlsMenu::handleMouseClick(F32 x, F32 y)
{   
    Minecraft *pMinecraft = Minecraft::GetInstance();
    
    if(x > m_width * 0.8 && x < m_width * 0.8 + 256 && y > m_height * 0.2 && y < m_height * 0.2 + 32)
    {
        m_keybindGuiOpen = !m_keybindGuiOpen;
        return true;
    }
    if(!m_keybindGuiOpen)
    {
        return UIScene::handleMouseClick(x, y);
    }
    if(x > m_width * 0.25 && x < m_width * 0.25 + 256 && y > m_height * 0.25 && y < m_height * 0.25 + 32)
    {
        pMinecraft->options->swapActionUse = !pMinecraft->options->swapActionUse;
        app.ActionGameSettings(m_iPad, eGameSetting_KeyboardBinding);
        return true;
    }
    for(int i = 0; i < buttons_length; i++)
    {
        Button button = buttons[i];
        if(x > m_width * button.x && x < m_width * button.x + 256 && y > m_height * button.y && y < m_height * button.y + 32 && !m_waitingForKeypress)
        {
            m_waitingForKeypress = true;
            m_idToBind = i;
            return true;
        }
    }
    
    // always consume to prevent Iggy re-entry on empty space (idk thats what another file said to do)
    return true;
}

void UIScene_ControlsMenu::tick()
{
    if(m_waitingForKeypress)
    {
        wchar_t ch;
        m_waitingForKeypress = !g_KBMInput.ConsumeChar(ch);
        
        Minecraft *pMinecraft = Minecraft::GetInstance();

        pMinecraft->options->keyboardBindings[m_idToBind] = toupper(ch);
        app.ActionGameSettings(m_iPad, eGameSetting_KeyboardBinding);
    }
    
    if(m_bLayoutChanged) PositionAllText(m_iPad);
    UIScene::tick();
}
#endif

UIScene_ControlsMenu::UIScene_ControlsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
    m_iPad = iPad;
    
    // all controls, autofills gui
    buttons[0].y = 0.3;
    buttons[0].name = L"Forward";
    buttons[1].y = 0.35;
    buttons[1].name = L"Backward";
    buttons[2].y = 0.4;
    buttons[2].name = L"Left";
    buttons[3].y = 0.45;
    buttons[3].name = L"Right";
    buttons[4].y = 0.5;
    buttons[4].name = L"Jump";
    buttons[5].y = 2;
    buttons[5].name = L"";
    buttons[6].y = 2;
    buttons[6].name = L"";
    buttons[7].y = 0.55;
    buttons[7].name = L"Inventory";
    buttons[8].y = 0.6;
    buttons[8].name = L"Drop";
    buttons[9].y = 0.65;
    buttons[9].name = L"Crafting";
    buttons[10].y = 2;
    buttons[10].name = L"";
    buttons[11].x = 0.4;
    buttons[11].y = 0.3;
    buttons[11].name = L"Chat";

	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	IggyDataValue result;
	IggyDataValue value[1];
	value[0].type = IGGY_DATATYPE_number;
#if defined(_XBOX) || defined(_WIN64)
	value[0].number = static_cast<F64>(0);
#elif defined(_DURANGO)
	value[0].number = (F64)1;
#elif defined(__PS3__)
	value[0].number = (F64)2;
#elif defined(__ORBIS__)
	value[0].number = (F64)3;
#elif defined(__PSVITA__)
	value[0].number = (F64)4;
#endif
	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcSetPlatform , 1 , value );

	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	if(bNotInGame)
	{
		LPWSTR layoutString = new wchar_t[ 128 ];
		swprintf( layoutString, 128, L"%ls", VER_PRODUCTVERSION_STR_W);	
		m_labelVersion.init(layoutString);
		delete [] layoutString;
	}
	// 4J-PB - stop the label showing in the in-game controls menu
	else
	{
		m_labelVersion.init(L" ");
	}
	m_bCreativeMode = !bNotInGame && Minecraft::GetInstance()->localplayers[m_iPad] && Minecraft::GetInstance()->localplayers[m_iPad]->abilities.mayfly;

#ifndef __PSVITA__
#ifdef __ORBIS__
	// no buttons to initialise if we're running this on PS4 remote play
	if(!InputManager.UsingRemoteVita())
#endif
	{
		m_buttonLayouts[0].init(L"1", eControl_Button0);
		m_buttonLayouts[1].init(L"2", eControl_Button1);
		m_buttonLayouts[2].init(L"3", eControl_Button2);
	}
#endif

	m_checkboxInvert.init(app.GetString(IDS_INVERT_LOOK), eControl_InvertLook, app.GetGameSettings(m_iPad,eGameSetting_ControlInvertLook));
	m_checkboxSouthpaw.init(app.GetString(IDS_SOUTHPAW), eControl_Southpaw, app.GetGameSettings(m_iPad,eGameSetting_ControlSouthPaw));

	m_iSchemeTextA[0]=IDS_CONTROLS_SCHEME0;
	m_iSchemeTextA[1]=IDS_CONTROLS_SCHEME1;
	m_iSchemeTextA[2]=IDS_CONTROLS_SCHEME2;

	int iSelected=app.GetGameSettings(m_iPad,eGameSetting_ControlScheme);

#ifndef __PSVITA__
	LPWSTR layoutString = new wchar_t[ 128 ];
	swprintf( layoutString, 128, L"%ls : %ls", app.GetString( IDS_CURRENT_LAYOUT ),app.GetString(m_iSchemeTextA[iSelected]));
#ifdef __ORBIS__
	if (!InputManager.UsingRemoteVita())
#endif
	{
		m_labelCurrentLayout.init(layoutString);
	}
#endif

	m_iCurrentNavigatedControlsLayout = iSelected;


#ifdef __ORBIS__
	// don't set controller layout if we're entering the PS4 remote play scene
	if(!InputManager.UsingRemoteVita())
#endif
	{
		IggyDataValue result;
		IggyDataValue value[1];
		value[0].type = IGGY_DATATYPE_number;
		value[0].number = static_cast<F64>(m_iCurrentNavigatedControlsLayout);
		IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcSetControllerLayout , 1 , value );
	}

#ifdef __ORBIS__
	// Set mapping to Vita mapping
	if (InputManager.UsingRemoteVita()) m_iCurrentNavigatedControlsLayout = 3;
#elif defined __PSVITA__
	// Set mapping to Vita mapping
	if (InputManager.IsVitaTV()) m_iCurrentNavigatedControlsLayout = 1;
#endif

	for(unsigned int i = 0; i < e_PadCOUNT; ++i)
	{
		m_labelsPad[i].init(L"");
		m_controlLines[i].setVisible(false);
	}
	m_bLayoutChanged = false;


	PositionAllText(m_iPad);
}

wstring UIScene_ControlsMenu::getMoviePath()
{
#ifdef __ORBIS__
	if(InputManager.UsingRemoteVita())
	{
		return L"ControlsRemotePlay";
	}
	else
#endif
#ifdef __PSVITA__
	if(InputManager.IsVitaTV())
	{
		return L"ControlsTV";
	}
	else
#endif
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"ControlsSplit";
	}
	else
	{
		return L"Controls";
	}
}

void UIScene_ControlsMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_ControlsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			app.CheckGameSettingsChanged(true,iPad);
			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
		if( pressed )
		{
			//CD - Added for audio
			ui.PlayUISFX(eSFX_Press);
		}
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

void UIScene_ControlsMenu::handleCheckboxToggled(F64 controlId, bool selected)
{
	switch(static_cast<int>(controlId))
	{
	case eControl_InvertLook:
		app.SetGameSettings(m_iPad,eGameSetting_ControlInvertLook,(unsigned char)( selected ) );
		break;
	case eControl_Southpaw:
		app.SetGameSettings(m_iPad,eGameSetting_ControlSouthPaw,(unsigned char)( selected ) );
		PositionAllText(m_iPad);
		break;
	};
}

void UIScene_ControlsMenu::handlePress(F64 controlId, F64 childId)
{
	int control = static_cast<int>(controlId);
	switch(control)
	{
	case eControl_Button0:
	case eControl_Button1:
	case eControl_Button2:
		app.SetGameSettings(m_iPad,eGameSetting_ControlScheme,static_cast<unsigned char>(control));
		LPWSTR layoutString = new wchar_t[ 128 ];
		swprintf( layoutString, 128, L"%ls : %ls", app.GetString( IDS_CURRENT_LAYOUT ),app.GetString(m_iSchemeTextA[control]));	
#ifdef __ORBIS__
		if (!InputManager.UsingRemoteVita())
#endif
		{
			m_labelCurrentLayout.setLabel(layoutString);
		}

		break;
	};
}

void UIScene_ControlsMenu::handleFocusChange(F64 controlId, F64 childId)
{
	int control = static_cast<int>(controlId);
	switch(control)
	{
	case eControl_Button0:
	case eControl_Button1:
	case eControl_Button2:
		m_iCurrentNavigatedControlsLayout=control;
		m_bLayoutChanged = true;
		break;
	};
}

void UIScene_ControlsMenu::PositionAllText(int iPad)
{
	for(unsigned int i = 0; i < e_PadCOUNT; ++i)
	{
		m_labelsPad[i].setLabel(L"");
		m_controlLines[i].setVisible(false);
	}

	if(m_bCreativeMode)
	{
		PositionText(iPad,IDS_CONTROLS_JUMPFLY,MINECRAFT_ACTION_JUMP);
	}
	else
	{
		PositionText(iPad,IDS_CONTROLS_JUMP,MINECRAFT_ACTION_JUMP);
	}
	PositionText(iPad,IDS_CONTROLS_INVENTORY,MINECRAFT_ACTION_INVENTORY);
	PositionText(iPad,IDS_CONTROLS_PAUSE,MINECRAFT_ACTION_PAUSEMENU);
	if(m_bCreativeMode)
	{
		PositionText(iPad,IDS_CONTROLS_SNEAKFLY,MINECRAFT_ACTION_SNEAK_TOGGLE);
	}
	else
	{
		PositionText(iPad,IDS_CONTROLS_SNEAK,MINECRAFT_ACTION_SNEAK_TOGGLE);
	}
	PositionText(iPad,IDS_CONTROLS_USE,MINECRAFT_ACTION_USE);
	PositionText(iPad,IDS_CONTROLS_ACTION,MINECRAFT_ACTION_ACTION);
	PositionText(iPad,IDS_CONTROLS_HELDITEM,MINECRAFT_ACTION_RIGHT_SCROLL);
	PositionText(iPad,IDS_CONTROLS_HELDITEM,MINECRAFT_ACTION_LEFT_SCROLL);
	PositionText(iPad,IDS_CONTROLS_DROP,MINECRAFT_ACTION_DROP);
	PositionText(iPad,IDS_CONTROLS_CRAFTING,MINECRAFT_ACTION_CRAFTING);
	PositionText(iPad,IDS_CONTROLS_THIRDPERSON,MINECRAFT_ACTION_RENDER_THIRD_PERSON);
	PositionText(iPad,IDS_CONTROLS_PLAYERS,MINECRAFT_ACTION_GAME_INFO);

	// Swap for southpaw.
	if ( app.GetGameSettings(m_iPad,eGameSetting_ControlSouthPaw) )
	{
		// Move
		PositionText(iPad,IDS_CONTROLS_LOOK,MINECRAFT_ACTION_RIGHT);
		// Look
		PositionText(iPad,IDS_CONTROLS_MOVE,MINECRAFT_ACTION_LOOK_RIGHT);
	}
	else // Normal right handed.
	{
		// Move
		PositionText(iPad,IDS_CONTROLS_MOVE,MINECRAFT_ACTION_RIGHT);
		// Look
		PositionText(iPad,IDS_CONTROLS_LOOK,MINECRAFT_ACTION_LOOK_RIGHT);
	}

	bool layoutHasDpadFly;
#ifdef __PSVITA__
	layoutHasDpadFly = m_iCurrentNavigatedControlsLayout == 1;
#else
	layoutHasDpadFly = m_iCurrentNavigatedControlsLayout == 0;
#endif

	// If we're in controls mode 1, and creative mode show the dpad for Creative Mode
	if(m_bCreativeMode && layoutHasDpadFly)
	{
		PositionText(iPad,IDS_CONTROLS_DPAD,MINECRAFT_ACTION_DPAD_LEFT);
	}
	m_bLayoutChanged = false;
}

void UIScene_ControlsMenu::PositionText(int iPad,int iTextID, unsigned char ucAction)
{
	unsigned int uiVal = InputManager.GetGameJoypadMaps(m_iCurrentNavigatedControlsLayout, ucAction);

	if (uiVal & _360_JOY_BUTTON_A) PositionTextDirect(iPad, iTextID, e_PadA, true);
	if (uiVal & _360_JOY_BUTTON_B) PositionTextDirect(iPad, iTextID, e_PadB, true);
	if (uiVal & _360_JOY_BUTTON_X) PositionTextDirect(iPad, iTextID, e_PadX, true);
	if (uiVal & _360_JOY_BUTTON_Y) PositionTextDirect(iPad, iTextID, e_PadY, true);
	if (uiVal & _360_JOY_BUTTON_BACK)
	{
#ifdef __ORBIS__
		PositionTextDirect(iPad, iTextID, (InputManager.UsingRemoteVita() ? e_PadTouch : e_PadBack), true);
#else
		PositionTextDirect(iPad, iTextID, e_PadBack, true);
#endif
	}
	if (uiVal & _360_JOY_BUTTON_START) PositionTextDirect(iPad, iTextID, e_PadStart, true);
	if (uiVal & _360_JOY_BUTTON_RB) PositionTextDirect(iPad, iTextID, e_PadRB, true);
	if (uiVal & _360_JOY_BUTTON_LB) PositionTextDirect(iPad, iTextID, e_PadLB, true);
	if (uiVal & _360_JOY_BUTTON_RTHUMB) PositionTextDirect(iPad, iTextID, e_PadRS_1, true);
	if (uiVal & _360_JOY_BUTTON_LTHUMB) PositionTextDirect(iPad, iTextID, e_PadLS_1, true);
		// Look
	if (uiVal & _360_JOY_BUTTON_RSTICK_RIGHT) PositionTextDirect(iPad, iTextID, e_PadRS_2, true);
		// Move
	if (uiVal & _360_JOY_BUTTON_LSTICK_RIGHT) PositionTextDirect(iPad, iTextID, e_PadLS_2, true);
	if (uiVal & _360_JOY_BUTTON_RT) PositionTextDirect(iPad, iTextID, e_PadRT, true);
	if (uiVal & _360_JOY_BUTTON_LT) PositionTextDirect(iPad, iTextID, e_PadLT, true);
	if (uiVal & _360_JOY_BUTTON_DPAD_RIGHT) PositionTextDirect(iPad, iTextID, e_PadDPadRight, true);
	if (uiVal & _360_JOY_BUTTON_DPAD_LEFT) PositionTextDirect(iPad, iTextID, e_PadDPadLeft, true);
	if (uiVal & _360_JOY_BUTTON_DPAD_UP) PositionTextDirect(iPad, iTextID, e_PadDPadUp, true);
	if (uiVal & _360_JOY_BUTTON_DPAD_DOWN) PositionTextDirect(iPad, iTextID, e_PadDPadDown, true);
	}

void UIScene_ControlsMenu::PositionTextDirect(int iPad,int iTextID, int iControlDetailsIndex, bool bShow)
{
	LPCWSTR text = app.GetString(iTextID);

	m_labelsPad[iControlDetailsIndex].setLabel(text);
	m_controlLines[iControlDetailsIndex].setVisible(bShow);
}