#pragma once

#include <atomic>
#include <string>

struct AppState{
    std::string modelName;
    std::atomic<bool> isRecording{false};
    std::atomic<bool> bypass{false};
    std::atomic<float> recommendedOutputdBModel{1.0f};
    std::atomic<float> masterVolume{1.0f};
};