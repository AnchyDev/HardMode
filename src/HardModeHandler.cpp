#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"

bool HardModeHandler::IsHardModeEnabled()
{
    return sConfigMgr->GetOption<bool>("HardMode.Enable", false);
}

void HardModeHandler::LoadHardModes()
{
    QueryResult qResult = WorldDatabase.Query("SELECT * FROM `hardmode_modes` ORDER BY id ASC");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();

            uint32 id = fields[0].Get<int32>();
            std::string name = fields[1].Get<std::string>();
            std::string description = fields[2].Get<std::string>();
            uint64 restrictions = fields[3].Get<uint64>();
            bool enabled = fields[4].Get<bool>();

            HardModeInfo mode;
            mode.Id = id;
            mode.Name = name;
            mode.Description = description;
            mode.Restrictions = restrictions;
            mode.Enabled = enabled;

            _hardModes.emplace(id, mode);

            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'hardmode_modes' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'hardmode_modes' table.");
    }
}

void HardModeHandler::ClearHardModes()
{
    _hardModes.clear();
}

std::map<uint8, HardModeInfo>* HardModeHandler::GetHardModes()
{
    return &_hardModes;
}

void HardModeHandler::LoadSelfCraftExcludeIds()
{
    QueryResult qResult = WorldDatabase.Query("SELECT `id` FROM `hardmode_selfcraft_exclude`");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();
            int32 id = fields[0].Get<int32>();

            _selfCraftExcludeIds.push_back(id);
            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'hardmode_selfcraft_exclude' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'hardmode_selfcraft_exclude' table.");
    }
}

void HardModeHandler::ClearSelfCraftExcludeIds()
{
    _selfCraftExcludeIds.clear();
}

std::vector<int32>* HardModeHandler::GetSelfCraftedExcludeIds()
{
    return &_selfCraftExcludeIds;
}

bool HardModeHandler::IsSelfCraftExcluded(int32 id)
{
    for (uint32 i = 0; i < _selfCraftExcludeIds.size(); ++i)
    {
        if (_selfCraftExcludeIds[i] == id)
        {
            return true;
        }
    }

    return false;
}

bool HardModeHandler::IsSelfCraftSpellExcluded(uint32 spellId)
{
    return IsSelfCraftExcluded(-spellId);

    return false;
}

bool HardModeHandler::IsSelfCraftItemExcluded(uint32 itemId)
{
    return IsSelfCraftExcluded(itemId);

    return false;
}

bool HardModeHandler::IsModeEnabledForPlayer(Player* player, uint8 mode)
{
    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

void HardModeHandler::UpdateModeForPlayer(Player* player, uint8 mode, bool state)
{
    player->UpdatePlayerSetting("HardMode", mode, state);
}

bool HardModeHandler::PlayerHasRestriction(Player* player, uint32 restriction)
{
    auto modes = sHardModeHandler->GetHardModes();
    for (auto it = modes->begin(); it != modes->end(); ++it)
    {
        auto mode = it->second;

        if (!mode.Enabled)
        {
            continue;
        }

        if (mode.Restrictions == HARDMODE_RESTRICT_NONE)
        {
            continue;
        }

        if (!sHardModeHandler->IsModeEnabledForPlayer(player, mode.Id))
        {
            continue;
        }

        auto rMask = (1 << restriction);
        bool hasRestriction = (mode.Restrictions & rMask) == rMask;

        if (hasRestriction)
        {
            return true;
        }
    }

    return false;
}

bool HardModeHandler::IsPlayerTainted(Player* player)
{
    return player->GetPlayerSetting("HardModeTainted", 0).value > 0;
}

void HardModeHandler::UpdatePlayerTainted(Player* player, bool state)
{
    player->UpdatePlayerSetting("HardModeTainted", 0, state);
}

bool HardModeHandler::IsPlayerShadowBanned(Player* player)
{
    return player->GetPlayerSetting("HardModeShadowBanned", 0).value > 0;
}

void HardModeHandler::UpdatePlayerShadowBanned(Player* player, bool state)
{
    player->UpdatePlayerSetting("HardModeShadowBanned", 0, state);
}

std::string HardModeHandler::GetNamesFromEnabledModes(Player* player)
{
    std::stringstream ss;
    std::vector<HardModeInfo> modes;

    auto hardModes = sHardModeHandler->GetHardModes();
    for (auto mode = hardModes->begin(); mode != hardModes->end(); ++mode)
    {
        if (sHardModeHandler->IsModeEnabledForPlayer(player, mode->second.Id))
        {
            modes.push_back(mode->second);
        }
    }

    for (uint8 i = 0; i < modes.size(); ++i)
    {
        ss << modes[i].Name;

        if (i != modes.size() - 1)
        {
            ss << ", ";
        }
    }

    return ss.str();
}

std::string HardModeHandler::GetNameFromMode(uint8 id)
{
    auto hardModes = sHardModeHandler->GetHardModes();

    auto mode = hardModes->find(id);

    if (mode != hardModes->end())
    {
        return mode->second.Name;
    }

    return "Unknown";
}
