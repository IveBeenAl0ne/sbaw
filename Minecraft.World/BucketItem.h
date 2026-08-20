#pragma once

#include "Item.h"

class Player;
class Level;

class BucketItem : public Item
{
private:
	int content;

public:
	BucketItem(int id, int content);

	virtual bool TestUse(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);
	virtual shared_ptr<ItemInstance> use(shared_ptr<ItemInstance> itemInstance, Level *level, shared_ptr<Player> player);

	bool emptyBucket(Level *level, int xt, int yt, int zt);

	virtual int getUseTooltipId(std::shared_ptr<ItemInstance> itemInstance, std::shared_ptr<Player> player, Level* level, int x, int y, int z, bool bUseItem) const;
};
