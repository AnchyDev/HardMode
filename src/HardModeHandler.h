#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "DifficultyMode.h"
#include "Player.h"
#include <vector>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    bool IsModeEnabled(Player* player, uint8 mode);
    void SetTainted(Player* player, bool value);
    bool GetTainted(Player* player);
    bool IsTainted(Player* player);
    std::string GetNameFromMode(uint8 mode);
    PlayerSettingMap* GetPlayerSettingsFromDatabase(ObjectGuid guid);

    static HardModeHandler* GetInstance();

public:
    DifficultyMode* Modes[DifficultyModes::DIFFICULTY_MODE_COUNT];
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
