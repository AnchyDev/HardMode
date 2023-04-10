#ifndef MODULE_HARDMODE_HANDLER_H
#define MODULE_HARDMODE_HANDLER_H

#include "HardModeTypes.h"

#include <vector>

class HardModeHandler
{
private:
    HardModeHandler() { }

public:
    void LoadHardModes();
    void ClearHardModes();
    std::vector<HardModeInfo>* GetHardModes();

private:
    std::vector<HardModeInfo> _hardModes;

public:
    static HardModeHandler* GetInstance()
    {
        static HardModeHandler instance;

        return &instance;
    }
};

#define sHardModeHandler HardModeHandler::GetInstance()

#endif // MODULE_HARDMODE_HANDLER_H
