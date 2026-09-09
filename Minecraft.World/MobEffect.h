#pragma once
using namespace std;

#include "AttributeModifier.h"

class Mob;
class MobEffectInstance;
class Attribute;

class MobEffect
{
public:
	enum EMobEffectIcon
	{
		e_MobEffectIcon_None,
		e_MobEffectIcon_Blindness,
		e_MobEffectIcon_FireResistance,
		e_MobEffectIcon_Haste,
		e_MobEffectIcon_Hunger,
		e_MobEffectIcon_Invisiblity,
		e_MobEffectIcon_JumpBoost,
		e_MobEffectIcon_MiningFatigue,
		e_MobEffectIcon_Nausea,
		e_MobEffectIcon_NightVision,
		e_MobEffectIcon_Poison,
		e_MobEffectIcon_Regeneration,
		e_MobEffectIcon_Resistance,
		e_MobEffectIcon_Slowness,
		e_MobEffectIcon_Speed,
		e_MobEffectIcon_Strength,
		e_MobEffectIcon_WaterBreathing,
		e_MobEffectIcon_Weakness,
		e_MobEffectIcon_Wither,
		e_MobEffectIcon_HealthBoost,
		e_MobEffectIcon_Absorption,

		e_MobEffectIcon_COUNT,
	};

	static const int NUM_EFFECTS = 32;
	static MobEffect *effects[NUM_EFFECTS];

	static MobEffect *voidEffect;
	static MobEffect *movementSpeed;
	static MobEffect *movementSlowdown;
	static MobEffect *digSpeed;
	static MobEffect *digSlowdown;
	static MobEffect *damageBoost;
	static MobEffect *heal;
	static MobEffect *harm;
	static MobEffect *jump;
	static MobEffect *confusion;
	static MobEffect *regeneration;
	static MobEffect *damageResistance;
	static MobEffect *fireResistance;
	static MobEffect *waterBreathing;
	static MobEffect *invisibility;
	static MobEffect *blindness;
	static MobEffect *nightVision;
	static MobEffect *hunger;
	static MobEffect *weakness;
	static MobEffect *poison;
	static MobEffect *wither;
	static MobEffect *healthBoost;
	static MobEffect *absorption;
	static MobEffect *saturation;
	static MobEffect *reserved_24;
	static MobEffect *reserved_25;
	static MobEffect *reserved_26;
	static MobEffect *reserved_27;
	static MobEffect *reserved_28;
	static MobEffect *reserved_29;
	static MobEffect *reserved_30;
	static MobEffect *reserved_31;

	const int id;

	static void staticCtor();

private:
	unordered_map<Attribute *, AttributeModifier *> attributeModifiers;
	int descriptionId;
	int m_postfixDescriptionId; // 4J added
	EMobEffectIcon icon; // 4J changed type
	const bool _isHarmful;
	double durationModifier;
	bool _isDisabled;
	const eMinecraftColour color;

protected:
	MobEffect(int id, bool isHarmful, eMinecraftColour color);

	//MobEffect *setIcon(int xPos, int yPos);
	MobEffect *setIcon(EMobEffectIcon icon);

public:
	virtual int getId();
	virtual void applyEffectTick(shared_ptr<LivingEntity> mob, int amplification);
	virtual void applyInstantenousEffect(shared_ptr<LivingEntity> source, shared_ptr<LivingEntity> mob, int amplification, double scale);
	virtual bool isInstantenous();
	virtual bool isDurationEffectTick(int remainingDuration, int amplification);

	MobEffect *setDescriptionId(unsigned int id);
	unsigned int getDescriptionId(int iData = -1);

	// 4J Added
	MobEffect *setPostfixDescriptionId(unsigned int id);
	unsigned int getPostfixDescriptionId(int iData = -1);

