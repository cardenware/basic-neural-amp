#pragma once

#include <string>
#include "miniaudio.h"

typedef struct {
    ma_device_id id;
    std::string name;
} Device;

typedef struct {
    std::string modelName;
    float recommendedOutputdB;
    bool bypass;
    float masterVolume;
} AmpParameters;

enum class MenuType {
    AudioSetup,
    MainMenu,
    AudioChain,
    ProcessorAdd,
    ProcessorRemove,
    ProcessorEdit,
    ProcessorReorder,
    NeuralModelAdapterEdit,
    NeuralModelSelector
};

enum class MenuAction {
    None,
    OpenAudioChain,
    OpenProcessorAdd,
    OpenProcessorRemove,
    OpenProcessorEdit,
    OpenProcessorReorder,
    OpenNeuralModelAdapterEdit,
    OpenNeuralModelSelector,
    ToggleRecording,
    Back,
    Quit
};

enum class FilterType {
    LowPass,
    HighPass,
    Peaking
};
