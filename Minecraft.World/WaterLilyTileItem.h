#pragma once

#include "ColoredTileItem.h"

class WaterLilyTileItem : public ColoredTileItem
{
public:
	using ColoredTileItem::getColor;
	WaterLilyTileItem(int id);

	virtual shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);
	virtual bool TestUse(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);
	virtual int getColor(int data, int spriteLayer);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
