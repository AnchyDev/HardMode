#include "HardModeHandler.h"
#include "HardModeReward.h"

#include "Tokenize.h"
#include "Player.h"
#include "StringConvert.h"
#include "DBCStore.h"

void HardModeHandler::LoadRewardsFromDatabase()
{
    _hardModeRewards.clear();

    QueryResult qResult = WorldDatabase.Query("SELECT `mode`, `reward_type`, `reward_id`, `reward_count` FROM `hardmode_rewards`");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();

            uint32 mode = fields[0].Get<uint32>();
            uint32 rewardType = fields[1].Get<uint32>();
            uint32 rewardId = fields[2].Get<uint32>();
            uint32 rewardCount = fields[3].Get<uint32>();

            HardModeReward reward;

            reward.Mode = mode;
            reward.Type = rewardType;
            reward.RewardId = rewardId;
            reward.RewardCount = rewardCount;

            _hardModeRewards.push_back(reward);
            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'hardmode_rewards' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'hardmode_rewards' table.");
    }
}

void HardModeHandler::RewardItem(Player* player, uint32 itemId, uint32 itemCount)
{
    player->AddItem(itemId, itemCount);
}

void HardModeHandler::RewardTitle(Player* player, uint32 titleId)
{
    auto titleEntry = sCharTitlesStore.LookupEntry(titleId);
    player->SetTitle(titleEntry);
}

void HardModeHandler::RewardSpell(Player* player, uint32 spellId)
{
    player->learnSpell(spellId);
}

void HardModeHandler::RewardPlayerForMode(Player* player, uint8 mode)
{
    std::vector<HardModeReward> rewards = GetRewardsForMode(mode);

    for (auto const& reward : rewards)
    {
        switch (reward.Type)
        {
        case HardModeRewardType::HARDMODE_REWARD_TYPE_ITEM:
            RewardItem(player, reward.RewardId, reward.RewardCount);
            break;

        case HardModeRewardType::HARDMODE_REWARD_TYPE_TITLE:
            RewardTitle(player, reward.RewardId);
            break;

        case HardModeRewardType::HARDMODE_REWARD_TYPE_SPELL:
            RewardSpell(player, reward.RewardId);
            break;
        }
    }
}

std::vector<HardModeReward> HardModeHandler::GetRewardsForMode(uint8 mode)
{
    std::vector<HardModeReward> rewards;

    for (uint32 i = 0; i < _hardModeRewards.size(); ++i)
    {
        auto reward = _hardModeRewards[i];

        if (reward.Mode == mode)
        {
            rewards.push_back(reward);
        }
    }

    return rewards;
}

bool HardModeHandler::IsModeEnabled(Player* player, uint8 mode)
{
    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

void HardModeHandler::SetTainted(Player* player, bool value)
{
    player->UpdatePlayerSetting("HardModeTainted", 0, value);
}

bool HardModeHandler::IsTainted(Player* player)
{
    return player->GetPlayerSetting("HardModeTainted", 0).value > 0;
}

void HardModeHandler::SetShadowBanned(Player* player, bool value)
{
    player->UpdatePlayerSetting("HardModeShadowBanned", 0, value);
}

bool HardModeHandler::IsShadowBanned(Player* player)
{
    return player->GetPlayerSetting("HardModeShadowBanned", 0).value > 0;
}

std::string HardModeHandler::GetNameFromMode(uint8 mode)
{
    switch (mode)
    {
    case DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED:
        return "Self-Crafted";
    case DifficultyModes::DIFFICULTY_MODE_HARDCORE:
        return "HardCore";
    }

    return "ERROR";
}

std::string HardModeHandler::GetNamesFromEnabledModes(Player* player)
{
    std::stringstream ss;
    std::vector<std::string> modes;

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabled(player, i))
        {
            modes.push_back(sHardModeHandler->GetNameFromMode(i));
        }
    }

    for (uint8 i = 0; i < modes.size(); ++i)
    {
        ss << modes[i];

        if (i != modes.size() - 1)
        {
            ss << ", ";
        }
    }

    return ss.str();
}

bool HardModeHandler::HasModesEnabled(Player* player)
{
    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabled(player, i))
        {
            return true;
        }
    }

    return false;
}

std::string HardModeHandler::GetConfigNameFromMode(uint8 mode)
{
    switch (mode)
    {
    case DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED:
        return "HardMode.EnableSelfCrafted";
    case DifficultyModes::DIFFICULTY_MODE_HARDCORE:
        return "HardMode.EnableHardCore";
    }

    return "ERROR";
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

HardModeHandler* HardModeHandler::GetInstance()
{
    static HardModeHandler instance;

    return &instance;
}
