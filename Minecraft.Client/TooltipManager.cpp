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

    if (itemInstance)
    {
        // 4J-PB - very special case for boat and empty bucket and glass bottle and more
        bool bUseItem = gameMode->useItem(player, level, itemInstance, true);

        switch (itemInstance->getItem()->id)
        {
        case Item::boat_Id:
        case Tile::waterLily_Id:
            if (bUseItem)
            {
                *piUse = IDS_TOOLTIPS_PLACE;
            }
            break;

        case Item::potion_Id:
            if (bUseItem)
            {
                if (MACRO_POTION_IS_SPLASH(itemInstance->getAuxValue()))
                {
                    *piUse = IDS_TOOLTIPS_THROW;
                }
                else
                {
                    *piUse = IDS_TOOLTIPS_DRINK;
                }
            }
            break;

        case Item::enderPearl_Id:
            if (bUseItem)
            {
                *piUse = IDS_TOOLTIPS_THROW;
            }
            break;

        case Item::eyeOfEnder_Id:
            // This will only work if there is a stronghold in this dimension
            if (bUseItem && (level->dimension->id == 0) && level->getLevelData()->getHasStronghold())
            {
                *piUse = IDS_TOOLTIPS_THROW;
            }
            break;

        case Item::expBottle_Id:
            if (bUseItem)
            {
                *piUse = IDS_TOOLTIPS_THROW;
            }
            break;
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

                /* 4J-Jev:
                 *	Moved this here so we have item tooltips to fallback on
                 *	for noteblocks, enderportals and flowerpots in case of non-standard items.
                 *	(ie. ignite behaviour)
                 */
                if (bUseItemOn && itemInstance != nullptr)
                {
                    switch (itemInstance->getItem()->id)
                    {
                    case Tile::mushroom_brown_Id:
                    case Tile::mushroom_red_Id:
                    case Tile::tallgrass_Id:
                    case Tile::cactus_Id:
                    case Tile::sapling_Id:
                    case Tile::reeds_Id:
                    case Tile::flower_Id:
                    case Tile::rose_Id:
                        *piUse = IDS_TOOLTIPS_PLANT;
                        break;

                        // Things to USE
                    case Item::hoe_wood_Id:
                    case Item::hoe_stone_Id:
                    case Item::hoe_iron_Id:
                    case Item::hoe_diamond_Id:
                    case Item::hoe_gold_Id:
                        *piUse = IDS_TOOLTIPS_TILL;
                        break;

                    case Item::seeds_wheat_Id:
                    case Item::netherwart_seeds_Id:
                        *piUse = IDS_TOOLTIPS_PLANT;
                        break;

                    case Item::dye_powder_Id:
                        // bonemeal grows various plants
                        if (itemInstance->getAuxValue() == DyePowderItem::WHITE)
                        {
                            switch (iTileID)
                            {
                            case Tile::sapling_Id:
                            case Tile::wheat_Id:
                            case Tile::grass_Id:
                            case Tile::mushroom_brown_Id:
                            case Tile::mushroom_red_Id:
                            case Tile::melonStem_Id:
                            case Tile::pumpkinStem_Id:
                            case Tile::carrots_Id:
                            case Tile::potatoes_Id:
                                *piUse = IDS_TOOLTIPS_GROW;
                                break;
                            }
                        }
                        break;

                    case Item::painting_Id:
                        *piUse = IDS_TOOLTIPS_HANG;
                        break;

                    case Item::flintAndSteel_Id:
                    case Item::fireball_Id:
                        *piUse = IDS_TOOLTIPS_IGNITE;
                        break;

                    case Item::fireworks_Id:
                        *piUse = IDS_TOOLTIPS_FIREWORK_LAUNCH;
                        break;

                    case Item::lead_Id:
                        *piUse = IDS_TOOLTIPS_ATTACH;
                        break;

                    default:
                        *piUse = IDS_TOOLTIPS_PLACE;
                        break;
                    }
                }

                switch (iTileID)
                {
                case Tile::anvil_Id:
                case Tile::enchantTable_Id:
                case Tile::brewingStand_Id:
                case Tile::workBench_Id:
                case Tile::furnace_Id:
                case Tile::furnace_lit_Id:
                case Tile::door_wood_Id:
                case Tile::dispenser_Id:
                case Tile::lever_Id:
                case Tile::button_stone_Id:
                case Tile::button_wood_Id:
                case Tile::trapdoor_Id:
                case Tile::fenceGate_Id:
                case Tile::beacon_Id:
                    *piAction = IDS_TOOLTIPS_MINE;
                    *piUse = IDS_TOOLTIPS_USE;
                    break;

                case Tile::chest_Id:
                    *piAction = IDS_TOOLTIPS_MINE;
                    *piUse = (Tile::chest->getContainer(level, x, y, z) != nullptr) ? IDS_TOOLTIPS_OPEN : -1;
                    break;

                case Tile::enderChest_Id:
                case Tile::chest_trap_Id:
                case Tile::dropper_Id:
                case Tile::hopper_Id:
                    *piUse = IDS_TOOLTIPS_OPEN;
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::activatorRail_Id:
                case Tile::goldenRail_Id:
                case Tile::detectorRail_Id:
                case Tile::rail_Id:
                    if (bUseItemOn)
                    {
                        *piUse = IDS_TOOLTIPS_PLACE;
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::bed_Id:
                    if (bUseItemOn)
                    {
                        *piUse = IDS_TOOLTIPS_SLEEP;
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::noteblock_Id:
                    // if in creative mode, we will mine
                    if (player->abilities.instabuild)
                    {
                        *piAction = IDS_TOOLTIPS_MINE;
                    }
                    else
                    {
                        *piAction = IDS_TOOLTIPS_PLAY;
                    }
                    *piUse = IDS_TOOLTIPS_CHANGEPITCH;
                    break;

                case Tile::sign_Id:
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::cauldron_Id:
                    // special case for a cauldron of water and an empty bottle
                    if (itemInstance)
                    {
                        int iID = itemInstance->getItem()->id;
                        int currentData = level->getData(x, y, z);
                        if ((iID == Item::glassBottle_Id) && (currentData > 0))
                        {
                            *piUse = IDS_TOOLTIPS_COLLECT;
                        }
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::cake_Id:
                    if (player->abilities.instabuild) // if in creative mode, we will mine
                    {
                        *piAction = IDS_TOOLTIPS_MINE;
                    }
                    else
                    {
                        if (player->getFoodData()->needsFood()) // 4J-JEV: Changed from healthto hunger.
                        {
                            *piAction = IDS_TOOLTIPS_EAT;
                            *piUse = IDS_TOOLTIPS_EAT;
                        }
                        else
                        {
                            *piAction = IDS_TOOLTIPS_MINE;
                        }
                    }
                    break;

                case Tile::jukebox_Id:
                    if (!bUseItemOn && itemInstance != nullptr)
                    {
                        int iID = itemInstance->getItem()->id;
                        if ((iID >= Item::record_01_Id) && (iID <= Item::record_12_Id))
                        {
                            *piUse = IDS_TOOLTIPS_PLAY;
                        }
                        *piAction = IDS_TOOLTIPS_MINE;
                    }
                    else
                    {
                        if (Tile::jukebox->TestUse(level, x, y, z, player)) // means we can eject
                        {
                            *piUse = IDS_TOOLTIPS_EJECT;
                        }
                        *piAction = IDS_TOOLTIPS_MINE;
                    }
                    break;

                case Tile::flowerPot_Id:
                    if (!bUseItemOn && (itemInstance != nullptr) && (iData == 0))
                    {
                        int iID = itemInstance->getItem()->id;
                        if (iID < 256) // is it a tile?
                        {
                            switch (iID)
                            {
                            case Tile::flower_Id:
                            case Tile::rose_Id:
                            case Tile::sapling_Id:
                            case Tile::mushroom_brown_Id:
                            case Tile::mushroom_red_Id:
                            case Tile::cactus_Id:
                            case Tile::deadBush_Id:
                                *piUse = IDS_TOOLTIPS_PLANT;
                                break;

                            case Tile::tallgrass_Id:
                                if (itemInstance->getAuxValue() != TallGrass::TALL_GRASS)
                                {
                                    *piUse = IDS_TOOLTIPS_PLANT;
                                }
                                break;
                            }
                        }
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::comparator_off_Id:
                case Tile::comparator_on_Id:
                    *piUse = IDS_TOOLTIPS_USE;
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::diode_off_Id:
                case Tile::diode_on_Id:
                    *piUse = IDS_TOOLTIPS_USE;
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::redStoneOre_Id:
                    if (bUseItemOn)
                    {
                        *piUse = IDS_TOOLTIPS_USE;
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                case Tile::door_iron_Id:
                    if (*piUse == IDS_TOOLTIPS_PLACE)
                    {
                        *piUse = -1;
                    }
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;

                default:
                    *piAction = IDS_TOOLTIPS_MINE;
                    break;
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

            switch (entityType)
            {
            case eTYPE_CHICKEN:
                {
                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

                    if (animal->isLeashed() && animal->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                        break;
                    }

                    switch (heldItemId)
                    {
                    case Item::nameTag_Id:
                        *piUse = IDS_TOOLTIPS_NAME;
                        break;

                    case Item::lead_Id:
                        if (!animal->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                        break;

                    default:
                        {
                            if (!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
                            {
                                *piUse = IDS_TOOLTIPS_LOVEMODE;
                            }
                        }
                        break;

                    case -1:
                        break; // 4J-JEV: Empty hand.
                    }
                }
                break;

            case eTYPE_COW:
                {
                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

                    if (animal->isLeashed() && animal->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                        break;
                    }

                    switch (heldItemId)
                    {
                        // Things to USE
                    case Item::nameTag_Id:
                        *piUse = IDS_TOOLTIPS_NAME;
                        break;
                    case Item::lead_Id:
                        if (!animal->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                        break;
                    case Item::bucket_empty_Id:
                        *piUse = IDS_TOOLTIPS_MILK;
                        break;
                    default:
                        {
                            if (!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
                            {
                                *piUse = IDS_TOOLTIPS_LOVEMODE;
                            }
                        }
                        break;

                    case -1:
                        break; // 4J-JEV: Empty hand.
                    }
                }
                break;
            case eTYPE_MUSHROOMCOW:
                {
                    // 4J-PB - Fix for #13081 - No tooltip is displayed for hitting a cow when you have nothing in your hand
                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

                    if (animal->isLeashed() && animal->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                        break;
                    }

                    // It's an item
                    switch (heldItemId)
                    {
                        // Things to USE
                    case Item::nameTag_Id:
                        *piUse = IDS_TOOLTIPS_NAME;
                        break;

                    case Item::lead_Id:
                        if (!animal->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                        break;

                    case Item::bowl_Id:
                    case Item::bucket_empty_Id: // You can milk a mooshroom with either a bowl (mushroom soup) or a bucket (milk)!
                        *piUse = IDS_TOOLTIPS_MILK;
                        break;
                    case Item::shears_Id:
                        {
                            if (player->isAllowedToAttackAnimals())
                            {
                                *piAction = IDS_TOOLTIPS_HIT;
                            }
                            if (!animal->isBaby())
                            {
                                *piUse = IDS_TOOLTIPS_SHEAR;
                            }
                        }
                        break;
                    default:
                        {
                            if (!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
                            {
                                *piUse = IDS_TOOLTIPS_LOVEMODE;
                            }
                        }
                        break;

                    case -1:
                        break; // 4J-JEV: Empty hand.
                    }
                }
                break;

            case eTYPE_BOAT:
                *piAction = IDS_TOOLTIPS_MINE;
                *piUse = IDS_TOOLTIPS_SAIL;
                break;

            case eTYPE_MINECART_RIDEABLE:
                *piAction = IDS_TOOLTIPS_MINE;
                *piUse = IDS_TOOLTIPS_RIDE; // are we in the minecart already? - 4J-JEV: Doesn't matter anymore.
                break;

            case eTYPE_MINECART_FURNACE:
                *piAction = IDS_TOOLTIPS_MINE;

                // if you have coal, it'll go. Is there an object in hand?
                if (heldItemId == Item::coal_Id)
                {
                    *piUse = IDS_TOOLTIPS_USE;
                }
                break;

            case eTYPE_MINECART_CHEST:
            case eTYPE_MINECART_HOPPER:
                *piAction = IDS_TOOLTIPS_MINE;
                *piUse = IDS_TOOLTIPS_OPEN;
                break;

            case eTYPE_MINECART_SPAWNER:
            case eTYPE_MINECART_TNT:
                *piUse = IDS_TOOLTIPS_MINE;
                break;

            case eTYPE_SHEEP:
                {
                    // can dye a sheep
                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    shared_ptr<Sheep> sheep = dynamic_pointer_cast<Sheep>(hitResult->entity);

                    if (sheep->isLeashed() && sheep->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                        break;
                    }

                    switch (heldItemId)
                    {
                    case Item::nameTag_Id:
                        *piUse = IDS_TOOLTIPS_NAME;
                        break;

                    case Item::lead_Id:
                        if (!sheep->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                        break;

                    case Item::dye_powder_Id:
                        {
                            // convert to tile-based color value (0 is white instead of black)
                            int newColor = ColoredTile::getTileDataForItemAuxValue(heldItem->getAuxValue());

                            // can only use a dye on sheep that haven't been sheared
                            if (!(sheep->isSheared() && sheep->getColor() != newColor))
                            {
                                *piUse = IDS_TOOLTIPS_DYE;
                            }
                        }
                        break;
                    case Item::shears_Id:
                        {
                            // can only shear a sheep that hasn't been sheared
                            if (!sheep->isBaby() && !sheep->isSheared())
                            {
                                *piUse = IDS_TOOLTIPS_SHEAR;
                            }
                        }

                        break;
                    default:
                        {
                            if (!sheep->isBaby() && !sheep->isInLove() && (sheep->getAge() == 0) && sheep->isFood(heldItem))
                            {
                                *piUse = IDS_TOOLTIPS_LOVEMODE;
                            }
                        }
                        break;

                    case -1:
                        break; // 4J-JEV: Empty hand.
                    }
                }
                break;

            case eTYPE_PIG:
                {
                    // can ride a pig
                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    shared_ptr<Pig> pig = dynamic_pointer_cast<Pig>(hitResult->entity);

                    if (pig->isLeashed() && pig->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                    }
                    else if (heldItemId == Item::lead_Id)
                    {
                        if (!pig->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                    }
                    else if (heldItemId == Item::nameTag_Id)
                    {
                        *piUse = IDS_TOOLTIPS_NAME;
                    }
                    else if (pig->hasSaddle()) // does the pig have a saddle?
                    {
                        *piUse = IDS_TOOLTIPS_MOUNT;
                    }
                    else if (!pig->isBaby())
                    {
                        if (player->inventory->IsHeldItem())
                        {
                            switch (heldItemId)
                            {
                            case Item::saddle_Id:
                                *piUse = IDS_TOOLTIPS_SADDLE;
                                break;

                            default:
                                {
                                    if (!pig->isInLove() && (pig->getAge() == 0) && pig->isFood(heldItem))
                                    {
                                        *piUse = IDS_TOOLTIPS_LOVEMODE;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
                break;

            case eTYPE_WOLF:
                // can be tamed, fed, and made to sit/stand, or enter love mode
                {
                    shared_ptr<Wolf> wolf = dynamic_pointer_cast<Wolf>(hitResult->entity);

                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    if (wolf->isLeashed() && wolf->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                        break;
                    }

                    switch (heldItemId)
                    {
                    case Item::nameTag_Id:
                        *piUse = IDS_TOOLTIPS_NAME;
                        break;

                    case Item::lead_Id:
                        if (!wolf->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                        break;

                    case Item::bone_Id:
                        if (!wolf->isAngry() && !wolf->isTame())
                        {
                            *piUse = IDS_TOOLTIPS_TAME;
                        }
                        else if (equalsIgnoreCase(player->getUUID(), wolf->getOwnerUUID()))
                        {
                            if (wolf->isSitting())
                            {
                                *piUse = IDS_TOOLTIPS_FOLLOWME;
                            }
                            else
                            {
                                *piUse = IDS_TOOLTIPS_SIT;
                            }
                        }

                        break;
                    case Item::enderPearl_Id:
                        // Use is throw, so don't change the tips for the wolf
                        break;
                    case Item::dye_powder_Id:
                        if (wolf->isTame())
                        {
                            if (ColoredTile::getTileDataForItemAuxValue(heldItem->getAuxValue()) != wolf->getCollarColor())
                            {
                                *piUse = IDS_TOOLTIPS_DYECOLLAR;
                            }
                            else if (wolf->isSitting())
                            {
                                *piUse = IDS_TOOLTIPS_FOLLOWME;
                            }
                            else
                            {
                                *piUse = IDS_TOOLTIPS_SIT;
                            }
                        }
                        break;
                    default:
                        if (wolf->isTame())
                        {
                            if (wolf->isFood(heldItem))
                            {
                                if (wolf->GetSynchedHealth() < wolf->getMaxHealth())
                                {
                                    *piUse = IDS_TOOLTIPS_HEAL;
                                }
                                else
                                {
                                    if (!wolf->isBaby() && !wolf->isInLove() && (wolf->getAge() == 0))
                                    {
                                        *piUse = IDS_TOOLTIPS_LOVEMODE;
                                    }
                                }
                                // break out here
                                break;
                            }

                            if (equalsIgnoreCase(player->getUUID(), wolf->getOwnerUUID()))
                            {
                                if (wolf->isSitting())
                                {
                                    *piUse = IDS_TOOLTIPS_FOLLOWME;
                                }
                                else
                                {
                                    *piUse = IDS_TOOLTIPS_SIT;
                                }
                            }
                        }
                        break;
                    }
                }
                break;
            case eTYPE_OCELOT:
                {
                    shared_ptr<Ocelot> ocelot = dynamic_pointer_cast<Ocelot>(hitResult->entity);

                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }

                    if (ocelot->isLeashed() && ocelot->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                    }
                    else if (heldItemId == Item::lead_Id)
                    {
                        if (!ocelot->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                    }
                    else if (heldItemId == Item::nameTag_Id)
                    {
                        *piUse = IDS_TOOLTIPS_NAME;
                    }
                    else if (ocelot->isTame())
                    {
                        // 4J-PB - if you have a raw fish in your hand, you will feed the ocelot rather than have it sit/follow
                        if (ocelot->isFood(heldItem))
                        {
                            if (!ocelot->isBaby())
                            {
                                if (!ocelot->isInLove())
                                {
                                    if (ocelot->getAge() == 0)
                                    {
                                        *piUse = IDS_TOOLTIPS_LOVEMODE;
                                    }
                                }
                                else
                                {
                                    *piUse = IDS_TOOLTIPS_FEED;
                                }
                            }
                        }
                        else if (equalsIgnoreCase(player->getUUID(), ocelot->getOwnerUUID()) && !ocelot->isSittingOnTile())
                        {
                            if (ocelot->isSitting())
                            {
                                *piUse = IDS_TOOLTIPS_FOLLOWME;
                            }
                            else
                            {
                                *piUse = IDS_TOOLTIPS_SIT;
                            }
                        }
                    }
                    else if (heldItemId >= 0)
                    {
                        if (ocelot->isFood(heldItem))
                        {
                            *piUse = IDS_TOOLTIPS_TAME;
                        }
                    }
                }
                break;

            case eTYPE_PLAYER:
                {
                    // Fix for #58576 - TU6: Content: Gameplay: Hit button prompt is available when attacking a host who has "Invisible" option turned on
                    shared_ptr<Player> TargetPlayer = dynamic_pointer_cast<Player>(hitResult->entity);

                    if (!TargetPlayer->hasInvisiblePrivilege()) // This means they are invisible, not just that they have the privilege
                    {
                        if (app.GetGameHostOption(eGameHostOption_PvP) && player->isAllowedToAttackPlayers())
                        {
                            *piAction = IDS_TOOLTIPS_HIT;
                        }
                    }
                }
                break;

            case eTYPE_ITEM_FRAME:
                {
                    shared_ptr<ItemFrame> itemFrame = dynamic_pointer_cast<ItemFrame>(hitResult->entity);

                    // is the frame occupied?
                    if (itemFrame->getItem() != nullptr)
                    {
                        // rotate the item
                        *piUse = IDS_TOOLTIPS_ROTATE;
                    }
                    else
                    {
                        // is there an object in hand?
                        if (heldItemId >= 0)
                        {
                            *piUse = IDS_TOOLTIPS_PLACE;
                        }
                    }

                    *piAction = IDS_TOOLTIPS_HIT;
                }
                break;

            case eTYPE_VILLAGER:
                {
                    // 4J-JEV: Cannot leash villagers.

                    shared_ptr<Villager> villager = dynamic_pointer_cast<Villager>(hitResult->entity);
                    if (!villager->isBaby())
                    {
                        *piUse = IDS_TOOLTIPS_TRADE;
                    }
                    *piAction = IDS_TOOLTIPS_HIT;
                }
                break;

            case eTYPE_ZOMBIE:
                {
                    shared_ptr<Zombie> zomb = dynamic_pointer_cast<Zombie>(hitResult->entity);
                    static GoldenAppleItem *goldapple = static_cast<GoldenAppleItem *>(Item::apple_gold);

                    // zomb->hasEffect(MobEffect::weakness) - not present on client.
                    if (zomb->isVillager() && zomb->isWeakened() && (heldItemId == Item::apple_gold_Id) && !goldapple->isFoil(heldItem))
                    {
                        *piUse = IDS_TOOLTIPS_CURE;
                    }
                    *piAction = IDS_TOOLTIPS_HIT;
                }
                break;

            case eTYPE_HORSE:
                {
                    shared_ptr<EntityHorse> horse = dynamic_pointer_cast<EntityHorse>(hitResult->entity);

                    bool heldItemIsFood = false, heldItemIsLove = false, heldItemIsArmour = false;

                    switch (heldItemId)
                    {
                    case Item::wheat_Id:
                    case Item::sugar_Id:
                    case Item::bread_Id:
                    case Tile::hayBlock_Id:
                    case Item::apple_Id:
                        heldItemIsFood = true;
                        break;
                    case Item::carrotGolden_Id:
                    case Item::apple_gold_Id:
                        heldItemIsLove = true;
                        heldItemIsFood = true;
                        break;
                    case Item::horseArmorDiamond_Id:
                    case Item::horseArmorGold_Id:
                    case Item::horseArmorMetal_Id:
                        heldItemIsArmour = true;
                        break;
                    }

                    if (horse->isLeashed() && horse->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                    }
                    else if (heldItemId == Item::lead_Id)
                    {
                        if (!horse->isLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                    }
                    else if (heldItemId == Item::nameTag_Id)
                    {
                        *piUse = IDS_TOOLTIPS_NAME;
                    }
                    else if (horse->isBaby()) // 4J-JEV: Can't ride baby horses due to morals.
                    {
                        if (heldItemIsFood)
                        {
                            // 4j - Can feed foles to speed growth.
                            *piUse = IDS_TOOLTIPS_FEED;
                        }
                    }
                    else if (!horse->isTamed())
                    {
                        if (heldItemId == -1)
                        {
                            // 4j - Player not holding anything, ride and attempt to break untamed horse.
                            *piUse = IDS_TOOLTIPS_TAME;
                        }
                        else if (heldItemIsFood)
                        {
                            // 4j - Attempt to make it like you more by feeding it.
                            *piUse = IDS_TOOLTIPS_FEED;
                        }
                    }
                    else if (player->isSneaking() || (heldItemId == Item::saddle_Id) || (horse->canWearArmor() && heldItemIsArmour))
                    {
                        // 4j - Access horses inventory
                        if (*piUse == -1)
                        {
                            *piUse = IDS_TOOLTIPS_OPEN;
                        }
                    }
                    else if (horse->canWearBags() && !horse->isChestedHorse() && (heldItemId == Tile::chest_Id))
                    {
                        // 4j - Attach saddle-bags (chest) to donkey or mule.
                        *piUse = IDS_TOOLTIPS_ATTACH;
                    }
                    else if (horse->isReadyForParenting() && heldItemIsLove)
                    {
                        // 4j - Different food to mate horses.
                        *piUse = IDS_TOOLTIPS_LOVEMODE;
                    }
                    else if (heldItemIsFood && (horse->getHealth() < horse->getMaxHealth()))
                    {
                        // 4j - Horse is damaged and can eat held item to heal
                        *piUse = IDS_TOOLTIPS_HEAL;
                    }
                    else
                    {
                        // 4j - Ride tamed horse.
                        *piUse = IDS_TOOLTIPS_MOUNT;
                    }

                    if (player->isAllowedToAttackAnimals())
                    {
                        *piAction = IDS_TOOLTIPS_HIT;
                    }
                }
                break;

            case eTYPE_ENDERDRAGON:
                // 4J-JEV: Enderdragon cannot be named.
                *piAction = IDS_TOOLTIPS_HIT;
                break;

            case eTYPE_LEASHFENCEKNOT:
                *piAction = IDS_TOOLTIPS_UNLEASH;
                if (heldItemId == Item::lead_Id && LeashItem::bindPlayerMobsTest(player, level, player->x, player->y, player->z))
                {
                    *piUse = IDS_TOOLTIPS_ATTACH;
                }
                else
                {
                    *piUse = IDS_TOOLTIPS_UNLEASH;
                }
                break;

            default:
                if (hitResult->entity->instanceof(eTYPE_MOB))
                {
                    shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(hitResult->entity);
                    if (mob->isLeashed() && mob->getLeashHolder() == player)
                    {
                        *piUse = IDS_TOOLTIPS_UNLEASH;
                    }
                    else if (heldItemId == Item::lead_Id)
                    {
                        if (!mob->isLeashed() && mob->canBeLeashed())
                        {
                            *piUse = IDS_TOOLTIPS_LEASH;
                        }
                    }
                    else if (heldItemId == Item::nameTag_Id)
                    {
                        *piUse = IDS_TOOLTIPS_NAME;
                    }
                }
                *piAction = IDS_TOOLTIPS_HIT;
                break;
            }
            break;
        }
    }
}
