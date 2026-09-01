#include "stdafx.h"
#include "../Minecraft.World/TilePos.h"
#include "../Minecraft.World/ByteBuffer.h"
#include "../Minecraft.World/ChestTileEntity.h"
#include "../Minecraft.World/Difficulty.h"
#include "../Minecraft.World/File.h"
#include "../Minecraft.World/HellRandomLevelSource.h"
#include "../Minecraft.World/IntCache.h"
#include "../Minecraft.World/Minecraft.World.h"
#include "../Minecraft.World/MobEffect.h"
#include "../Minecraft.World/SparseDataStorage.h"
#include "../Minecraft.World/SparseLightStorage.h"
#include "../Minecraft.World/StrongholdFeature.h"
#include "../Minecraft.World/System.h"
#include "../Minecraft.World/TilePos.h"
#include "../Minecraft.World/Villager.h"
#include "../Minecraft.World/net.minecraft.h"
#include "../Minecraft.World/net.minecraft.stats.h"
#include "../Minecraft.World/net.minecraft.world.entity.animal.h"
#include "../Minecraft.World/net.minecraft.world.entity.h"
#include "../Minecraft.World/net.minecraft.world.entity.item.h"
#include "../Minecraft.World/net.minecraft.world.entity.monster.h"
#include "../Minecraft.World/net.minecraft.world.entity.player.h"
#include "../Minecraft.World/net.minecraft.world.item.h"
#include "../Minecraft.World/net.minecraft.world.level.chunk.h"
#include "../Minecraft.World/net.minecraft.world.level.dimension.h"
#include "../Minecraft.World/net.minecraft.world.level.h"
#include "../Minecraft.World/net.minecraft.world.level.storage.h"
#include "../Minecraft.World/net.minecraft.world.level.tile.h"
#include "../Minecraft.World/net.minecraft.world.phys.h"
#include "AchievementPopup.h"
#include "Camera.h"
#include "Chunk.h"
#include "ClientConnection.h"
#include "Common/UI/UIScene.h"
#include "CreativeMode.h"
#include "DeathScreen.h"
#include "DemoLevel.h"
#include "DemoUser.h"
#include "EntityRenderDispatcher.h"
#include "ErrorScreen.h"
#include "FrustumCuller.h"
#include "GameMode.h"
#include "GameRenderer.h"
#include "GuiParticles.h"
#include "HumanoidModel.h"
#include "InBedChatScreen.h"
#include "Input.h"
#include "InventoryScreen.h"
#include "ItemInHandRenderer.h"
#include "LevelRenderer.h"
#include "Minecraft.h"
#include "MultiPlayerLevel.h"
#include "MultiPlayerLocalPlayer.h"
#include "Options.h"
#include "ParticleEngine.h"
#include "ProgressRenderer.h"
#include "Screen.h"
#include "StatsCounter.h"
#include "SurvivalMode.h"
#include "TextureManager.h"
#include "TexturePackRepository.h"
#include "Textures.h"
#include "TileEntityRenderDispatcher.h"
#include "Timer.h"
#include "TitleScreen.h"
#include "User.h"
#include "Windows64/Windows64_Xuid.h"
#include "stdafx.h"
#ifdef _XBOX
#include "Xbox/Network/NetworkPlayerXbox.h"
#endif
#include "Common/UI/IUIScene_CreativeMenu.h"
#include "Common/UI/UIFontData.h"
#include "DLCTexturePack.h"

#ifdef __ORBIS__
#include "Orbis/Network/PsPlusUpsellWrapper_Orbis.h"
#endif

#include "TooltipManager.h"

