#include "stdafx.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.entity.projectile.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.level.h"
#include "SnowballItem.h"
#include "SoundTypes.h"

SnowballItem::SnowballItem(int id) : Item(id)
{
	this->maxStackSize = 16;
}

shared_ptr<ItemInstance> SnowballItem::use(shared_ptr<ItemInstance> instance, Level *level, shared_ptr<Player> player)
{
	if (!player->abilities.instabuild)
	{
		instance->count--;
	}
	level->playEntitySound((shared_ptr<Entity> ) player, eSoundType_RANDOM_BOW, 0.5f, 0.4f / (random->nextFloat() * 0.4f + 0.8f));
	if (!level->isClientSide) level->addEntity(std::make_shared<Snowball>(level, player));
	return instance;
}

int SnowballItem::getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const
{
    return IDS_TOOLTIPS_THROW;
}
