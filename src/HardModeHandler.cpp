#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Player.h"
#include "StringConvert.h"
#include "Tokenize.h"

#include <sstream>

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
            mode.Id = id - 1; /* PlayerSettings are 0 indexed, hardmode_modes is not. */
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

void HardModeHandler::LoadAuras()
{
    QueryResult qResult = WorldDatabase.Query("SELECT `mode`, `aura` FROM `hardmode_auras`");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();

            uint32 mode = fields[0].Get<uint32>();
            uint32 aura = fields[1].Get<uint32>();

            auto it = _auras.find(mode);
            if (it == _auras.end())
            {
                std::vector<uint32> auras;
                _auras.emplace(mode, auras);

                it = _auras.find(mode);
                if (it == _auras.end())
                {
                    LOG_ERROR("module", "An error occurred when trying to populate auras map!");
                    return;
                }
            }

            it->second.push_back(aura);
            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'hardmode_auras' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'hardmode_auras' table.");
    }
}

void HardModeHandler::ClearAuras()
{
    _auras.clear();
}

std::map<uint8, std::vector<uint32>>* HardModeHandler::GetAuras()
{
    return &_auras;
}

std::vector<uint32>* HardModeHandler::GetAurasForMode(uint8 mode)
{
    auto auras = sHardModeHandler->GetAuras();

    auto auraIt = auras->find(mode);
    if (auraIt != auras->end())
    {
        return &(auraIt->second);
    }

    return nullptr;
}

void HardModeHandler::ValidatePlayerAuras(Player* player)
{
    if (!player)
    {
        return;
    }

    auto modes = sHardModeHandler->GetHardModes();

    if (!modes)
    {
        return;
    }

    if (modes->size() < 1)
    {
        return;
    }

    for (auto modeIt = modes->begin(); modeIt != modes->end(); ++modeIt)
    {
        auto mode = modeIt->second.Id;
        auto auras = sHardModeHandler->GetAurasForMode(mode);
        
        if (!auras)
        {
            continue;
        }

        for (auto aura : *auras)
        {
            if (sHardModeHandler->IsModeEnabledForPlayer(player, mode))
            {
                if (!player->HasAura(aura))
                {
                    _scheduler.Schedule(1s, [aura, player](TaskContext /*task*/)
                     {
                            if (!player || !aura)
                            {
                                return;
                            }

                            player->AddAura(aura, player);
                     });
                }
            }
            else
            {
                if (player->HasAura(aura))
                {
                    _scheduler.Schedule(1s, [aura, player](TaskContext /*task*/)
                    {
                        if (!player || !aura)
                        {
                            return;
                        }

                        player->RemoveAura(aura);
                    });
                }
            }
        }
    }
}

void HardModeHandler::LoadRewards()
{
    QueryResult qResult = WorldDatabase.Query("SELECT `mode`, `reward_level`, `reward_type`, `reward_id`, `reward_count` FROM `hardmode_rewards`");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();

            uint32 mode = fields[0].Get<uint32>();
            uint32 level = fields[1].Get<uint32>();
            uint32 rewardType = fields[2].Get<uint32>();
            uint32 rewardId = fields[3].Get<uint32>();
            uint32 rewardCount = fields[4].Get<uint32>();

            HardModeReward reward;

            reward.Mode = mode;
            reward.Level = level;
            reward.Type = rewardType;
            reward.RewardId = rewardId;
            reward.RewardCount = rewardCount;

            auto it = _rewards.find(mode);
            if (it == _rewards.end())
            {
                std::vector<HardModeReward> rewards;
                _rewards.emplace(mode, rewards);

                it = _rewards.find(mode);
                if (it == _rewards.end())
                {
                    LOG_ERROR("module", "An error occurred when trying to populate rewards map!");
                    return;
                }
            }

            it->second.push_back(reward);
            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'hardmode_rewards' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'hardmode_rewards' table.");
    }
}

void HardModeHandler::ClearRewards()
{
    _rewards.clear();
}

std::map<uint8, std::vector<HardModeReward>>* HardModeHandler::GetRewards()
{
    return &_rewards;
}

std::vector<HardModeReward>* HardModeHandler::GetRewardsForMode(uint8 mode)
{
    auto it = _rewards.find(mode);
    if (it != _rewards.end())
    {
        return &it->second;
    }

    return nullptr;
}

void HardModeHandler::TryRewardPlayer(Player* player, std::vector<HardModeReward> rewards)
{
    for (auto reward : rewards)
    {
        switch (reward.Type)
        {
        case HardModeRewardType::HARDMODE_REWARD_TYPE_TITLE:
            RewardTitle(player, reward.RewardId);
            break;

        case HardModeRewardType::HARDMODE_REWARD_TYPE_SPELL:
            RewardSpell(player, reward.RewardId);
            break;
        }
    }

    RewardItems(player, rewards);
}

