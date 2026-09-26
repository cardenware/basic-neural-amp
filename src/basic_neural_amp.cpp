#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "common/types.h"
#include "common/app_state.h"
#include "neural_models/model_catalog/model_catalog.h"
#include "menu/menu.h"

#include "audio_engine/audio_engine.h"

#include "audio_processor/chain/chain.h"
#include "audio_processor/neural_model_adapter/neural_model_adapter.h"
#include "audio_processor/equalizer/equalizer.h"

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

int main(int argc, char** argv) {
    AudioEngine audioEngine;
    ModelCatalog modelCatalog;

    auto audioChain = std::make_shared<AudioChain>();

    std::size_t eqId = audioChain->addProcessor<Equalizer>(48000.0f);
    auto* eq = audioChain->getProcessor<Equalizer>(eqId);
    // Filter range -12dB -> +12dB
    eq->setBand(FilterType::HighPass, 40, 0.707, 0.0);
    eq->setBand(FilterType::Peaking, 90, 1.0, 0.0);   // bass
    eq->setBand(FilterType::Peaking, 650, 1.0, 0.0);    // mid
    eq->setBand(FilterType::Peaking, 2800, 1.0, 0.0);  // treble
    eq->setBand(FilterType::LowPass, 6000.0, 0.707, 0.0);

    std::size_t neuralAmpId = audioChain->addProcessor<NeuralModelAdapter>();
    auto* neuralAmp = audioChain->getProcessor<NeuralModelAdapter>(neuralAmpId);

    Menu menu(appState);
    
    menu.setModelAvailables(modelCatalog.get());
    
    std::filesystem::path namFilePath = parseNAMFilePath(argc, argv);

    if (!namFilePath.empty()) {
        neuralAmp->setActiveModel(namFilePath);
    }

    std::size_t maxInputFrames = 4096;
    auto processedSignal = std::make_shared<float[]>(maxInputFrames);

    audioEngine.setProcessor(
        [processedSignal, audioChain](
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
            std::copy(input, input + frameCount, processedSignal.get());
            
            // dB = 20 * log10(A/A0); A = amplitude, A0 = reference amplitude
            
            audioChain->process(processedSignal.get(), frameCount);

            // mono -> stereo
            for (unsigned int i = 0; i < frameCount; ++i) {
                // float sample = processedSignal.get()[i] * outputGain;
                float sample = processedSignal.get()[i];
                output[i * 2 + 0] = sample;
                output[i * 2 + 1] = sample;
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

    appState.audioChain = audioChain->getAudioChainStr();

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
                    neuralAmp->setActiveModel(selectedModel);
                }
                
                menu.setMenuType(MenuType::MainMenu);
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
