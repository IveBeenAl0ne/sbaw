#pragma once
#include "stdafx.h"
#include "MultiPlayerLocalPlayer.h"
#include "../Minecraft.World/Item.h"
#include <memory>

class TooltipManager
{
  public:
    static void getTooltips(shared_ptr<MultiplayerLocalPlayer> player,
                   Level *level,
                   HitResult *hitResult,
                   MultiPlayerGameMode *gameMode,
                   int *piAction,
                   int *piJump,
                   int *piUse,
                   int *piAlt);
};
