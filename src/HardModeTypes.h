#ifndef MODULE_HARDMODE_TYPES_H
#define MODULE_HARDMODE_TYPES_H

#include "Common.h"

enum HardModeConstants
{
    HARDMODE_MAIL_SENDER = 441102
};

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
    HARDMODE_RESTRICT_NONE =                  0, // Used internally, DO NOT USE
    HARDMODE_RESTRICT_RETAIL_XP =             1, // 2: The player can only receive 1x exp rates from kills and quests.
    HARDMODE_RESTRICT_SELFCRAFTED =           2, // 4: The player can only use self-crafted items (armor, weapons, consumables, etc..)
    HARDMODE_RESTRICT_INTERACT_AUCTION =      3, // 8: The player cannot interact with the auction house.
    HARDMODE_RESTRICT_INTERACT_GUILDBANK =    4, // 16: The player cannot interact with the guild bank.
    HARDMODE_RESTRICT_INTERACT_MAIL_SEND =    5, // 32: The player cannot send mail.
    HARDMODE_RESTRICT_INTERACT_MAIL_RECEIVE = 6, // 64: The player cannot receive mail.
    HARDMODE_RESTRICT_PERMADEATH =            7, // 128: The player is sent to the shadow tomb on death.
    HARDMODE_RESTRICT_COUNT =                 8, // Used internally. DO NOT USE
};

enum HardModeRewardType
{
    HARDMODE_REWARD_TYPE_ITEM = 0,
    HARDMODE_REWARD_TYPE_TITLE = 1,
    HARDMODE_REWARD_TYPE_SPELL = 2
};

struct HardModeReward
{
    uint32 Mode;
    uint32 Level;
    uint32 Type;
    uint32 RewardId;
    uint32 RewardCount;
};

#endif // MODULE_HARDMODE_TYPES_H
