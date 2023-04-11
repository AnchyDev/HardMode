#ifndef MODULE_HARDMODE_TYPES_H
#define MODULE_HARDMODE_TYPES_H

#include "Common.h"

struct HardModeInfo
{
    uint8 Id;
    std::string Name;
    std::string Description;
    uint64 Restrictions;
    bool Enabled;
};

enum HardModeRestrictions
{
    HARDMODE_RESTRICT_NONE = 0, // Used internally, DO NOT USE
    HARDMODE_RESTRICT_RETAIL_XP = 1, // 2: The player can only receive 1x exp rates from kills and quests.
    HARDMODE_RESTRICT_SELFCRAFTED = 2, // 4: The player can only use self-crafted items (armor, weapons, consumables, etc..)
    HARDMODE_RESTRICT_INTERACT_AUCTION = 3,// 8: The player cannot interact with the auction house.
    HARDMODE_RESTRICT_INTERACT_GUILDBANK = 4, // 16: The player cannot interact with the guild bank.
    HARDMODE_RESTRICT_COUNT = 5, // Used internally. DO NOT USE
};

#endif // MODULE_HARDMODE_TYPES_H
