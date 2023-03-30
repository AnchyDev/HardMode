#include "HardModeHandler.h"
#include "Player.h"

bool HardModeHandler::IsModeEnabled(Player* player, uint8 mode)
{
    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

std::string HardModeHandler::GetNameFromMode(uint8 mode)
{
    switch (mode)
    {
    case DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED:
        return "Self-Crafted";
    }

    return "ERROR";
}

HardModeHandler* HardModeHandler::GetInstance()
{
    static HardModeHandler instance;

    return &instance;
}
