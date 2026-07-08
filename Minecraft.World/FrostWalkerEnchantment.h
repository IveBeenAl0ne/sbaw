#pragma once

#include "Enchantment.h"

class Level;
class LivingEntity;

class FrostWalkerEnchantment : public Enchantment
{
public:
	FrostWalkerEnchantment(int id, int freq);

	virtual int getMinCost(int level) override;
	virtual int getMaxCost(int level) override;
	virtual int getMaxLevel() override;
	virtual bool isTreasureEnchantment() const override { return true; }

	static void freezeNearby(shared_ptr<LivingEntity> living, Level *level, int px, int py, int pz, int enchLevel);
};
