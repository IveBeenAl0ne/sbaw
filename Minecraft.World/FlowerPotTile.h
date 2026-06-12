#pragma once

#include "BaseEntityTile.h"

class FlowerPotTile : public BaseEntityTile
{
public:
	FlowerPotTile(int id);

	void updateDefaultShape();
	bool isSolidRender(bool isServerLevel = false);
	int getRenderShape();
	bool isCubeShaped();
	bool isValidPlant(int itemId, int aux);
	bool use(Level *level, int x, int y, int z, shared_ptr<Player> player, int clickedFace, float clickX, float clickY, float clickZ, bool soundOnly = false);
	int cloneTileId(Level *level, int x, int y, int z);
	int cloneTileData(Level *level, int x, int y, int z);
	bool useOwnCloneData();
	bool mayPlace(Level *level, int x, int y, int z);
	void neighborChanged(Level *level, int x, int y, int z, int type);
	using Tile::spawnResources;
	void spawnResources(Level *level, int x, int y, int z, int data, float odds, int playerBonusLevel);
	int getResource(int data, Random *random, int playerBonusLevel);

	virtual shared_ptr<TileEntity> newTileEntity(Level *level) override;
};