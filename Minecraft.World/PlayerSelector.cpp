#include "stdafx.h"
#include "PlayerSelector.h"
#include "EntityTypeMap.h"
#include "MojangsonParser.h"
#include "CompoundTag.h"
//#include "net.minecraft.world.entity.player.h"
#include"../Minecraft.Client/ServerPlayer.h"
#include "../Minecraft.Client/ServerPlayerGameMode.h"
#include <algorithm>
#include <cwctype>
#include <sstream>
#include <regex>
#include <random>

using std::wstring;
using std::map;
using std::vector;
using std::shared_ptr;

static wstring toLowerW(const wstring& s)
{
    wstring r = s;
    std::transform(r.begin(), r.end(), r.begin(), towlower);
    return r;
}

static bool startsWith(const wstring& s, const wstring& prefix)
{
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}


static bool parseToken(const wstring& token, wstring& selectorType, wstring& argString)
{
    if (token.size() < 2 || token[0] != L'@') return false;
    wchar_t t = towlower(token[1]);
    if (t != L'p' && t != L'a' && t != L'r' && t != L'e' && t != L's') return false;
    selectorType = wstring(1, t);

    if (token.size() == 2)
    {
        argString = L"";
        return true;
    }
    if (token[2] == L'[' && token.back() == L']')
    {
        argString = token.substr(3, token.size() - 4);
        return true;
    }
    return false;
}


map<wstring, wstring> PlayerSelector::getArgumentMap(const wstring& argString)
{
    map<wstring, wstring> result;
    if (argString.empty()) return result;

    vector<wstring> tokens;
    {
        int depth = 0;
        wstring cur;
        for (wchar_t c : argString)
        {
            if (c == L'{') depth++;
            if (c == L'}') depth--;
            if (c == L',' && depth == 0) { tokens.push_back(cur); cur.clear(); }
            else cur += c;
        }
        if (!cur.empty()) tokens.push_back(cur);
    }

    static const wchar_t* positional[] = { L"x", L"y", L"z", L"r" };
    int positionalIdx = 0;

    for (const wstring& tok : tokens)
    {
        size_t eq = tok.find(L'=');
        if (eq == wstring::npos)
        {
            if (positionalIdx < 4 && !tok.empty())
                result[positional[positionalIdx]] = tok;
            positionalIdx++;
        }
        else
        {
            wstring key   = tok.substr(0, eq);
            wstring value = tok.substr(eq + 1);
         
            while (!key.empty()   && iswspace(key.front()))   key.erase(key.begin());
            while (!key.empty()   && iswspace(key.back()))    key.pop_back();
            while (!value.empty() && iswspace(value.front())) value.erase(value.begin());
            while (!value.empty() && iswspace(value.back()))  value.pop_back();
            result[key] = value;
            positionalIdx = 4; 
        }
    }

    return result;
}

int PlayerSelector::parseIntWithDefault(const map<wstring, wstring>& args, const wstring& key, int def)
{
    auto it = args.find(key);
    if (it == args.end() || it->second.empty()) return def;
    try { return std::stoi(it->second); } catch (...) { return def; }
}

double PlayerSelector::parseDoubleWithDefault(const map<wstring, wstring>& args, const wstring& key, double def)
{
    auto it = args.find(key);
    if (it == args.end() || it->second.empty()) return def;
    try { return std::stod(it->second); } catch (...) { return def; }
}

wstring PlayerSelector::getArg(const map<wstring, wstring>& args, const wstring& key)
{
    auto it = args.find(key);
    return (it != args.end()) ? it->second : L"";
}

void PlayerSelector::getOrigin(const map<wstring, wstring>& args, shared_ptr<CommandSender> sender,
                               double& ox, double& oy, double& oz)
{
    ox = oy = oz = 0.0;
    if (sender)
    {
        auto e = dynamic_pointer_cast<Entity>(sender);
        if (e) { ox = e->x; oy = e->y; oz = e->z; }
    }
    auto it = args.find(L"x"); if (it != args.end()) { try { ox = std::stod(it->second); } catch (...) {} }
    it = args.find(L"y");      if (it != args.end()) { try { oy = std::stod(it->second); } catch (...) {} }
    it = args.find(L"z");      if (it != args.end()) { try { oz = std::stod(it->second); } catch (...) {} }
}