void TooltipManager::getTooltips(shared_ptr<MultiplayerLocalPlayer> player,
                                 Level *level,
                                 HitResult *hitResult,
                                 MultiPlayerGameMode *gameMode,
                                 int *piAction,
                                 int *piJump,
                                 int *piUse,
                                 int *piAlt)
{

    if (piAction == nullptr || piJump == nullptr || piUse == nullptr || piAlt == nullptr)
    {
        return;
    }
    if (player == nullptr || level == nullptr)
    {
        return;
    }

    if (player->isUnderLiquid(Material::water))
    {
        *piJump = IDS_TOOLTIPS_SWIMUP;
        return;
    }
    else
    {
        *piJump = -1;
    }

    *piUse = -1;
    *piAction = -1;
    *piAlt = -1;

    // 4J-PB another special case for when the player is sleeping in a bed
    if (player->isSleeping() && (level != nullptr) && level->isClientSide)
    {
        *piUse = IDS_TOOLTIPS_WAKEUP;
        return;
    }

    if (player->isRiding())
    {
        shared_ptr<Entity> mount = player->riding;

        if (mount->instanceof(eTYPE_MINECART) || mount->instanceof(eTYPE_BOAT))
        {
            *piAlt = IDS_TOOLTIPS_EXIT;
        }
        else
        {
            *piAlt = IDS_TOOLTIPS_DISMOUNT;
        }

        return;
    }

    // no hit result, but we may have something in our hand that we can do something with
    shared_ptr<ItemInstance> itemInstance = player->inventory->getSelected();

    // 4J-JEV: Moved all this here to avoid having it in 3 different places.

    if (itemInstance)
    {
        bool bUseItem = gameMode->useItem(player, level, itemInstance, true);

        int tooltipId = itemInstance->getItem()->getUseTooltipId(itemInstance, player, level, 0, 0, 0, bUseItem); // Use 0 0 0 coordinates here because we don't need them
        if (tooltipId != 0)
        {
            *piUse = tooltipId;
        }
    }

    if (hitResult != nullptr)
    {
        switch (hitResult->type)
        {
        case HitResult::TILE:
            {
                int x, y, z;
                x = hitResult->x;
                y = hitResult->y;
                z = hitResult->z;
                int face = hitResult->f;

                int iTileID = level->getTile(x, y, z);
                int iData = level->getData(x, y, z);

                if (gameMode != nullptr && gameMode->getTutorial() != nullptr)
                {
                    // 4J Stu - For the tutorial we want to be able to record what items we look at so that we can give hints
                    gameMode->getTutorial()->onLookAt(iTileID, iData);
                }

                // 4J-PB - Call the useItemOn with the TestOnly flag set
                bool bUseItemOn = gameMode->useItemOn(player, level, itemInstance, x, y, z, face, hitResult->pos, true);

                // Not4J Anaël: This is here in case of special blocks like noteblocks and ender pearls/eyes.
                if (bUseItemOn && itemInstance != nullptr)
                {
                    int tooltipId = itemInstance->getItem()->getUseTooltipId(itemInstance, player, level, x, y, z, bUseItemOn);
                    if (tooltipId != 0)
                    {
                        *piUse = tooltipId;
                    }
                }

                if (iTileID > 0 && iTileID < 256 && Tile::tiles[iTileID] != nullptr)
                {
                    Tile *pTile = Tile::tiles[iTileID];
                    int interactTooltip = pTile->getInteractTooltipId(level, x, y, z, player, bUseItemOn);
                    if (interactTooltip != 0)
                    {
                        *piUse = interactTooltip;
                    }

                    *piAction = pTile->getAttackTooltipId(level, x, y, z, player, bUseItemOn);
                }
            }
            break;

        case HitResult::ENTITY:
            eINSTANCEOF entityType = hitResult->entity->GetType();

            if ((gameMode != nullptr) && (gameMode->getTutorial() != nullptr))
            {
                // 4J Stu - For the tutorial we want to be able to record what items we look at so that we can give hints
                gameMode->getTutorial()->onLookAtEntity(hitResult->entity);
            }

            shared_ptr<ItemInstance> heldItem = nullptr;
            if (player->inventory->IsHeldItem())
            {
                heldItem = player->inventory->getSelected();
            }
            int heldItemId = heldItem != nullptr ? heldItem->getItem()->id : -1;

            shared_ptr<Entity> entity = hitResult->entity;
            if (entity != nullptr)
            {
                *piAction = entity->getAttackTooltipId(player);

                int useTooltip = entity->getInteractTooltipId(level, player, heldItem);
                if (useTooltip != 0)
                {
                    *piUse = useTooltip;
                }
            }
        }
    }
}
