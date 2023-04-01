#include "HardMode.h"
#include "HardModeHandler.h"
#include "HardModeReward.h"

#include "Tokenize.h"
#include "Player.h"
#include "StringConvert.h"
#include "DBCStore.h"
#include "Mail.h"
#include "Config.h"

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
            LOG_ERROR("entities.player.items", "> HardModeHandler::SendMailItems: Incorrect item count ({}) for item id {}", itemEntry, itemCount);
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
    std::string body = Acore::StringFormatFmt("Congratulations on reaching max level on {}, enjoy your rewards!", hardModeName);

    SendMailItems(player, mailItems, header, body);
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

    // Reward non-item rewards, items are handled differently.
    for (auto const& reward : rewards)
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
    case DifficultyModes::DIFFICULTY_MODE_SLOWXP:
        return "SlowXP";
    }

    return "ERROR";
}

std::string HardModeHandler::GetNamesFromEnabledModes(Player* player)
{
    std::stringstream ss;
    std::vector<std::string> modes;

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
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
        if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
        {
            return true;
        }
    }

    return false;
}

bool HardModeHandler::IsModeEnabledForPlayer(Player* player, uint8 mode)
{
    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

bool HardModeHandler::IsModeEnabledForPlayerAndServer(Player* player, uint8 mode)
{
    if (!sHardModeHandler->IsModeEnabledForPlayer(player, mode))
    {
        return false;
    }

    if (!sConfigMgr->GetOption<bool>(sHardModeHandler->GetConfigNameFromMode(mode), false))
    {
        return false;
    }

    return true;
}

uint32 HardModeHandler::GetEnabledModesAsMask(Player* player)
{
    uint32 mask = 0;

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            mask += (1 << i);
        }
    }

    return mask;
}

std::string HardModeHandler::GetConfigNameFromMode(uint8 mode)
{
    switch (mode)
    {
    case DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED:
        return "HardMode.EnableSelfCrafted";
    case DifficultyModes::DIFFICULTY_MODE_HARDCORE:
        return "HardMode.EnableHardCore";
    case DifficultyModes::DIFFICULTY_MODE_SLOWXP:
        return "HardMode.EnableSlowXP";
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
