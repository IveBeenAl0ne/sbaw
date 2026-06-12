#pragma once

#include "Tile_SPU.h"
#include "FlowerPotTileEntity.h"

class FlowerPotTile_SPU : public EntityTile_SPU
{
public:
	FlowerPotTile_SPU(int id) : EntityTile_SPU(id) {}

	void updateDefaultShape()
	{
		float size = 6.0f / 16.0f;
		float half = size / 2;
		setShape(0.5f - half, 0, 0.5f - half, 0.5f + half, size, 0.5f + half);
	}
 	bool isSolidRender(bool isServerLevel = false) { return false; }
	int getRenderShape() { return SHAPE_FLOWER_POT; }
	bool isCubeShaped() { return false; }
	virtual void updateShape(ChunkRebuildData *level, int x, int y, int z, int forceData = -1, TileEntity* forceEntity = nullptr) override
    {
        updateDefaultShape();

		int plantItemId = 0;
        int plantAux = 0;

        if (forceEntity && forceEntity->GetType() == eTYPE_FLOWERPOTTILEENTITY)
        {
            FlowerPotTileEntity *potTe = (FlowerPotTileEntity*)forceEntity;
            if (potTe->hasFlower())
            {
                plantItemId = potTe->getItemId();
                plantAux = potTe->getAux();
            }
        }

        if (plantItemId == 0)
        {
            int data = level->getData(x, y, z);
            if (data != 0)
            {
                switch (data)
                {
                case 1: plantItemId = Tile_SPU::rose_Id; plantAux = 0; break;
                case 2: plantItemId = Tile_SPU::flower_Id; plantAux = 0; break;
                case 3: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_DEFAULT; break;
                case 4: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_EVERGREEN; break;
                case 5: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_BIRCH; break;
                case 6: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_JUNGLE; break;
                case 7: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_ACACIA; break;
                case 8: plantItemId = Tile_SPU::sapling_Id; plantAux = Sapling_SPU::TYPE_DARK_OAK; break;
                case 9: plantItemId = Tile_SPU::mushroom_red_Id; plantAux = 0; break;
                case 10: plantItemId = Tile_SPU::mushroom_brown_Id; plantAux = 0; break;
                case 11: plantItemId = Tile_SPU::cactus_Id; plantAux = 0; break;
                case 12: plantItemId = Tile_SPU::deadBush_Id; plantAux = 0; break;
                case 13: plantItemId = Tile_SPU::tallgrass_Id; plantAux = TallGrass_SPU::FERN; break;
                }
            }

            if (plantItemId == 0) return;
        }

        TileRenderer_SPU renderer(level);
        Tesselator_SPU *t = &level->m_tesselator;
        float yOff = 4.0f / 16.0f;
        t->addOffset(0, yOff, 0);

        if (plantItemId != 0)
        {
            float yOff = 4.0f / 16.0f;
            level->addOffset(0, yOff, 0);

            if (plantItemId == Tile_SPU::rose_Id || plantItemId == Tile_SPU::flower_Id)
            {
                Tile_SPU *plantTile = Tile_SPU::tiles[plantItemId];
                if (plantTile)
                {
                    plantTile->updateShape(level, x, y, z, plantAux, nullptr);
                    renderer.tesselateCrossTexture(plantTile, plantAux, x, y, z, 0.75f);
                }
            }
            else if (plantItemId == Tile_SPU::sapling_Id)
            {
                Tile_SPU *saplingTile = Tile_SPU::tiles[Tile_SPU::sapling_Id];
                if (saplingTile)
                {
                    saplingTile->updateShape(level, x, y, z, plantAux, nullptr);
                    renderer.tesselateCrossTexture(saplingTile, plantAux, x, y, z, 0.75f);
                }
            }
            else if (plantItemId == Tile_SPU::mushroom_red_Id || plantItemId == Tile_SPU::mushroom_brown_Id)
            {
                Tile_SPU *mushroomTile = Tile_SPU::tiles[plantItemId];
                if (mushroomTile)
                {
                    mushroomTile->updateShape(level, x, y, z, 0, nullptr);
                    renderer.tesselateCrossTexture(mushroomTile, 0, x, y, z, 0.75f);
                }
            }
            else if (plantItemId == Tile_SPU::tallgrass_Id && plantAux == TallGrass_SPU::FERN)
            {
                Tile_SPU *grassTile = Tile_SPU::tiles[Tile_SPU::tallgrass_Id];
                if (grassTile)
                {
                    grassTile->updateShape(level, x, y, z, TallGrass_SPU::FERN, nullptr);
                    renderer.tesselateCrossTexture(grassTile, TallGrass_SPU::FERN, x, y, z, 0.75f);
                }
            }
            else if (plantItemId == Tile_SPU::deadBush_Id)
            {
                Tile_SPU *deadBushTile = Tile_SPU::tiles[Tile_SPU::deadBush_Id];
                if (deadBushTile)
                {
                    deadBushTile->updateShape(level, x, y, z, 0, nullptr);
                    renderer.tesselateCrossTexture(deadBushTile, 0, x, y, z, 0.75f);
                }
            }
            else if (plantItemId == Tile_SPU::cactus_Id)
            {
                Tile_SPU *cactusTile = Tile_SPU::tiles[Tile_SPU::cactus_Id];
                if (cactusTile)
                {
                    float halfSize = 0.25f / 2;
                    renderer.setShape(0.5f - halfSize, 0.0f, 0.5f - halfSize, 0.5f + halfSize, 0.25f, 0.5f + halfSize);
                    renderer.renderBlock(cactusTile, level, x, y, z);
                    renderer.setShape(0.5f - halfSize, 0.25f, 0.5f - halfSize, 0.5f + halfSize, 0.5f, 0.5f + halfSize);
                    renderer.renderBlock(cactusTile, level, x, y, z);
                    renderer.setShape(0.5f - halfSize, 0.5f, 0.5f - halfSize, 0.5f + halfSize, 0.75f, 0.5f + halfSize);
                    renderer.renderBlock(cactusTile, level, x, y, z);
                    renderer.setShape(0, 0, 0, 1, 1, 1);
                }
            }

            level->addOffset(0, -yOff, 0);
        }
    }
};