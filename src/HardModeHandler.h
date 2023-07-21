#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "HardModeTypes.h"

#include "Player.h"
#include "TaskScheduler.h"

#include <vector>
#include <map>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    bool IsHardModeEnabled();

    void LoadHardModes();
    void ClearHardModes();
    std::map<uint8, HardModeInfo>* GetHardModes();
    HardModeInfo* GetHardModeFromId(uint8 id);

    void LoadPlayerSettings();
    void LoadPlayerSettings(ObjectGuid guid);
    void ClearPlayerSettings();
    void SavePlayerSettings();
    void SavePlayerSetting(uint64 guid, HardModePlayerSettings* settings);
    void UpdatePlayerSettings(ObjectGuid guid, HardModePlayerSettings* settings);
    std::map<uint64, HardModePlayerSettings>* GetPlayerSettings();
    HardModePlayerSettings* GetPlayerSetting(ObjectGuid guid);

    void LoadSelfCraftExcludeIds();
    void ClearSelfCraftExcludeIds();
    std::vector<int32>* GetSelfCraftedExcludeIds();
    bool IsSelfCraftExcluded(int32 id);
    bool IsSelfCraftSpellExcluded(uint32 spellId);
    bool IsSelfCraftItemExcluded(uint32 itemId);

    void LoadAuras();
    void ClearAuras();
    std::map<uint8, std::vector<uint32>>* GetAuras();
    std::vector<uint32>* GetAurasForMode(uint8 mode);
    void ValidatePlayerAuras(Player* player);

    void UpdatePlayerScaleSpeed(Player* player, float scaleSpeed);

    void LoadRewards();
    void ClearRewards();
    std::map<uint8, std::vector<HardModeReward>>* GetRewards();
    std::vector<HardModeReward>* GetRewardsForMode(uint8 mode);
    void TryRewardPlayer(Player* player, std::vector<HardModeReward> rewards);
    void RewardItems(Player* player, std::vector<HardModeReward> rewards);
    void RewardTitle(Player* player, uint32 titleId);
    void RewardSpell(Player* player, uint32 spellId);
    void SendMailItems(Player* player, std::vector<std::pair<uint32, uint32>>& mailItems, std::string header, std::string body);

    void SendAlert(Player* player, std::string message);

    bool IsModeEnabledForPlayer(ObjectGuid guid, uint8 mode);
    void UpdateModeForPlayer(ObjectGuid guid, uint8 mode, bool state);
    bool PlayerHasRestriction(ObjectGuid guid, uint32 restriction);
    std::vector<HardModeInfo> GetPlayerModesFromRestriction(ObjectGuid guid, uint32 restriction);
    std::string GetDelimitedModes(std::vector<HardModeInfo> modes, std::string delimiter);
    bool HasMatchingModesWithRestriction(Player* player, Player* target, uint32 restriction);
    bool ModeHasRestriction(uint8 mode, uint32 restriction);

    bool IsPlayerTainted(ObjectGuid guid);
    void UpdatePlayerTainted(ObjectGuid guid, bool state);
    bool CanTaintPlayer(ObjectGuid guid);

    bool IsPlayerShadowBanned(ObjectGuid guid);
    void UpdatePlayerShadowBanned(ObjectGuid guid, bool state);
    void TryShadowBanPlayer(ObjectGuid guid);

    std::string GetNamesFromEnabledModes(Player* player);
    std::string GetNameFromMode(uint8 mode);

    PlayerSettingMap* GetPlayerSettingsFromDatabase(ObjectGuid guid);
    TaskScheduler* GetScheduler();

private:
    std::map<uint8, HardModeInfo> _hardModes;
    std::map<uint64, HardModePlayerSettings> _playerSettings;
    std::vector<int32> _selfCraftExcludeIds;
    std::map<uint8, std::vector<HardModeReward>> _rewards;
    std::map<uint8, std::vector<uint32>> _auras;
    TaskScheduler _scheduler;

public:
    static HardModeHandler* GetInstance()
    {
        static HardModeHandler instance;

        return &instance;
    }
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
