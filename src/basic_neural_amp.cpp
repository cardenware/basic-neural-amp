#include <iostream>
#include <filesystem>
#include <string>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "NeuralAudio/NeuralModel.h"
#include "audio_engine/audio_engine.h"

struct AudioParameters{
    std::atomic<bool> bypass{false};
    std::atomic<float> recommendedOutputdBModel{1.0f};
    std::atomic<float> masterVolume{1.0f};
};

AudioParameters audioParameters;

void clearScreen() {
    // \033[H moves the cursor to the top-left home position
    // \033[2J clears the entire screen
    std::cout << "\033[H\033[2J" << std::flush;
}

void createVolumeBar() {
    std::cout <<"[";
    for (int i = 0; i < 10; ++i) {
        char symbol = (i < (audioParameters.masterVolume.load(std::memory_order_relaxed) * 10.0f)) ? '#' : '-';
        std::cout << symbol;
    }
    std::cout << "]: " << static_cast<int>(std::lround(audioParameters.masterVolume.load(std::memory_order_relaxed) * 100.0f)) << "%" << std::endl << std::endl;
}

void showMenu() {
    std::cout << "========================================" << std::endl;
    std::cout << "             BASIC NEURAL AMP           " << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    std::cout << "AMP + CAB" << std::endl;
    std::cout << "5150 Stealth 100w Red Mesa OS - jp_is_out_of_tune.nam" << std::endl << std::endl;

    std::cout << "MASTER VOLUME" << std::endl;
    createVolumeBar();

    std::cout << "CONTROLS" << std::endl;

    std::string bypass = audioParameters.bypass.load(std::memory_order_relaxed) ? "On" : "Off";

    std::cout << "[1] Bypass: " << bypass << std::endl;
    std::cout << "[2] Increase volume +10%" << std::endl;
    std::cout << "[3] Decrease volume -10%" << std::endl;
    std::cout << "[4] Quit." << std::endl << std::endl;
    std::cout << "Your choice: ";
}

std::filesystem::path parseNAMFilePath(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Missing required option: --nam-file" << std::endl;
        return std::filesystem::path{};
    }

    std::string namFileFlag = argv[1];

    if (std::string(argv[1]) != "--nam-file") {
        std::cerr << "Unkown flag: " << "namFileFlag" << std::endl;
        return std::filesystem::path{};
    }
    else {
        std::filesystem::path namFilePath = argv[2];
        return namFilePath;
    }
}

int main(int argc, char** argv) {
    std::filesystem::path namFilePath = parseNAMFilePath(argc, argv);

    if (namFilePath.empty()) {
        return 1;
    }
    AudioEngine audioEngine;

    // MODEL
    std::cout << "Loading model: " << namFilePath.string() << "..." << std::endl;
    NeuralAudio::NeuralModelLoader loader;
    NeuralAudio::NeuralModel* model = loader.CreateFromFile(namFilePath.string().c_str());

    std::cout << "Model loaded successfully." << std::endl;
    if (!model) {
        std::cerr << "Failed to load model." << std::endl;
        return 2;
    }
    std::cout << "Model version: " << model->GetModelVersion() << std::endl << std::endl;

    if (model->HasQualityScaling()) {
        model->SetQualityScaleFactor(0.5f);
    }

    audioParameters.recommendedOutputdBModel.store(model->GetRecommendedOutputDBAdjustment());

    // AUDIO

    std::cout << "========================================" << std::endl;
    std::cout << "                AUDIO SETUP             " << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    audioEngine.enumerateDevices();

    unsigned int  inputDeviceIdx = 0, outputDeviceIdx = 0;

    std::cout << "\nYour input device: ";
    std::cin >> inputDeviceIdx;

    std::cout << "Your output device: ";
    std::cin >> outputDeviceIdx;
    
    audioEngine.setProcessor(
        [model](
            const float* input,
            float* output,
            unsigned int frameCount) {
            /*
                NeuralAudio works with mono buffers.
                The model processes:
                input[0 ... frameCount-1]
                write:
                output[0 ... frameCount-1]
            */
            std::vector<float> inputCopy(input, input + frameCount);
            std::vector<float> processed(frameCount);
            
            // dB = 20 * log10(A/A0); A = amplitude, A0 = reference amplitude

            if (audioParameters.bypass.load(std::memory_order_relaxed)) {
                processed = inputCopy;
            }
            else {
                model->Process(
                    inputCopy.data(),
                    processed.data(),
                    static_cast<int>(frameCount)
                );
            }
            
            // Calculate outputGain (MASTER VOLUME)
           const float outputGain = std::pow(10.0f, audioParameters.recommendedOutputdBModel.load(std::memory_order_relaxed) / 20.0f) * ((audioParameters.masterVolume.load(std::memory_order_relaxed) * 100.0f) / 100.0f);
            
           // mono -> stereo
            for (unsigned int i = 0;
                 i < frameCount;
                 ++i) {
                output[i * 2 + 0] =
                    processed[i] * outputGain;

                output[i * 2 + 1] =
                    processed[i] * outputGain;
            }
        }
    );

    bool isRunning = true;
    audioEngine.start(inputDeviceIdx, outputDeviceIdx);

    char option;

    do {
        clearScreen();
        showMenu();

        std::cin >> option;

        switch (option) {
            case '1':
                audioParameters.bypass.store(
                    !audioParameters.bypass.load(std::memory_order_relaxed),
                    std::memory_order_relaxed
                );
                break;
            case '2': {
                float currentVolume = audioParameters.masterVolume.load(std::memory_order_relaxed);
                
                audioParameters.masterVolume.store(
                    std::min(1.0f, currentVolume + 0.1f),
                    std::memory_order_relaxed
                );
                break;
            }
            case '3': {
                float currentVolume = audioParameters.masterVolume.load(std::memory_order_relaxed);
                
                audioParameters.masterVolume.store(
                    std::max(0.0f, currentVolume - 0.1f),
                    std::memory_order_relaxed
                );
                break;
            }
            case '4':
                isRunning = false;
                break;
            default:
                break;
        }
    } while(isRunning);

    return 0;
}