	bool hasIcon();
	EMobEffectIcon getIcon(); // 4J changed return type
	bool isHarmful();
	static wstring formatDuration(MobEffectInstance *instance);

protected:
	MobEffect *setDurationModifier(double durationModifier);

public:
	virtual double getDurationModifier();
	virtual MobEffect *setDisabled();
	virtual bool isDisabled();
	virtual eMinecraftColour getColor();

	virtual MobEffect *addAttributeModifier(Attribute *attribute, eMODIFIER_ID id, double amount, int operation);
	virtual unordered_map<Attribute *, AttributeModifier *> *getAttributeModifiers();
	virtual void removeAttributeModifiers(shared_ptr<LivingEntity> entity, BaseAttributeMap *attributes, int amplifier);
	virtual void addAttributeModifiers(shared_ptr<LivingEntity> entity, BaseAttributeMap *attributes, int amplifier);
	virtual double getAttributeModifierValue(int amplifier, AttributeModifier *original);
    static int getEffectIdByCanonicalName(const std::wstring &name)
    {
        std::wstring lower = name;
        for (auto &c : lower)
        {
            c = towlower(c);
        }

        // I know this is ugly but it didn't leave me the choice...
        if (lower == L"speed")
        {
            return MobEffect::movementSpeed ? MobEffect::movementSpeed->id : 1;
        }
        if (lower == L"slowness")
        {
            return MobEffect::movementSlowdown ? MobEffect::movementSlowdown->id : 2;
        }
        if (lower == L"haste")
        {
            return MobEffect::digSpeed ? MobEffect::digSpeed->id : 3;
        }
        if (lower == L"mining_fatigue")
        {
            return MobEffect::digSlowdown ? MobEffect::digSlowdown->id : 4;
        }
        if (lower == L"strength")
        {
            return MobEffect::damageBoost ? MobEffect::damageBoost->id : 5;
        }
        if (lower == L"instant_health")
        {
            return MobEffect::heal ? MobEffect::heal->id : 6;
        }
        if (lower == L"instant_damage")
        {
            return MobEffect::harm ? MobEffect::harm->id : 7;
        }
        if (lower == L"jump_boost")
        {
            return MobEffect::jump ? MobEffect::jump->id : 8;
        }
        if (lower == L"nausea")
        {
            return MobEffect::confusion ? MobEffect::confusion->id : 9;
        }
        if (lower == L"regeneration")
        {
            return MobEffect::regeneration ? MobEffect::regeneration->id : 10;
        }
        if (lower == L"resistance")
        {
            return MobEffect::damageResistance ? MobEffect::damageResistance->id : 11;
        }
        if (lower == L"fire_resistance")
        {
            return MobEffect::fireResistance ? MobEffect::fireResistance->id : 12;
        }
        if (lower == L"water_breathing")
        {
            return MobEffect::waterBreathing ? MobEffect::waterBreathing->id : 13;
        }
        if (lower == L"invisibility")
        {
            return MobEffect::invisibility ? MobEffect::invisibility->id : 14;
        }
        if (lower == L"blindness")
        {
            return MobEffect::blindness ? MobEffect::blindness->id : 15;
        }
        if (lower == L"night_vision")
        {
            return MobEffect::nightVision ? MobEffect::nightVision->id : 16;
        }
        if (lower == L"hunger")
        {
            return MobEffect::hunger ? MobEffect::hunger->id : 17;
        }
        if (lower == L"weakness")
        {
            return MobEffect::weakness ? MobEffect::weakness->id : 18;
        }
        if (lower == L"poison")
        {
            return MobEffect::poison ? MobEffect::poison->id : 19;
        }
        if (lower == L"wither")
        {
            return MobEffect::wither ? MobEffect::wither->id : 20;
        }
        if (lower == L"health_boost")
        {
            return MobEffect::healthBoost ? MobEffect::healthBoost->id : 21;
        }
        if (lower == L"absorption")
        {
            return MobEffect::absorption ? MobEffect::absorption->id : 22;
        }
        if (lower == L"saturation")
        {
            return MobEffect::saturation ? MobEffect::saturation->id : 23;
        }

        return 0;
    }
};
