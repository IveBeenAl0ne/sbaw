#pragma once

#include "Item.h"

class Icon;

class BottleItem : public Item
{
public:
	BottleItem(int id);

	//@Override
	Icon *getIcon(int auxValue);

	virtual shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);
	virtual bool TestUse(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);

	//@Override
	void registerIcons(IconRegister *iconRegister);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
