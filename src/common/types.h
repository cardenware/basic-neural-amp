#pragma once

#include <string>
#include "miniaudio.h"

typedef struct {
    ma_device_id id;
    std::string name;
} Device;

enum class MenuType {
    AudioSetup,
    ModelSelection,
    MainMenu
};

enum class MenuAction {
    None,
    SelectModel,
    ToggleBypass,
    IncreaseVolume,
    DecreaseVolume,
    ToggleRecording,
    Back,
    Quit
};