bool PlayerSelector::hasWorldBindingArgs(const map<wstring, wstring>& args)
{
    static const wchar_t* binding[] = { L"x", L"y", L"z", L"dx", L"dy", L"dz", L"rm", L"r" };
    for (auto k : binding)
        if (args.count(k)) return true;
    return false;
}

int PlayerSelector::normaliseAngle(int a)
{
    a = a % 360;
    if (a >= 180)  a -= 360;
    if (a < -180)  a += 360;
    return a;
}


bool PlayerSelector::matchesType(shared_ptr<Entity> e, const wstring& selectorType, const map<wstring, wstring>& args)
{
    wstring typeVal = getArg(args, L"type");
    bool invert = false;
    if (!typeVal.empty() && typeVal[0] == L'!')
    {
        invert = true;
        typeVal = typeVal.substr(1);
    }


    if (typeVal.empty())
    {
        bool isPlayer = e->instanceof(eTYPE_PLAYER);
        if (selectorType == L"e") return true;         // all entities
        return isPlayer;                               // only players for @a/@p/@r
    }

    bool matches;
    if (typeVal == L"player")
        matches = e->instanceof(eTYPE_PLAYER);
    else
    {
        eINSTANCEOF t = EntityTypeMap::getTypeFromName(toLowerW(typeVal));
        matches = (t != eTYPE_NOTSET) && e->instanceof(t);
    }

    return invert ? !matches : matches;
}

bool PlayerSelector::matchesLevel(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    int lm = parseIntWithDefault(args, L"lm", -1);
    int l  = parseIntWithDefault(args, L"l",  -1);
    if (lm == -1 && l == -1) return true;

    auto player = dynamic_pointer_cast<Player>(e);
    if (!player) return false;

    int xp = player->experienceLevel;
    if (lm >= 0 && xp < lm) return false;
    if (l  >= 0 && xp > l)  return false;
    return true;
}

bool PlayerSelector::matchesGameMode(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    int m = parseIntWithDefault(args, L"m", -1);
    if (m == -1) return true;

    auto player = dynamic_pointer_cast<ServerPlayer>(e);
    if (!player) return false;

    return player->gameMode->getGameModeForPlayer()->getId() == m;
}

bool PlayerSelector::matchesTeam(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    wstring teamVal = getArg(args, L"team");
    if (teamVal.empty()) return true;

    bool invert = false;
    if (teamVal[0] == L'!') { invert = true; teamVal = teamVal.substr(1); }

    // No scoreboard system
    wstring entityTeam = L"";  // placeholder: implement when scoreboard exists
    bool matches = (entityTeam == teamVal);
    return invert ? !matches : matches;
}

bool PlayerSelector::matchesName(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    wstring nameVal = getArg(args, L"name");
    if (nameVal.empty()) return true;

    bool invert = false;
    if (nameVal[0] == L'!') { invert = true; nameVal = nameVal.substr(1); }

    bool matches = (e->getName() == nameVal);
    return invert ? !matches : matches;
}

bool PlayerSelector::matchesRange(shared_ptr<Entity> e, const map<wstring, wstring>& args,
                                  double ox, double oy, double oz)
{
    int rm = parseIntWithDefault(args, L"rm", -1);
    int r  = parseIntWithDefault(args, L"r",  -1);
    if (rm < 0 && r < 0) return true;

    double dx = e->x - ox;
    double dy = e->y - oy;
    double dz = e->z - oz;
    double distSq = dx*dx + dy*dy + dz*dz;

    if (rm >= 0 && distSq < (double)(rm * rm)) return false;
    if (r  >= 0 && distSq > (double)(r  * r))  return false;
    return true;
}

