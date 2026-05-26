#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.dimension.h"
#include "net.minecraft.world.item.enchantment.h"
#include "net.minecraft.world.food.h"
#include "net.minecraft.stats.h"
#include "SlimeTile.h"
#include "Entity.h"

SlimeTile::SlimeTile(int id) : HalfTransparentTile(id, L"slime", Material::clay, false)
{
	friction = 0.8f;
}

int SlimeTile::getRenderLayer()
{
	return 2;
}

int SlimeTile::getRenderShape()
{
	return Tile::SHAPE_SLIME;
}

bool SlimeTile::shouldRenderFace(LevelSource *level, int x, int y, int z, int face)
{
	return true;
}

bool SlimeTile::isSolidRender()
{
    return false;
}

int SlimeTile::getPistonPushReaction()
{
	return Material::PUSH_SLIME;
}

void SlimeTile::fallOn(Level *level, int x, int y, int z,
                       shared_ptr<Entity> entity, float distance)
{
    HalfTransparentTile::fallOn(level, x, y, z, entity, distance);

    if (entity == nullptr)
        return;

    entity->clearFallDamageQueue();

	entity->fallDistance = 0.0f;

	if (entity->isSneaking())
        return;

    if (std::abs(entity->yd) < 0.1f)
        return;

    if (entity->yd < 0.0f)
    {
        entity->yd = -entity->yd;

        if (!(entity->instanceof(eTYPE_LIVINGENTITY)))
        {
            entity->yd *= 0.8f;
        }
    }
}

void SlimeTile::stepOn(Level *level, int x, int y, int z, shared_ptr<Entity> entity)
{
    if (entity != nullptr)
    {
        entity->clearFallDamageQueue();
    }

    if (entity != nullptr &&
        std::abs(entity->yd) < 0.1f &&
        !entity->isSneaking())
    {
        double d0 = 0.4 + std::abs(entity->yd) * 0.2;

        entity->xd *= d0;
        entity->zd *= d0;
        level->playSound(x + 0.5, y + 0.5, z + 0.5, eSoundType_MOB_SLIME_SMALL, 0.4f, 0.8f + level->random->nextFloat() * 0.4f);
    }

    HalfTransparentTile::stepOn(level, x, y, z, entity);
}

void SlimeTile::updateEntityAfterFallOn(Level* level, shared_ptr<Entity> entity)
{
    // stub
}
