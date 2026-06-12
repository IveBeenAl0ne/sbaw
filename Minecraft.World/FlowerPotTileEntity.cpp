#include "stdafx.h"
#include "FlowerPotTileEntity.h"
#include "com.mojang.nbt.h"
#include "TileEntityDataPacket.h"

FlowerPotTileEntity::FlowerPotTileEntity()
{
    m_itemId = 0;
    m_aux = 0;
}

void FlowerPotTileEntity::setFlower(int itemId, int aux)
{
    m_itemId = itemId;
    m_aux = aux;
    setChanged();
}

bool FlowerPotTileEntity::hasFlower() const
{
    return m_itemId != 0;
}

shared_ptr<ItemInstance> FlowerPotTileEntity::getFlowerItem() const
{
    if (!hasFlower()) return nullptr;
    return std::make_shared<ItemInstance>(m_itemId, 1, m_aux);
}

void FlowerPotTileEntity::clearFlower()
{
    m_itemId = 0;
    m_aux = 0;
    setChanged();
}

void FlowerPotTileEntity::load(CompoundTag* tag)
{
    TileEntity::load(tag);
    m_itemId = tag->getInt(L"ItemId");
    m_aux = tag->getInt(L"Aux");
}

void FlowerPotTileEntity::save(CompoundTag* tag)
{
    TileEntity::save(tag);
    tag->putInt(L"ItemId", m_itemId);
    tag->putInt(L"Aux", m_aux);
}

shared_ptr<TileEntity> FlowerPotTileEntity::clone()
{
    auto result = std::make_shared<FlowerPotTileEntity>();
    TileEntity::clone(result);
    result->m_itemId = m_itemId;
    result->m_aux = m_aux;
    return result;
}

shared_ptr<Packet> FlowerPotTileEntity::getUpdatePacket()
{
    CompoundTag* tag = new CompoundTag();
    save(tag);
    return std::make_shared<TileEntityDataPacket>(x, y, z, TileEntityDataPacket::TYPE_FLOWER_POT, tag);
}

void FlowerPotTileEntity::handleUpdateTag(CompoundTag* tag)
{
    load(tag);
}