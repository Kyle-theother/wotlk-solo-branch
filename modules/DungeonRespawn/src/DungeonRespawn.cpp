#include "DungeonRespawn.h"

bool DSPlayerScript::IsInsideDungeonRaid(Player* player)
{
    if (!player)
        return false;

    Map* map = player->GetMap();
    if (!map)
        return false;

    return map->IsDungeon() || map->IsRaid();
}

void DSPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    if (!drEnabled || !IsInsideDungeonRaid(player))
        return;

    playersToTeleport.push_back(player->GetGUID());
}

void DSPlayerScript::ResurrectPlayer(Player* player)
{
    player->ResurrectPlayer(respawnHpPct / 100.0f, false);
    player->SpawnCorpseBones();
}

bool DSPlayerScript::OnPlayerBeforeTeleport(Player* player, uint32 mapid, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (!drEnabled || !player)
        return true;

    if (player->GetMapId() != mapid)
    {
        auto prData = GetOrCreateRespawnData(player);
        prData->isTeleportingNewMap = true;
    }

    if (!IsInsideDungeonRaid(player) || !player->isDead())
        return true;

    GuidVector::iterator itToRemove;
    bool canRestore = false;

    for (auto it = playersToTeleport.begin(); it != playersToTeleport.end(); ++it)
    {
        if (*it == player->GetGUID())
        {
            itToRemove = it;
            canRestore = true;
            break;
        }
    }

    if (!canRestore)
        return true;

    playersToTeleport.erase(itToRemove);

    auto prData = GetOrCreateRespawnData(player);
    if (!prData || prData->dungeon.map == -1 || prData->dungeon.map != int32(player->GetMapId()))
        return true;

    player->TeleportTo(prData->dungeon.map, prData->dungeon.x, prData->dungeon.y, prData->dungeon.z, prData->dungeon.o);
    ResurrectPlayer(player);
    return false;
}

void DSWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        SaveRespawnData();
        respawnData.clear();
    }

    drEnabled = sConfigMgr->GetOption<bool>("DungeonRespawn.Enable", false);
    respawnHpPct = sConfigMgr->GetOption<float>("DungeonRespawn.RespawnHealthPct", 50.0f);

    QueryResult qResult = CharacterDatabase.Query("SELECT `guid`, `map`, `x`, `y`, `z`, `o` FROM `dungeonrespawn_playerinfo`");
    if (!qResult)
    {
        LOG_INFO("module", "Loaded '0' rows from 'dungeonrespawn_playerinfo' table.");
        return;
    }

    uint32 dataCount = 0;
    do
    {
        Field* fields = qResult->Fetch();
        PlayerRespawnData prData;
        prData.guid = ObjectGuid(fields[0].Get<uint64>());
        prData.dungeon.map = fields[1].Get<int32>();
        prData.dungeon.x = fields[2].Get<float>();
        prData.dungeon.y = fields[3].Get<float>();
        prData.dungeon.z = fields[4].Get<float>();
        prData.dungeon.o = fields[5].Get<float>();
        prData.isTeleportingNewMap = false;
        prData.inDungeon = false;
        respawnData.push_back(prData);
        dataCount++;
    } while (qResult->NextRow());

    LOG_INFO("module", "Loaded '{}' rows from 'dungeonrespawn_playerinfo' table.", dataCount);
}

void DSWorldScript::OnShutdown()
{
    SaveRespawnData();
}

void DSWorldScript::SaveRespawnData()
{
    for (const auto& prData : respawnData)
    {
        if (prData.inDungeon)
        {
            CharacterDatabase.Execute("INSERT INTO `dungeonrespawn_playerinfo` (guid, map, x, y, z, o) VALUES ({}, {}, {}, {}, {}, {}) ON DUPLICATE KEY UPDATE map={}, x={}, y={}, z={}, o={}",
                prData.guid.GetRawValue(), prData.dungeon.map, prData.dungeon.x, prData.dungeon.y, prData.dungeon.z, prData.dungeon.o,
                prData.dungeon.map, prData.dungeon.x, prData.dungeon.y, prData.dungeon.z, prData.dungeon.o);
        }
        else
            CharacterDatabase.Execute("DELETE FROM `dungeonrespawn_playerinfo` WHERE guid = {}", prData.guid.GetRawValue());
    }
}

PlayerRespawnData* DSPlayerScript::GetOrCreateRespawnData(Player* player)
{
    for (auto it = respawnData.begin(); it != respawnData.end(); ++it)
        if (player->GetGUID() == it->guid)
            return &(*it);

    CreateRespawnData(player);
    return GetOrCreateRespawnData(player);
}

void DSPlayerScript::OnPlayerMapChanged(Player* player)
{
    if (!player)
        return;

    auto prData = GetOrCreateRespawnData(player);
    if (!prData)
        return;

    bool inDungeon = IsInsideDungeonRaid(player);
    prData->inDungeon = inDungeon;
    if (!inDungeon || !prData->isTeleportingNewMap)
        return;

    prData->dungeon.map = player->GetMapId();
    prData->dungeon.x = player->GetPositionX();
    prData->dungeon.y = player->GetPositionY();
    prData->dungeon.z = player->GetPositionZ();
    prData->dungeon.o = player->GetOrientation();
    prData->isTeleportingNewMap = false;
}

void DSPlayerScript::CreateRespawnData(Player* player)
{
    PlayerRespawnData newPrData;
    newPrData.dungeon.map = -1;
    newPrData.dungeon.x = 0;
    newPrData.dungeon.y = 0;
    newPrData.dungeon.z = 0;
    newPrData.dungeon.o = 0;
    newPrData.guid = player->GetGUID();
    newPrData.isTeleportingNewMap = false;
    newPrData.inDungeon = false;
    respawnData.push_back(newPrData);
}

void DSPlayerScript::OnPlayerLogin(Player* player)
{
    if (player)
        GetOrCreateRespawnData(player);
}

void DSPlayerScript::OnPlayerLogout(Player* player)
{
    if (!player)
        return;

    for (auto it = playersToTeleport.begin(); it < playersToTeleport.end(); ++it)
    {
        if (player->GetGUID() == (*it))
        {
            playersToTeleport.erase(it);
            break;
        }
    }
}

void SC_AddDungeonRespawnScripts()
{
    new DSWorldScript();
    new DSPlayerScript();
}
