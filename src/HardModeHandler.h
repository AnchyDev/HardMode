#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "DifficultyMode.h"
#include "HardModeReward.h"

#include "Player.h"
#include <vector>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    bool IsModeEnabled(Player* player, uint8 mode);
    void SetTainted(Player* player, bool value);
    bool IsTainted(Player* player);
    void SetShadowBanned(Player* player, bool value);
    bool IsShadowBanned(Player* player);

    std::string GetNameFromMode(uint8 mode);
    std::string GetNamesFromEnabledModes(Player* player);
    bool HasModesEnabled(Player* player);
    std::string GetConfigNameFromMode(uint8 mode);
    PlayerSettingMap* GetPlayerSettingsFromDatabase(ObjectGuid guid);

    void LoadRewardsFromDatabase();
    std::vector<HardModeReward> GetRewardsForMode(uint8 mode);
    void RewardPlayerForMode(Player* player, uint8 mode);
    void RewardItems(Player* player, std::vector<HardModeReward> rewards);
    void RewardTitle(Player* player, uint32 titleId);
    void RewardSpell(Player* player, uint32 spellId);
    void SendMailItems(Player* player, std::vector<std::pair<uint32, uint32>>& mailItems, std::string header, std::string body);

    static HardModeHandler* GetInstance();

public:
    DifficultyMode* Modes[DifficultyModes::DIFFICULTY_MODE_COUNT];

private:
    std::vector<HardModeReward> _hardModeRewards;
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
