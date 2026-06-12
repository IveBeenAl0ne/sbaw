#pragma once
#include "TileEntity.h"

class FlowerPotTileEntity : public TileEntity
{
public:
    eINSTANCEOF GetType() { return eTYPE_FLOWERPOTTILEENTITY; }
    static TileEntity* create() { return new FlowerPotTileEntity(); }

    FlowerPotTileEntity();
    void setFlower(int itemId, int aux);
    bool hasFlower() const;
    shared_ptr<ItemInstance> getFlowerItem() const;
    int getItemId() const { return m_itemId; }
    int getAux() const { return m_aux; }
    void clearFlower();
    virtual void load(CompoundTag* tag) override;
    virtual void save(CompoundTag* tag) override;
    virtual shared_ptr<TileEntity> clone() override;
    virtual shared_ptr<Packet> getUpdatePacket();
    virtual void handleUpdateTag(CompoundTag* tag);
private:
    int m_itemId;
    int m_aux;
};