#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "common/app_state.h"
#include "neural_models/model_catalog/model_catalog.h"
#include "menu/menu.h"
#include "NeuralAudio/NeuralModel.h"
#include "audio_engine/audio_engine.h"

AppState appState;

std::filesystem::path parseNAMFilePath(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "No .nam file was provided. Skipping model loading..." << std::endl;
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

void setActiveModel(
    NeuralAudio::NeuralModelLoader& loader,
    std::atomic<std::shared_ptr<NeuralAudio::NeuralModel>>& activeModel,
    std::filesystem::path& modelPath
) {
    std::cout << "Loading activeModel: " << modelPath.filename().string() << "..." << std::endl;
    std::shared_ptr<NeuralAudio::NeuralModel> replacement{
        loader.CreateFromFile(modelPath)
    };

    if (!replacement) {
        std::cerr << "Couldn't load model: " << modelPath << std::endl;
        return;
    }

    activeModel.store(replacement, std::memory_order_release);
    auto model = activeModel.load(std::memory_order_acquire);

    std::cout << "activeModel loaded successfully." << std::endl;
    std::cout << "activeModel version: " << model->GetModelVersion() << std::endl << std::endl;

    appState.recommendedOutputdBModel.store(
        model->GetRecommendedOutputDBAdjustment()
    );
}

int main(int argc, char** argv) {
    AudioEngine audioEngine;
    ModelCatalog modelCatalog;
    Menu menu(&appState);
    menu.setModelAvailables(modelCatalog.get());
    
    std::filesystem::path namFilePath = parseNAMFilePath(argc, argv);

    NeuralAudio::NeuralModelLoader loader;
    std::atomic<std::shared_ptr<NeuralAudio::NeuralModel>> activeModel;
    if (!namFilePath.empty()) {
        appState.modelName = namFilePath.stem().string();
        setActiveModel(
            loader,
            activeModel,
            namFilePath
        );
    }
    
    audioEngine.setProcessor(
        [&activeModel](

            const float* input,
            float* output,
            unsigned int frameCount) {
            /*
                NeuralAudio works with mono buffers.
                The activeModel processes:
                input[0 ... frameCount-1]
                write:
                output[0 ... frameCount-1]
            */

            std::vector<float> inputCopy(input, input + frameCount);
            std::vector<float> processed(frameCount);
            
            auto model = activeModel.load(std::memory_order_acquire);
            if (appState.bypass.load(std::memory_order_relaxed) || !model) {
                processed = inputCopy;
            }
            else {
                model->Process(
                    inputCopy.data(),
                    processed.data(),
                    static_cast<int>(frameCount)
                );
            }
            
            // dB = 20 * log10(A/A0); A = amplitude, A0 = reference amplitude
            // Calculate outputGain (MASTER VOLUME)
            float currentVolume = appState.masterVolume.load(std::memory_order_relaxed);
            float recommendedOutputdB = appState.recommendedOutputdBModel.load(std::memory_order_relaxed);
            const float outputGain = std::pow(
                10.0f, 
                recommendedOutputdB / 20.0f) * ((currentVolume * 100.0f) / 100.0f
            );

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
    menu.setMenuType(MenuType::AudioSetup);
    menu.setDeviceAvailables(audioEngine.getDevices());
    menu.show();
    menu.readAction();

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
            case MenuAction::SelectModel: {
                menu.setMenuType(MenuType::ModelSelection);
                menu.show();
                action = menu.readAction();
                
                if (action == MenuAction::Back) {
                    menu.setMenuType(MenuType::MainMenu);
                    break;
                }

                std::filesystem::path selectedModel = menu.getSelectedModel();

                if (!selectedModel.empty()) {
                    appState.modelName = selectedModel.filename().string();
                    setActiveModel(loader, activeModel, selectedModel);
                }
                
                menu.setMenuType(MenuType::MainMenu);
                break;
            }
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