void HardModeHandler::RewardItems(Player* player, std::vector<HardModeReward> rewards)
{
    std::vector<std::pair<uint32, uint32>> mailItems;
    uint8 mode = 0;

    for (auto const& reward : rewards)
    {
        if (reward.Type == HardModeRewardType::HARDMODE_REWARD_TYPE_ITEM)
        {
            mailItems.push_back(std::make_pair(reward.RewardId, reward.RewardCount));
            mode = reward.Mode;
        }
    }

    if (mailItems.size() < 1)
    {
        return;
    }

    std::string hardModeName = sHardModeHandler->GetNameFromMode(mode);
    std::string header = Acore::StringFormatFmt("{} Rewards", hardModeName);
    std::string body = Acore::StringFormatFmt("Congratulations on reaching level {} on {} mode, enjoy your rewards!", player->GetLevel(), hardModeName);

    SendMailItems(player, mailItems, header, body);
}

void HardModeHandler::RewardTitle(Player* player, uint32 titleId)
{
    auto titleEntry = sCharTitlesStore.LookupEntry(titleId);

    if (!titleEntry)
    {
        LOG_ERROR("module", "No title with entry '{}' found while rewarding player '{}'.", titleId, player->GetName());
        return;
    }

    player->SetTitle(titleEntry);
}

void HardModeHandler::RewardSpell(Player* player, uint32 spellId)
{
    auto spellEntry = sSpellStore.LookupEntry(spellId);

    if (!spellEntry)
    {
        LOG_ERROR("module", "No spell with entry '{}' found while rewarding player '{}'.", spellId, player->GetName());
        return;
    }

    player->learnSpell(spellId);
}

void HardModeHandler::SendMailItems(Player* player, std::vector<std::pair<uint32, uint32>>& mailItems, std::string header, std::string body)
{
    using SendMailTempateVector = std::vector<std::pair<uint32, uint32>>;

    std::vector<SendMailTempateVector> allItems;

    auto AddMailItem = [&allItems](uint32 itemEntry, uint32 itemCount)
    {
        SendMailTempateVector toSendItems;

        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemEntry);
        if (!itemTemplate)
        {
            LOG_ERROR("entities.player.items", "> HardModeHandler::SendMailItems: Item id {} is invalid", itemEntry);
            return;
        }

        if (itemCount < 1 || (itemTemplate->MaxCount > 0 && itemCount > static_cast<uint32>(itemTemplate->MaxCount)))
        {
            LOG_ERROR("entities.player.items", "> HardModeHandler::SendMailItems: Incorrect item count ({}) for item id {}", itemCount, itemEntry);
            return;
        }

        while (itemCount > itemTemplate->GetMaxStackSize())
        {
            if (toSendItems.size() <= MAX_MAIL_ITEMS)
            {
                toSendItems.emplace_back(itemEntry, itemTemplate->GetMaxStackSize());
                itemCount -= itemTemplate->GetMaxStackSize();
            }
            else
            {
                allItems.emplace_back(toSendItems);
                toSendItems.clear();
            }
        }

        toSendItems.emplace_back(itemEntry, itemCount);
        allItems.emplace_back(toSendItems);
    };

    for (auto& [itemEntry, itemCount] : mailItems)
    {
        AddMailItem(itemEntry, itemCount);
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    MailSender sender(MAIL_CREATURE, HARDMODE_MAIL_SENDER);
    MailDraft draft(header, body);

    for (auto const& items : allItems)
    {
        for (auto const& [itemEntry, itemCount] : items)
        {
            if (Item* mailItem = Item::CreateItem(itemEntry, itemCount))
            {
                mailItem->SaveToDB(trans);
                draft.AddItem(mailItem);
            }
        }
    }

    draft.SendMailTo(trans, MailReceiver(player, player->GetGUID().GetCounter()), sender);

    CharacterDatabase.CommitTransaction(trans);
}

void HardModeHandler::SendAlert(Player* player, std::string message)
{
    WorldPacket data(SMSG_NOTIFICATION, (message.size() + 1));
    data << message;

    player->SendDirectMessage(&data);
}

bool HardModeHandler::IsModeEnabledForPlayer(Player* player, uint8 mode)
{
    if (!player)
    {
        return false;
    }

    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

bool HardModeHandler::IsModeEnabledForPlayerSettings(PlayerSettingMap* settingMap, uint8 mode)
{
    if (!settingMap)
    {
        return false;
    }

    auto itr = settingMap->find("HardMode");
    if (itr == settingMap->end())
    {
        return false;
    }


    auto setting = itr->second;
    if (setting.size() < (mode + 1))
    {
        return false;
    }

    return setting[mode].value > 0;
}

void HardModeHandler::UpdateModeForPlayer(Player* player, uint8 mode, bool state)
{
    if (!player)
    {
        return;
    }

    player->UpdatePlayerSetting("HardMode", mode, state);
    sHardModeHandler->ValidatePlayerAuras(player);
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

bool HardModeHandler::OfflinePlayerHasRestriction(PlayerSettingMap* settingMap, uint32 restriction)
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

        if (!sHardModeHandler->IsModeEnabledForPlayerSettings(settingMap, mode.Id))
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

std::vector<HardModeInfo> HardModeHandler::GetPlayerModesFromRestriction(Player* player, uint32 restriction)
{
    auto modes = sHardModeHandler->GetHardModes();
    std::vector<HardModeInfo> enabledModes;

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
            enabledModes.push_back(mode);
        }
    }

    return enabledModes;
}

