#include "stdafx.h"
#include "net.minecraft.commands.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.damagesource.h"
#include "net.minecraft.world.level.h"
#include "BasicTypeContainers.h"
#include "KillCommand.h"
#include "PlayerSelector.h"


static void killEntity(shared_ptr<Entity> entity)
{
    if (entity->instanceof(eTYPE_LIVINGENTITY))
    {
        auto living = dynamic_pointer_cast<LivingEntity>(entity);
        if (living != nullptr)
        {
            living->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
            return;
        }
        entity->remove();
        return;
    }
    entity->remove();
}

EGameCommand KillCommand::getId()
{
    return eGameCommand_Kill;
}

int KillCommand::getPermissionLevel()
{
    return LEVEL_ALL;
}

void KillCommand::execute(shared_ptr<CommandSender> source, byteArray commandData)
{
    shared_ptr<Player> senderPlayer = dynamic_pointer_cast<Player>(source);
    if (senderPlayer == nullptr)
    {
        return;
    }

    // /kill with no arguments kills the sender
    if (commandData.length == 0 || commandData.data == nullptr)
    {
        senderPlayer->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        return;
    }

    ByteArrayInputStream bais(commandData);
    DataInputStream dis(&bais);
    wstring targetName = dis.readUTF();

    // use PlayerSelector 
    if (PlayerSelector::hasArguments(targetName))
    {
        auto entities = PlayerSelector::matchEntities(source, targetName);
        if (entities.empty())
        {
            source->sendMessage(L"No entity was found.");
            return;
        }

        for (auto& entity : entities)
            if (!entity->removed)
                killEntity(entity);

        source->sendMessage(L"Killed " + to_wstring(entities.size()) + L" entities.");
        return;
    }

    // By player name
    Level* level = senderPlayer->level;
    if (level == nullptr)
    {
        return;
    }

    shared_ptr<Player> targetPlayer = level->getPlayerByName(targetName);
    if (targetPlayer != nullptr)
    {
        targetPlayer->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        return;
    }

    source->sendMessage(L"No entity was found.");
}