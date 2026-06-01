#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.commands.h"

using std::wstring;
using std::map;
using std::vector;
using std::shared_ptr;

//translation of net.minecraft.command.PlayerSelector from java to c++
class PlayerSelector
{
public:
    // Returns true if the token is a selector pattern (@p, @a, @r, @e, @s)
    static bool hasArguments(const wstring& token);

    static bool matchesMultiplePlayers(const wstring& token);

    static shared_ptr<Player> matchOnePlayer(shared_ptr<CommandSender> sender, const wstring& token);

    static shared_ptr<Entity> matchOneEntity(shared_ptr<CommandSender> sender, const wstring& token);

    static vector<shared_ptr<Entity>> matchEntities(shared_ptr<CommandSender> sender, const wstring& token, eINSTANCEOF targetType = eTYPE_ENTITY);

private:

    static map<wstring, wstring> getArgumentMap(const wstring& argString);
    static int    parseIntWithDefault(const map<wstring, wstring>& args, const wstring& key, int def);
    static double parseDoubleWithDefault(const map<wstring, wstring>& args, const wstring& key, double def);
    static wstring getArg(const map<wstring, wstring>& args, const wstring& key);

    static void getOrigin(const map<wstring, wstring>& args, shared_ptr<CommandSender> sender,
                          double& ox, double& oy, double& oz);

    static bool hasWorldBindingArgs(const map<wstring, wstring>& args);

    static int normaliseAngle(int angle);

    static vector<shared_ptr<Entity>> applyCountAndSort(
        vector<shared_ptr<Entity>>& candidates,
        const map<wstring, wstring>& args,
        shared_ptr<CommandSender> sender,
        const wstring& selectorType,
        double ox, double oy, double oz);

    static bool matchesType     (shared_ptr<Entity> e, const wstring& selectorType, const map<wstring, wstring>& args);
    static bool matchesLevel    (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesGameMode (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesTeam     (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesName     (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesRange    (shared_ptr<Entity> e, const map<wstring, wstring>& args, double ox, double oy, double oz);
    static bool matchesRotation (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesNbt      (shared_ptr<Entity> e, const map<wstring, wstring>& args);
    static bool matchesAABB     (shared_ptr<Entity> e, const map<wstring, wstring>& args, double ox, double oy, double oz);
};