std::vector<HardModeInfo> HardModeHandler::GetOfflinePlayerModesFromRestriction(PlayerSettingMap* settingMap, uint32 restriction)
{
    auto modes = sHardModeHandler->GetHardModes();
    std::vector<HardModeInfo> enabledModes;

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

        if (!sHardModeHandler->IsModeEnabledForPlayerSettings(settingMap, mode.Id))
        {
            continue;
        }

        auto rMask = (1 << restriction);
        bool hasRestriction = (mode.Restrictions & rMask) == rMask;

        if (hasRestriction)
        {
            enabledModes.push_back(mode);
        }
    }

    return enabledModes;
}

std::string HardModeHandler::GetDelimitedModes(std::vector<HardModeInfo> modes, std::string delimiter)
{
    std::stringstream ss;

    for (uint8 i = 0; i < modes.size(); ++i)
    {
        ss << modes[i].Name;

        if (i != modes.size() - 1)
        {
            ss << delimiter;
        }
    }

    return ss.str();
}

bool HardModeHandler::HasMatchingModesWithRestriction(Player* player, Player* target, uint32 restriction)
{
    auto hardModes = sHardModeHandler->GetHardModes();

    for (auto it = hardModes->begin(); it != hardModes->end(); ++it)
    {
        auto mode = it->second;

        if (!mode.Enabled)
        {
            continue;
        }

        if (!sHardModeHandler->ModeHasRestriction(mode.Id, restriction))
        {
            continue;
        }

        bool flag1 = (sHardModeHandler->IsModeEnabledForPlayer(player, mode.Id));
        bool flag2 = (sHardModeHandler->IsModeEnabledForPlayer(target, mode.Id));

        if (flag1 != flag2)
        {
            return false;
        }
    }

    return true;
}

bool HardModeHandler::ModeHasRestriction(uint8 mode, uint32 restriction)
{
    auto modes = sHardModeHandler->GetHardModes();
    auto modeIt = modes->find(mode);

    if (modeIt == modes->end())
    {
        return false;
    }

    auto rMask = (1 << restriction);
    bool hasRestriction = (modeIt->second.Restrictions & rMask) == rMask;

    return hasRestriction;
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

    if (state)
    {
        if (!player->HasAura(HARDMODE_AURA_SHADOWBAN))
        {
            player->AddAura(HARDMODE_AURA_SHADOWBAN, player);
        }
    }
    else
    {
        if (player->HasAura(HARDMODE_AURA_SHADOWBAN))
        {
            player->RemoveAura(HARDMODE_AURA_SHADOWBAN);
        }
    }
}

void HardModeHandler::TryShadowBanPlayer(Player* player)
{
    sHardModeHandler->UpdatePlayerShadowBanned(player, true);

    WorldLocation worldLoc(HARDMODE_AREA_AZSHARACRATER, -614.38, -239.69, 379.35, 0.69); // Azshara Crater / Shadow Tomb
    player->TeleportTo(worldLoc);
    player->SetHomebind(worldLoc, HARDMODE_AREA_SHADOWTOMB);

    if (!player->IsAlive())
    {
        player->ResurrectPlayer(100, false);
        player->RemoveCorpse();
    }
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

PlayerSettingMap* HardModeHandler::GetPlayerSettingsFromDatabase(ObjectGuid guid)
{
    PlayerSettingMap* settingMap = new PlayerSettingMap();

    auto result = CharacterDatabase.Query("SELECT source, data FROM character_settings WHERE guid = {}", guid.GetRawValue());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            std::string source = fields[0].Get<std::string>();
            std::string data = fields[1].Get<std::string>();

            std::vector<std::string_view> tokens = Acore::Tokenize(data, ' ', false);

            PlayerSettingVector setting;
            setting.resize(tokens.size());

            uint32 count = 0;

            for (auto token : tokens)
            {
                if (token.empty())
                {
                    continue;
                }

                PlayerSetting set;
                set.value = Acore::StringTo<uint32>(token).value();
                setting[count] = set;
                ++count;
            }

            (*settingMap)[source] = setting;

        } while (result->NextRow());
    }

    return settingMap;
}

TaskScheduler* HardModeHandler::GetScheduler()
{
    return &_scheduler;
}
