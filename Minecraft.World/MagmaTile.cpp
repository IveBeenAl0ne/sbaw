#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "MagmaTile.h"

MagmaTile::MagmaTile(int id) : Tile(id, Material::stone)
{
	setLightEmission(0.2f);
}