bool PlayerSelector::matchesRotation(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    bool hasRy  = args.count(L"ry")  > 0;
    bool hasRym = args.count(L"rym") > 0;
    bool hasRx  = args.count(L"rx")  > 0;
    bool hasRxm = args.count(L"rxm") > 0;

    if (!hasRy && !hasRym && !hasRx && !hasRxm) return true;

    if (hasRy || hasRym)
    {
        int yMin = normaliseAngle(parseIntWithDefault(args, L"rym", 0));
        int yMax = normaliseAngle(parseIntWithDefault(args, L"ry",  359));
        int yaw  = normaliseAngle((int)std::floor(e->yRot));
        bool ok = (yMin > yMax) ? (yaw >= yMin || yaw <= yMax) : (yaw >= yMin && yaw <= yMax);
        if (!ok) return false;
    }

    if (hasRx || hasRxm)
    {
        int xMin   = normaliseAngle(parseIntWithDefault(args, L"rxm", 0));
        int xMax   = normaliseAngle(parseIntWithDefault(args, L"rx",  359));
        int pitch  = normaliseAngle((int)std::floor(e->xRot));
        bool ok = (xMin > xMax) ? (pitch >= xMin || pitch <= xMax) : (pitch >= xMin && pitch <= xMax);
        if (!ok) return false;
    }

    return true;
}

bool PlayerSelector::matchesNbt(shared_ptr<Entity> e, const map<wstring, wstring>& args)
{
    wstring nbtVal = getArg(args, L"nbt");
    if (nbtVal.empty()) return true;

    bool invert = false;
    if (nbtVal[0] == L'!') { invert = true; nbtVal = nbtVal.substr(1); }

    CompoundTag* filter = nullptr;
    try { filter = MojangsonParser::parse(nbtVal); }
    catch (...) { return !invert; }

    CompoundTag entityNbt(L"");           
   if (e == nullptr) return !invert;

try {
    e->saveWithoutId(&entityNbt);
} catch (...) {
    delete filter;
    return !invert;
}

    auto* allTags = filter->getAllTags(); 
    bool matches = true;
    for (Tag* filterTag : *allTags)
    {
        Tag* entityTag = entityNbt.get(filterTag->getName());
        if (entityTag == nullptr || !filterTag->equals(entityTag))
        {
            matches = false;
            break;
        }
    }
    delete allTags;   
    delete filter;    

    return invert ? !matches : matches;
}

bool PlayerSelector::matchesAABB(shared_ptr<Entity> e, const map<wstring, wstring>& args,
                                  double ox, double oy, double oz)
{
    if (!args.count(L"dx") && !args.count(L"dy") && !args.count(L"dz")) return true;

    int dx = parseIntWithDefault(args, L"dx", 0);
    int dy = parseIntWithDefault(args, L"dy", 0);
    int dz = parseIntWithDefault(args, L"dz", 0);

    double x0 = ox + (dx < 0 ? dx : 0);
    double y0 = oy + (dy < 0 ? dy : 0);
    double z0 = oz + (dz < 0 ? dz : 0);
    double x1 = ox + (dx < 0 ? 0 : dx) + 1;
    double y1 = oy + (dy < 0 ? 0 : dy) + 1;
    double z1 = oz + (dz < 0 ? 0 : dz) + 1;

    return e->x >= x0 && e->x < x1 &&
           e->y >= y0 && e->y < y1 &&
           e->z >= z0 && e->z < z1;
}

