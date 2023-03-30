#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "DifficultyMode.h"
#include <vector>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    bool IsModeEnabled(Player* player, uint8 mode);
    std::string GetNameFromMode(uint8 mode);

    static HardModeHandler* GetInstance();

public:
    DifficultyMode* Modes[DifficultyModes::DIFFICULTY_MODE_COUNT];
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
