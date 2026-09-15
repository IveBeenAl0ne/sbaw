#pragma once

#include "EnchantmentCategory.h"

class DamageSource;
class Mob;

class Enchantment //implements Descriptive<Enchantment> {
{
public :
	//static Enchantment *enchantments[256];
	static EnchantmentArray enchantments;
	static vector<Enchantment *> validEnchantments;

	static const int FREQ_COMMON = 10;
	static const int FREQ_UNCOMMON = 5;
	static const int FREQ_RARE = 2;
	static const int FREQ_VERY_RARE = 1;

	// armor
	static Enchantment *allDamageProtection;
	static Enchantment *fireProtection;
	static Enchantment *fallProtection;
	static Enchantment *explosionProtection;
	static Enchantment *projectileProtection;
	static Enchantment *drownProtection;
	static Enchantment *waterWorker;
    static Enchantment *waterWalker;
	static Enchantment *frostWalker;
	static Enchantment *thorns;

	// weapon
	static Enchantment *damageBonus;
	static Enchantment *damageBonusUndead;
	static Enchantment *damageBonusArthropods;
	static Enchantment *knockback;
	static Enchantment *fireAspect;
	static Enchantment *lootBonus;

	// digger
	static Enchantment *diggingBonus;
	static Enchantment *untouching;
	static Enchantment *digDurability;
	static Enchantment *resourceBonus;

	// bows
	static Enchantment *arrowBonus;
	static Enchantment *arrowKnockback;
	static Enchantment *arrowFire;
	static Enchantment *arrowInfinite;

	// fishing rod
	static Enchantment *lure;
	static Enchantment *luckOfTheSea;

	// misc / treasure
	static Enchantment *mending;

	const int id;

	static void staticCtor();

private:
	const int frequency;

public:
	const EnchantmentCategory *category;

protected:
	int descriptionId;

private:
	void _init(int id);

protected:
	Enchantment(int id, int frequency, const EnchantmentCategory *category);
	Enchantment(int id);

public:
	virtual int getFrequency();
	virtual int getMinLevel();
	virtual int getMaxLevel();
	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getDamageProtection(int level, DamageSource *source);
	virtual float getDamageBonus(int level, shared_ptr<LivingEntity> target);
	virtual bool isCompatibleWith(Enchantment *other) const;
	virtual Enchantment *setDescriptionId(int id);
	virtual int getDescriptionId();
	virtual bool isTreasureOnly() { return false; }
	virtual HtmlString getFullname(int level);
	virtual bool canEnchant(shared_ptr<ItemInstance> item);
	// 4J Added
	wstring getLevelString(int level);

    static std::wstring getEnchantmentName(Enchantment *e, int level)
    {
        if (e == nullptr)
        {
            return L"Unknown";
        }

        std::wstring name = L"Enchantment";

        if (e == Enchantment::allDamageProtection)
        {
            name = L"Protection";
        }
        else if (e == Enchantment::fireProtection)
        {
            name = L"Fire Protection";
        }
        else if (e == Enchantment::fallProtection)
        {
            name = L"Feather Falling";
        }
        else if (e == Enchantment::explosionProtection)
        {
            name = L"Blast Protection";
        }
        else if (e == Enchantment::projectileProtection)
        {
            name = L"Projectile Protection";
        }
        else if (e == Enchantment::damageBonus)
        {
            name = L"Sharpness";
        }
        else if (e == Enchantment::damageBonusUndead)
        {
            name = L"Smite";
        }
        else if (e == Enchantment::damageBonusArthropods)
        {
            name = L"Bane of Arthropods";
        }
        else if (e == Enchantment::knockback)
        {
            name = L"Knockback";
        }
        else if (e == Enchantment::fireAspect)
        {
            name = L"Fire Aspect";
        }
        else if (e == Enchantment::lootBonus)
        {
            name = L"Looting";
        }
        else if (e == Enchantment::diggingBonus)
        {
            name = L"Efficiency";
        }
        else if (e == Enchantment::untouching)
        {
            name = L"Silk Touch";
        }
        else if (e == Enchantment::digDurability)
        {
            name = L"Unbreaking";
        }
        else if (e == Enchantment::resourceBonus)
        {
            name = L"Fortune";
        }
        else if (e == Enchantment::arrowBonus)
        {
            name = L"Power";
        }
        else if (e == Enchantment::arrowKnockback)
        {
            name = L"Punch";
        }
        else if (e == Enchantment::arrowFire)
        {
            name = L"Flame";
        }
        else if (e == Enchantment::arrowInfinite)
        {
            name = L"Infinity";
        }
        else if (e == Enchantment::luckOfTheSea)
        {
            name = L"Luck of the Sea";
        }
        else if (e == Enchantment::lure)
        {
            name = L"Lure";
        }
        else if (e == Enchantment::mending)
        {
            name = L"Mending";
        }
        else if (e == Enchantment::frostWalker)
        {
            name = L"Frost Walker";
        }

        std::wstring levelStr = e->getLevelString(level);

        return name + (levelStr.empty() ? L"" : L" " + levelStr);
    }

