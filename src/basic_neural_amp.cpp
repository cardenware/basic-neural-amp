#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "common/app_state.h"
#include "menu/menu.h"
#include "NeuralAudio/NeuralModel.h"
#include "audio_engine/audio_engine.h"

AudioEngine audioEngine;
AppState appState;
Menu menu(&appState);

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

    appState.modelName = namFilePath.stem().string();

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
        model->SetQualityScaleFactor(0.75f);
    }

    appState.recommendedOutputdBModel.store(model->GetRecommendedOutputDBAdjustment());
    
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

            if (appState.bypass.load(std::memory_order_relaxed)) {
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
           const float outputGain = std::pow(10.0f, appState.recommendedOutputdBModel.load(std::memory_order_relaxed) / 20.0f) * ((appState.masterVolume.load(std::memory_order_relaxed) * 100.0f) / 100.0f);

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

    // AUDIO SETUP
    std::vector<std::vector<Device>> devices = audioEngine.getDevices();

    menu.setMenuType(MenuType::AudioSetup);
    menu.setDeviceAvailables(&devices);
    menu.show();
    
    std::vector<int> selectedDevices = menu.getSelectedDeviceIndices();
    audioEngine.start(selectedDevices[0], selectedDevices[1]);

    menu.setMenuType(MenuType::MainMenu);
    MenuAction action;
    bool isRunning = true;    

    do {
        menu.clear();
        menu.show();

        action = menu.readAction();

        switch (action) {
            case MenuAction::ToggleBypass:
                appState.bypass.store(
                    !appState.bypass.load(std::memory_order_relaxed),
                    std::memory_order_relaxed
                );
                break;
            case MenuAction::IncreaseVolume: {
                float currentVolume = appState.masterVolume.load(std::memory_order_relaxed);
                
                appState.masterVolume.store(
                    std::min(1.0f, currentVolume + 0.1f),
                    std::memory_order_relaxed
                );
                break;
            }
            case MenuAction::DecreaseVolume: {
                float currentVolume = appState.masterVolume.load(std::memory_order_relaxed);
                
                appState.masterVolume.store(
                    std::max(0.0f, currentVolume - 0.1f),
                    std::memory_order_relaxed
                );
                break;
            }
            case MenuAction::ToggleRecording: {
                bool isRecording = !appState.isRecording.load(
                    std::memory_order_relaxed
                );

                appState.isRecording.store(
                    isRecording,
                    std::memory_order_relaxed
                );

                if (isRecording) {
                    audioEngine.startRecord();
                }
                else {
                    audioEngine.stopRecord();
                }
                break;
            }
            case MenuAction::Quit:
                isRunning = false;
                break;
            default:
                break;
        }
    } while(isRunning);

    return 0;
}
