#pragma once

#include "BaseRailTile.h"

class RailTile : public BaseRailTile
{
	friend class ChunkRebuildData;

private:
	Icon *iconTurn;
	
	public:
	RailTile(int id);

	Icon *getTexture(int face, int data);
	void registerIcons(IconRegister *iconRegister);

	virtual int getInteractTooltipId(Level* level, int x, int y, int z, std::shared_ptr<Player> player, bool bUseItemOn) const override;

protected:
	void updateState(Level *level, int x, int y, int z, int data, int dir, int type);
};