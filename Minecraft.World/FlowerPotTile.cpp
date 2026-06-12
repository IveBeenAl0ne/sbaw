#include "stdafx.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "FlowerPotTile.h"
#include "FlowerPotTileEntity.h"

FlowerPotTile::FlowerPotTile(int id) : BaseEntityTile(id, Material::decoration, isSolidRender())
{
    updateDefaultShape();
    sendTileData();
}

void FlowerPotTile::updateDefaultShape()
{
	float size = 6.0f / 16.0f;
	float half = size / 2;
	setShape(0.5f - half, 0, 0.5f - half, 0.5f + half, size, 0.5f + half);
}

bool FlowerPotTile::isSolidRender(bool isServerLevel)
{
	return false;
}

int FlowerPotTile::getRenderShape()
{
	return SHAPE_FLOWER_POT;
}

bool FlowerPotTile::isCubeShaped()
{
	return false;
}

bool FlowerPotTile::isValidPlant(int itemId, int aux)
{
    if (itemId == Tile::rose_Id || itemId == Tile::flower_Id)
        return true;
    if (itemId == Tile::sapling_Id && aux >= 0 && aux <= 6) return true;
    if (itemId == Tile::mushroom_brown_Id || itemId == Tile::mushroom_red_Id) return true;
    if (itemId == Tile::cactus_Id || itemId == Tile::deadBush_Id ||
        (itemId == Tile::tallgrass_Id && aux == TallGrass::FERN))
        return true;
    return false;
}

bool FlowerPotTile::use(Level* level, int x, int y, int z, shared_ptr<Player> player,
                        int clickedFace, float clickX, float clickY, float clickZ, bool soundOnly)
{
    auto held = player->inventory->getSelected();
    if (!held) return false;
    if (level->getData(x, y, z) != 0) return false;
    int itemId = held->getItem()->id;
    int aux = held->getAuxValue();
    if (!isValidPlant(itemId, aux)) return false;

    auto te = level->getTileEntity(x, y, z);
    if (!te)
    {
        return false;
    }
    auto potTe = std::dynamic_pointer_cast<FlowerPotTileEntity>(te);
    if (!potTe) return false;
    potTe->setFlower(itemId, aux);
    level->setData(x, y, z, 1, Tile::UPDATE_CLIENTS);

    if (!player->abilities.instabuild)
    {
        if (--held->count <= 0)
            player->inventory->setItem(player->inventory->selected, nullptr);
    }
    return true;
}

int FlowerPotTile::cloneTileId(Level* level, int x, int y, int z)
{
    auto te = level->getTileEntity(x, y, z);
    if (te)
    {
        auto potTe = std::dynamic_pointer_cast<FlowerPotTileEntity>(te);
        if (potTe && potTe->hasFlower())
        {
            auto item = potTe->getFlowerItem();
            if (item) return item->id;
        }
    }
    return Item::flowerPot_Id;
}

int FlowerPotTile::cloneTileData(Level* level, int x, int y, int z)
{
    auto te = level->getTileEntity(x, y, z);
    if (te)
    {
        auto potTe = std::dynamic_pointer_cast<FlowerPotTileEntity>(te);
        if (potTe && potTe->hasFlower())
        {
            auto item = potTe->getFlowerItem();
            if (item) return item->getAuxValue();
        }
    }
    return 0;
}

bool FlowerPotTile::useOwnCloneData()
{
	return true;
}

bool FlowerPotTile::mayPlace(Level *level, int x, int y, int z)
{
	return Tile::mayPlace(level, x, y, z) && level->isTopSolidBlocking(x, y - 1, z);
}

void FlowerPotTile::neighborChanged(Level *level, int x, int y, int z, int type)
{
	if (!level->isTopSolidBlocking(x, y - 1, z))
	{
		spawnResources(level, x, y, z, level->getData(x, y, z), 0);

		level->removeTile(x, y, z);
	}
}

void FlowerPotTile::spawnResources(Level* level, int x, int y, int z, int data, float odds, int playerBonusLevel)
{
    Tile::spawnResources(level, x, y, z, data, odds, playerBonusLevel);
    auto te = level->getTileEntity(x, y, z);
    if (te)
    {
        auto potTe = std::dynamic_pointer_cast<FlowerPotTileEntity>(te);
        if (potTe && potTe->hasFlower())
        {
            auto flowerItem = potTe->getFlowerItem();
            if (flowerItem) popResource(level, x, y, z, flowerItem);
        }
    }
}

int FlowerPotTile::getResource(int data, Random *random, int playerBonusLevel)
{
	return Item::flowerPot_Id;
}

shared_ptr<TileEntity> FlowerPotTile::newTileEntity(Level *level)
{
    return std::make_shared<FlowerPotTileEntity>();
}