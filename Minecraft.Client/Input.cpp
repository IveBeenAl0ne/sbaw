#include "stdafx.h"
#include "Minecraft.h"
#include "GameMode.h"
#include "../Minecraft.World/net.minecraft.world.entity.player.h"
#include "../Minecraft.World/net.minecraft.world.level.h"
#include "../Minecraft.World/net.minecraft.world.level.storage.h"
#include "Input.h"
#include "../Minecraft.Client/LocalPlayer.h"
#include "Options.h"
#include "Common/Input/PCInput.h"
#ifdef _WINDOWS64
#include "Windows64/KeyboardMouseInput.h" // mouse grab release below
#endif

Input::Input()
{
	xa = 0;
	ya = 0;
	wasJumping = false;
	jumping = false;
	sneaking = false;
	sprinting = false;

	lReset = false;
    rReset = false;
}

void Input::tick(LocalPlayer *player)
{
	// 4J Stu -  Assume that we only need one input class, even though the java has subclasses for keyboard/controller
	// This function is based on the ControllerInput class in the Java, and will probably need changed
	//OutputDebugString("INPUT: Beginning input tick\n");

	Minecraft *pMinecraft=Minecraft::GetInstance();
	int iPad=player->GetXboxPad();

	float controllerXA = 0.0f;
	float controllerYA = 0.0f;

	// 4J-PB minecraft movement seems to be the wrong way round, so invert x!
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LEFT) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_RIGHT) )
		controllerXA = -InputManager.GetJoypadStick_LX(iPad);

	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_FORWARD) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_BACKWARD) )
		controllerYA = InputManager.GetJoypadStick_LY(iPad);

	float kbXA = 0.0f;
	float kbYA = 0.0f;

	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LEFT) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_RIGHT) )
		kbXA = PCInput::GetMoveX(iPad);
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_FORWARD) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_BACKWARD) )
		kbYA = PCInput::GetMoveY(iPad);

	if (kbXA != 0.0f || kbYA != 0.0f)
	{
		xa = kbXA;
		ya = kbYA;
	}
	else
	{
		xa = controllerXA;
		ya = controllerYA;
	}

#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())
	{
		xa = ya = 0.0f;
		player->abilities.flying = true;
	}
#endif

    if (!lReset)
    {
        if (xa*xa+ya*ya==0.0f)
        {
            lReset = true;
        }
        xa = ya = 0.0f;
    }

	// 4J: In flying mode, don't actually toggle sneaking (unless we're riding in which case we need to sneak to dismount)
	if(!player->abilities.flying || player->riding != nullptr)
	{
		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_SNEAK_TOGGLE)) && pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_SNEAK_TOGGLE))
		{
			sneaking=!sneaking;
		}
	}

	// Hold to crouch. The pad still toggles via the press path above.
	if (PCInput::IsKBMDriving(iPad)
		&& pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_SNEAK_TOGGLE)
		&& !player->abilities.flying)
	{
		sneaking = PCInput::ActionDown(iPad, MINECRAFT_ACTION_SNEAK_TOGGLE);
	}

	// Sprint modifier + forward, holds while flying too.
	sprinting = PCInput::ActionDown(iPad, PC_ACTION_SPRINT) && (kbYA > 0.0f);

	if(sneaking)
	{
		xa*=0.3f;
		ya*=0.3f;
	}

    float turnSpeed = 50.0f;

	float tx = 0.0f;
	float ty = 0.0f;

	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_LEFT) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_RIGHT) )
		tx = InputManager.GetJoypadStick_RX(iPad)*(static_cast<float>(app.GetGameSettings(iPad, eGameSetting_Sensitivity_InGame))/100.0f); // apply sensitivity to look
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_UP) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_DOWN) )
		ty = InputManager.GetJoypadStick_RY(iPad)*(static_cast<float>(app.GetGameSettings(iPad, eGameSetting_Sensitivity_InGame))/100.0f); // apply sensitivity to look

#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())	tx = ty = 0.0f;
#endif

	// 4J: WESTY : Invert look Y if required.
	if ( app.GetGameSettings(iPad,eGameSetting_ControlInvertLook) )
	{
		ty = -ty;
	}

    if (!rReset)
    {
        if (tx*tx+ty*ty==0.0f)
        {
            rReset = true;
        }
        tx = ty = 0.0f;
    }

	float turnX = tx * abs(tx) * turnSpeed;
	float turnY = ty * abs(ty) * turnSpeed;

	float mx = 0.0f;
	float my = 0.0f;
	PCInput::GetLookDelta(iPad, mx, my);
	turnX += mx;
	turnY += my;

	player->interpolateTurn(turnX, turnY);

    //jumping = controller.isButtonPressed(0);

	if( PCInput::ActionDown(iPad, MINECRAFT_ACTION_JUMP) && pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_JUMP) )
		jumping = true;
	else
 		jumping = false;

#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())	jumping = false;
#endif

#ifdef _WINDOWS64
	if (iPad == 0 && g_KBMInput.IsKeyPressed(VK_ESCAPE) && g_KBMInput.IsMouseGrabbed())
	{
		g_KBMInput.SetMouseGrabbed(false);
	}
#endif

	//OutputDebugString("INPUT: End input tick\n");
}
