#pragma once

#include "ComplexItem.h"

class EmptyMapItem : public ComplexItem
{
public:
	EmptyMapItem(int id);

	shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
