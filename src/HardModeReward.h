#ifndef MODULE_HARDMODE_REWARD_H
#define MODULE_HARDMODE_REWARD_H

#include "Common.h"

enum HardModeRewardType
{
    HARDMODE_REWARD_TYPE_ITEM = 0,
    HARDMODE_REWARD_TYPE_TITLE = 1,
    HARDMODE_REWARD_TYPE_SPELL = 2
};

struct HardModeReward
{
    uint32 Mode;
    uint32 Type;
    uint32 RewardId;
    uint32 RewardCount;
};

#endif // MODULE_HARDMODE_REWARD_H
