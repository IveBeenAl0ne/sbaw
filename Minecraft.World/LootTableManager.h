#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

struct LootTableConditionDefinition
{
    std::string conditionName;
    std::string entity;
    std::string propertyName;
    std::string propertyValue;
};

struct LootTableFunctionDefinition
{
    std::string functionName;
    int minCount = 0;
    int maxCount = 0;
    std::vector<LootTableConditionDefinition> conditions;
};

struct LootTableEntryDefinition
{
    std::string type;
    std::string name;
    int weight = 1;
    std::vector<LootTableFunctionDefinition> functions;
};

struct LootTablePoolConditionDefinition
{
    std::string conditionName;
    double chance = 0.0;
    double lootingMultiplier = 0.0;
};

struct LootTablePoolDefinition
{
    int rolls = 1;
    int minRolls = 1;
    int maxRolls = 1;
    std::vector<LootTableEntryDefinition> entries;
    std::vector<LootTablePoolConditionDefinition> conditions;
};

struct LootTableDefinition
{
    std::string path;
    std::vector<LootTablePoolDefinition> pools;
};

struct LootTableDropResult
{
    int itemId = 0;
    int count = 0;
};

class LootTableManager
{
public:
    static LootTableManager &Get();

    bool LoadFromDisk(const std::string &baseDirectory = "");
    bool HasLoadedTables() const;

    const LootTableDefinition *GetTable(const std::string &tableName) const;
    std::vector<LootTableDropResult> ResolveDrops(
        const std::string &tableName,
        bool wasKilledByPlayer,
        int playerBonusLevel,
        const std::function<int(int)> &randomInt,
        bool entityIsOnFire = false) const;

private:
    std::vector<LootTableDefinition> m_tables;
    std::unordered_map<std::string, size_t> m_tableIndexByPath;
    std::string m_rootDirectory;
    bool m_loaded = false;
};
