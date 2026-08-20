#pragma once

#include "Item.h"

class EnderpearlItem : public Item
{
public:
	EnderpearlItem(int id);

	virtual shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> instance, Level *level, shared_ptr<Player> player);
	// 4J added
	virtual bool TestUse(shared_ptr<ItemInstance> instance, Level *level, shared_ptr<Player> player);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
