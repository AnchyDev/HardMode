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
    HARDMODE_RESTRICT_COUNT = 2, // Used internally. DO NOT USE
};

#endif // MODULE_HARDMODE_TYPES_H
