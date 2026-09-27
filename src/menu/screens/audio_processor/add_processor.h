#pragma once

#include <iostream>
#include <string>

#include "../../../common/types.h"
#include "../../../audio_processor/chain/chain.h"
#include "../../../audio_processor/equalizer/equalizer.h"
#include "../../../audio_processor/neural_model_adapter/neural_model_adapter.h"
#include "../base.h"

class AddProcessorScreen : public BaseScreen {
    public:
        explicit AddProcessorScreen(AudioChain& audioChain)
            : m_audioChain(audioChain) {}

        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "             ADD PROCESSOR             " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            std::cout << "[1] Equalizer" << std::endl;
            std::cout << "[2] Neural model adapter" << std::endl;
            std::cout << "[B] Back" << std::endl << std::endl;
            std::cout << "Select an option: " << std::flush;
        }

        MenuAction read() override {
            MenuAction action = MenuAction::None;

            std::string input;
            std::cin >> input;
            
            if (input == "b" || input == "B") {
                action =  MenuAction::Back;
            }
            else if (input == "1") {
                m_audioChain.addProcessor<Equalizer>(48000.0f);
                action =  MenuAction::Back;
            }
            else if (input == "2") {
                m_audioChain.addProcessor<NeuralModelAdapter>();
                action =  MenuAction::Back;
            }

            return action;
        }

    private:
        AudioChain& m_audioChain;
};
