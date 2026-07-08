#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.entity.h"
#include "Material.h"
#include "Mth.h"
#include "FrostWalkerEnchantment.h"

FrostWalkerEnchantment::FrostWalkerEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::armor_feet)
{
	setDescriptionId(IDS_ENCHANTMENT_FROST_WALKER);
}

int FrostWalkerEnchantment::getMinCost(int level)
{
	return level * 10;
}

int FrostWalkerEnchantment::getMaxCost(int level)
{
	return getMinCost(level) + 15;
}

int FrostWalkerEnchantment::getMaxLevel()
{
	return 2;
}

void FrostWalkerEnchantment::freezeNearby(shared_ptr<LivingEntity> living, Level *level, int px, int py, int pz, int enchLevel)
{
	if (!living->onGround) return;

	int radius = 2 + enchLevel;
	if (radius > 16) radius = 16;
	float f = (float)radius;
	int r = radius;

	for (int dx = -r; dx <= r; dx++)
	{
		for (int dz = -r; dz <= r; dz++)
		{
			int bx = px + dx;
			int by = py - 1;
			int bz = pz + dz;

			double ddx = (bx + 0.5) - living->x;
			double ddy = (by + 0.5) - living->y;
			double ddz = (bz + 0.5) - living->z;
			if (ddx * ddx + ddy * ddy + ddz * ddz > (double)(f * f)) continue;

			if (level->getTile(bx, by + 1, bz) != 0) continue;

			Material *ground = level->getMaterial(bx, by, bz);
			if (ground != Material::water) continue;
			if (level->getData(bx, by, bz) != 0) continue;

			if (Tile::frosted_ice->mayPlace(level, bx, by, bz))
			{
				level->setTileAndData(bx, by, bz, Tile::frosted_ice_Id, 0, Tile::UPDATE_ALL);
			}
		}
	}
}
