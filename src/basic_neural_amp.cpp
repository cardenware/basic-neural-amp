#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <atomic>
#include <algorithm>

#include "common/types.h"
#include "common/app_state.h"
#include "menu/menu.h"

#include "audio_engine/audio_engine.h"

#include "audio_processor/chain/chain.h"

AppState appState;

int main(int argc, char** argv) {
    AudioEngine audioEngine;

    auto audioChain = std::make_shared<AudioChain>();

    Menu menu(appState, audioEngine, *audioChain);

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
    menu.push(MenuType::AudioSetup);
    menu.show();

    menu.read();

    menu.push(MenuType::MainMenu);

    MenuAction menuAction;
    appState.isRunning = true; 
    
    do {
        appState.audioChain = audioChain->getAudioChainStr();

        menu.show();
        menuAction = menu.read();

        switch(menuAction) {
            case MenuAction::OpenAudioChain:
                menu.push(MenuType::AudioChain);
                break;
            case MenuAction::OpenProcessorAdd:
                menu.push(MenuType::ProcessorAdd);
                break;
            case MenuAction::OpenProcessorEdit:
                menu.push(MenuType::ProcessorEdit);
                break;
            case MenuAction::OpenProcessorRemove:
                menu.push(MenuType::ProcessorRemove);
                break;
            case MenuAction::OpenProcessorReorder:
                menu.push(MenuType::ProcessorReorder);
                break;
            case MenuAction::OpenNeuralModelAdapterEdit:
                menu.push(MenuType::NeuralModelAdapterEdit);
                break;
            case MenuAction::OpenNeuralModelSelector:
                std::cout << "OPEN MODEL SELECTOR" << std::endl;
                menu.push(MenuType::NeuralModelSelector);
                break;
            case MenuAction::ToggleRecording:
                break;
            case MenuAction::Back:
                menu.pop();
                break;
            case MenuAction::Quit:
                appState.isRunning = false;
                break;
            default:
                break;
        }
    } while(appState.isRunning);

    return 0;
}
