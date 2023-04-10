#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "HardModeTypes.h"

#include "Player.h"

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

    void LoadSelfCraftExcludeIds();
    void ClearSelfCraftExcludeIds();
    std::vector<int32>* GetSelfCraftedExcludeIds();
    bool IsSelfCraftExcluded(int32 id);
    bool IsSelfCraftSpellExcluded(uint32 spellId);
    bool IsSelfCraftItemExcluded(uint32 itemId);

    bool IsModeEnabledForPlayer(Player* player, uint8 mode);
    void UpdateModeForPlayer(Player* player, uint8 mode, bool state);
    bool PlayerHasRestriction(Player* player, uint32 restriction);
    bool IsPlayerTainted(Player* player);
    void UpdatePlayerTainted(Player* player, bool state);
    bool IsPlayerShadowBanned(Player* player);
    void UpdatePlayerShadowBanned(Player* player, bool state);
    std::string GetNamesFromEnabledModes(Player* player);
    std::string GetNameFromMode(uint8 mode);

private:
    std::map<uint8, HardModeInfo> _hardModes;
    std::vector<int32> _selfCraftExcludeIds;

public:
    static HardModeHandler* GetInstance()
    {
        static HardModeHandler instance;

        return &instance;
    }
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
