#pragma once

#include <atomic>
#include <string>

struct AppState{
    std::string audioChain;
    std::atomic<bool> isRecording{false};
};