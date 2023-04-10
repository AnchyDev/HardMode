#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "HardModeTypes.h"

#include "Player.h"

#include <vector>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    void LoadHardModes();
    void ClearHardModes();
    std::vector<HardModeInfo>* GetHardModes();

    bool IsModeEnabledForPlayer(Player* player, uint8 mode);
    void UpdateModeForPlayer(Player* player, uint8 mode, bool state);
    bool IsPlayerTainted(Player* player);
    void UpdatePlayerTainted(Player* player, bool state);
    bool IsPlayerShadowBanned(Player* player);
    void UpdatePlayerShadowBanned(Player* player, bool state);
    std::string GetNamesFromEnabledModes(Player* player);
    std::string GetNameFromMode(uint8 mode);

private:
    std::vector<HardModeInfo> _hardModes;

public:
    static HardModeHandler* GetInstance()
    {
        static HardModeHandler instance;

        return &instance;
    }
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
