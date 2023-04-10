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
    HARDMODE_RESTRICT_NONE = 0,
    HARDMODE_RESTRICT_SELFCRAFT = 1, // The player can only use self-crafted items.
    HARDMODE_RESTRICT_COUNT = 2, // The restriction count, used internally. DO NOT USE
};

#endif // MODULE_HARDMODE_TYPES_H
