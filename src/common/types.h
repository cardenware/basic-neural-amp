#pragma once

#include <string>
#include "miniaudio.h"

typedef struct {
    ma_device_id id;
    std::string name;
} Device;

enum class MenuType {
    AudioSetup,
    MainMenu
};

enum class MenuAction {
    None,
    SelectInput,
    SelectOutput,
    ToggleBypass,
    IncreaseVolume,
    DecreaseVolume,
    ToggleRecording,
    Quit
};