    static int getEnchantmentIdByCanonicalName(const std::wstring &name)
    {
        std::wstring lower = name;
        for (auto &c : lower)
        {
            c = towlower(c);
        }

        // clean up the minecraft: prefix (i don't know if it should be done here or in PlayerConnection.cpp, i'll leave that here i guess)
        if (lower.find(L"minecraft:") == 0)
        {
            lower = lower.substr(10);
        }

        if (lower == L"protection")
        {
            return Enchantment::allDamageProtection ? Enchantment::allDamageProtection->id : 0;
        }
        if (lower == L"fire_protection")
        {
            return Enchantment::fireProtection ? Enchantment::fireProtection->id : 1;
        }
        if (lower == L"feather_falling")
        {
            return Enchantment::fallProtection ? Enchantment::fallProtection->id : 2;
        }
        if (lower == L"blast_protection")
        {
            return Enchantment::explosionProtection ? Enchantment::explosionProtection->id : 3;
        }
        if (lower == L"projectile_protection")
        {
            return Enchantment::projectileProtection ? Enchantment::projectileProtection->id : 4;
        }
        if (lower == L"respiration")
        {
            return Enchantment::drownProtection ? Enchantment::drownProtection->id : 5;
        }
        if (lower == L"aqua_affinity")
        {
            return Enchantment::waterWorker ? Enchantment::waterWorker->id : 6;
        }
        if (lower == L"thorns")
        {
            return Enchantment::thorns ? Enchantment::thorns->id : 7;
        }
        if (lower == L"depth_strider")
        {
            return Enchantment::waterWalker ? Enchantment::waterWalker->id : 8;
        }
        if (lower == L"frost_walker")
        {
            return Enchantment::frostWalker ? Enchantment::frostWalker->id : 9;
        }
        if (lower == L"sharpness")
        {
            return Enchantment::damageBonus ? Enchantment::damageBonus->id : 16;
        }
        if (lower == L"smite")
        {
            return Enchantment::damageBonusUndead ? Enchantment::damageBonusUndead->id : 17;
        }
        if (lower == L"bane_of_arthropods")
        {
            return Enchantment::damageBonusArthropods ? Enchantment::damageBonusArthropods->id : 18;
        }
        if (lower == L"knockback")
        {
            return Enchantment::knockback ? Enchantment::knockback->id : 19;
        }
        if (lower == L"fire_aspect")
        {
            return Enchantment::fireAspect ? Enchantment::fireAspect->id : 20;
        }
        if (lower == L"looting")
        {
            return Enchantment::lootBonus ? Enchantment::lootBonus->id : 21;
        }
        if (lower == L"efficiency")
        {
            return Enchantment::diggingBonus ? Enchantment::diggingBonus->id : 32;
        }
        if (lower == L"silk_touch")
        {
            return Enchantment::untouching ? Enchantment::untouching->id : 33;
        }
        if (lower == L"unbreaking")
        {
            return Enchantment::digDurability ? Enchantment::digDurability->id : 34;
        }
        if (lower == L"fortune")
        {
            return Enchantment::resourceBonus ? Enchantment::resourceBonus->id : 35;
        }
        if (lower == L"power")
        {
            return Enchantment::arrowBonus ? Enchantment::arrowBonus->id : 48;
        }
        if (lower == L"punch")
        {
            return Enchantment::arrowKnockback ? Enchantment::arrowKnockback->id : 49;
        }
        if (lower == L"flame")
        {
            return Enchantment::arrowFire ? Enchantment::arrowFire->id : 50;
        }
        if (lower == L"infinity")
        {
            return Enchantment::arrowInfinite ? Enchantment::arrowInfinite->id : 51;
        }
        if (lower == L"luck_of_the_sea")
        {
            return Enchantment::luckOfTheSea ? Enchantment::luckOfTheSea->id : 61;
        }
        if (lower == L"lure")
        {
            return Enchantment::lure ? Enchantment::lure->id : 62;
        }
        if (lower == L"mending")
        {
            return Enchantment::mending ? Enchantment::mending->id : 70;
        }

        return -1;
    }

private:

};
