#include "HardModeHandler.h"

#include "Tokenize.h"
#include "Player.h"
#include "StringConvert.h"

bool HardModeHandler::IsModeEnabled(Player* player, uint8 mode)
{
    return player->GetPlayerSetting("HardMode", mode).value > 0;
}

void HardModeHandler::SetTainted(Player* player, bool value)
{
    player->UpdatePlayerSetting("HardModeTainted", 0, value);
}

bool HardModeHandler::GetTainted(Player* player)
{
    return player->GetPlayerSetting("HardModeTainted", 0).value > 0;
}

bool HardModeHandler::IsTainted(Player* player)
{
    if (GetTainted(player))
    {
        return true;
    }

    return false;
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

PlayerSettingMap* HardModeHandler::GetPlayerSettingsFromDatabase(ObjectGuid guid)
{
    PlayerSettingMap* settingMap = new PlayerSettingMap();

    auto result = CharacterDatabase.Query("SELECT source, data FROM character_settings WHERE guid = {}", guid.GetRawValue());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            std::string source = fields[0].Get<std::string>();
            std::string data = fields[1].Get<std::string>();

            std::vector<std::string_view> tokens = Acore::Tokenize(data, ' ', false);

            PlayerSettingVector setting;
            setting.resize(tokens.size());

            uint32 count = 0;

            for (auto token : tokens)
            {
                if (token.empty())
                {
                    continue;
                }

                PlayerSetting set;
                set.value = Acore::StringTo<uint32>(token).value();
                setting[count] = set;
                ++count;
            }

            (*settingMap)[source] = setting;

        } while (result->NextRow());
    }

    return settingMap;
}

HardModeHandler* HardModeHandler::GetInstance()
{
    static HardModeHandler instance;

    return &instance;
}