vector<shared_ptr<Entity>> PlayerSelector::applyCountAndSort(
    vector<shared_ptr<Entity>>& candidates,
    const map<wstring, wstring>& args,
    shared_ptr<CommandSender> sender,
    const wstring& selectorType,
    double ox, double oy, double oz)
{
    
    int defaultCount = (selectorType == L"a" || selectorType == L"e") ? 0 : 1;
    int c = parseIntWithDefault(args, L"c", defaultCount);

    if (selectorType == L"r")
    {
        
        std::shuffle(candidates.begin(), candidates.end(), std::mt19937{std::random_device{}()});
    }
    else if (selectorType == L"p" || selectorType == L"a" || selectorType == L"e")
    {
        
        std::sort(candidates.begin(), candidates.end(),
            [ox, oy, oz](const shared_ptr<Entity>& a, const shared_ptr<Entity>& b)
            {
                double da = (a->x-ox)*(a->x-ox) + (a->y-oy)*(a->y-oy) + (a->z-oz)*(a->z-oz);
                double db = (b->x-ox)*(b->x-ox) + (b->y-oy)*(b->y-oy) + (b->z-oz)*(b->z-oz);
                return da < db;
            });
    }

    if (c == 0) return candidates;

    
    if (c < 0)
    {
        std::reverse(candidates.begin(), candidates.end());
        c = -c;
    }

    int take = (std::min)(c, (int)candidates.size());
    return vector<shared_ptr<Entity>>(candidates.begin(), candidates.begin() + take);
}

bool PlayerSelector::hasArguments(const wstring& token)
{
    wstring st, as;
    return parseToken(token, st, as);
}

bool PlayerSelector::matchesMultiplePlayers(const wstring& token)
{
    wstring st, as;
    if (!parseToken(token, st, as)) return false;
    auto args = getArgumentMap(as);
    int defC = (st == L"a" || st == L"e") ? 0 : 1;
    return parseIntWithDefault(args, L"c", defC) != 1;
}

vector<shared_ptr<Entity>> PlayerSelector::matchEntities(
    shared_ptr<CommandSender> sender,
    const wstring& token,
    eINSTANCEOF targetType)
{
    wstring selectorType, argString;
    if (!parseToken(token, selectorType, argString))
        return {};

    auto args = getArgumentMap(argString);

    double ox, oy, oz;
    getOrigin(args, sender, ox, oy, oz);

    Level* level = nullptr;
    {
        auto senderEntity = dynamic_pointer_cast<Entity>(sender);
        if (senderEntity) level = senderEntity->level;
    }
    if (!level) return {};

    // @s 
    if (selectorType == L"s")
    {
        auto senderEntity = dynamic_pointer_cast<Entity>(sender);
        if (!senderEntity || senderEntity->removed) return {};
        if (targetType != eTYPE_ENTITY && !senderEntity->instanceof(targetType)) return {};
        return { senderEntity };
    }

    vector<shared_ptr<Entity>> candidates;

    // Gather from all entities in the level
    for (auto& entity : level->entities)
    {
        if (entity == nullptr || entity->removed) continue;
        if (targetType != eTYPE_ENTITY && !entity->instanceof(targetType)) continue;

        if (!matchesType    (entity, selectorType, args)) continue;
        if (!matchesLevel   (entity, args))               continue;
        if (!matchesGameMode(entity, args))               continue;
        if (!matchesTeam    (entity, args))               continue;
        if (!matchesName    (entity, args))               continue;
        if (!matchesRange   (entity, args, ox, oy, oz))   continue;
        if (!matchesRotation(entity, args))               continue;
        if (!matchesNbt     (entity, args))               continue;
        if (!matchesAABB    (entity, args, ox, oy, oz))   continue;

        candidates.push_back(entity);
    }

    return applyCountAndSort(candidates, args, sender, selectorType, ox, oy, oz);
}

shared_ptr<Entity> PlayerSelector::matchOneEntity(shared_ptr<CommandSender> sender, const wstring& token)
{
    auto list = matchEntities(sender, token, eTYPE_ENTITY);
    return (list.size() == 1) ? list[0] : nullptr;
}

shared_ptr<Player> PlayerSelector::matchOnePlayer(shared_ptr<CommandSender> sender, const wstring& token)
{
    auto list = matchEntities(sender, token, eTYPE_PLAYER);
    if (list.size() != 1) return nullptr;
    return dynamic_pointer_cast<Player>(list[0]);
}