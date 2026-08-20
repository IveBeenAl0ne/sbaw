#pragma once

#include "Item.h"

class ItemInstance;

class ExperienceItem : public Item
{
public:
	ExperienceItem(int id);

	virtual bool isFoil(shared_ptr<ItemInstance> itemInstance);
	virtual shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);
	virtual bool TestUse(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
