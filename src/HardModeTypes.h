#ifndef MODULE_HARDMODE_TYPES_H
#define MODULE_HARDMODE_TYPES_H

#include "Common.h"

enum HardModeConstants
{
    HARDMODE_MAIL_SENDER = 441102,
    HARDMODE_AURA_SHADOWBAN = 45681,
    HARDMODE_AREA_AZSHARACRATER = 37,
    HARDMODE_AREA_SHADOWTOMB = 3888,
    HARDMODE_AREA_UNKNOWN = 4988
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
    HARDMODE_RESTRICT_NONE =                        0,  // Used internally, DO NOT USE
    HARDMODE_RESTRICT_RETAIL_XP =                   1,  // 2: The player can only receive 1x exp rates from kills, quests, etc..
    HARDMODE_RESTRICT_SELFCRAFTED =                 2,  // 4: The player can only use self-crafted items (armor, weapons, consumables, etc..)
    HARDMODE_RESTRICT_INTERACT_AUCTION =            3,  // 8: The player cannot interact with the auction house.
    HARDMODE_RESTRICT_INTERACT_GUILDBANK =          4,  // 16: The player cannot interact with the guild bank.
    HARDMODE_RESTRICT_INTERACT_MAIL_SEND =          5,  // 32: The player cannot send mail.
    HARDMODE_RESTRICT_INTERACT_MAIL_RECEIVE =       6,  // 64: The player cannot receive mail.
    HARDMODE_RESTRICT_INTERACT_TRADE =              7,  // 128: The player cannot trade/be traded.
    HARDMODE_RESTRICT_PERMADEATH =                  8,  // 256: The player is sent to the shadow tomb on death.
    HARDMODE_RESTRICT_HIDE_WHOLIST =                9,  // 512: The players location is hidden on the who list.
    HARDMODE_RESTRICT_HIDE_FRIENDS =                10, // 1024, The players location is hidden on the friends list.
    HARDMODE_RESTRICT_HIDE_GUILD =                  11, // 2048, The players location is hidden on the guild roster.
    HARDMODE_RESTRICT_INTERACT_LFG =                12, // 4096, The player cannot queue for LFG.
    HARDMODE_RESTRICT_INTERACT_GROUP =              13, // 8192, The player cannot join groups.
    HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY =    14, // 16384 The player can only group with matching modes.
    HARDMODE_RESTRICT_BAD_LUCK =                    15, // 32768 The player has bad luck with loot drops.
    HARDMODE_RESTRICT_PACIFIST =                    16, // 65536 The player loses the mode if they damage or kill units.
    HARDMODE_RESTRICT_COUNT =                       17,  // Used internally. DO NOT USE
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
