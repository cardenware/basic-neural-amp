#pragma once

#include <iostream>
#include <string>
#include <cmath>

#include "../../../common/types.h"
#include "../../../audio_processor/neural_model_adapter/neural_model_adapter.h"
#include "../base.h"

class EditNeuralModelAdapterScreen : public BaseScreen {
    public:
        explicit EditNeuralModelAdapterScreen(NeuralModelAdapter& neuralModelAdapter)
            : m_neuralModelAdapter(neuralModelAdapter) {}

        void show() override {
            clear();
            
            const AmpParameters& ampParameters = m_neuralModelAdapter.getAmpParameters();

            std::cout << "========================================" << std::endl;
            std::cout << "        EDIT NEURAL MODEL ADAPTER       " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            std::cout << "Model: " << (ampParameters.modelName.empty() ? "None" : ampParameters.modelName) << std::endl;
            std::cout << "Recommended output[dB]: " << ampParameters.recommendedOutputdB << std::endl;
            std::cout << "Bypass: " << m_neuralModelAdapter.getBypass() << std::endl;
            std::cout << "Master volume: " << ampParameters.masterVolume << std::endl << std::endl;

            std::cout << "[1] Select model" << std::endl;
            std::cout << "[2] Toggle bypass" << std::endl;
            std::cout << "[3] Increase volume (+10%)" << std::endl;
            std::cout << "[4] Decrease volume (-10%)" << std::endl;

            std::cout << std::endl;
            std::cout << "[B] Back" << std::endl << std::endl;
            std::cout << "Enter a number to select an option: ";
        }

        MenuAction read() override {
            MenuAction action = MenuAction::None;
            
            std::string input;
            std::cin >> input;

            if (input == "b" || input == "B") {
                return MenuAction::Back;
            }

            try {
                const AmpParameters& ampParameters = m_neuralModelAdapter.getAmpParameters();
                const int index = std::stoi(input);
                
                switch (index) {
                    case 1:
                        return MenuAction::OpenNeuralModelSelector;
                    case 2:
                        m_neuralModelAdapter.setBypass(!m_neuralModelAdapter.getBypass());
                        return MenuAction::None;
                    case 3: {
                        float currentVolume = std::min(1.0f, ampParameters.masterVolume + 0.1f);
                        m_neuralModelAdapter.setMasterVolume(currentVolume);
                        return MenuAction::None;
                    }
                    case 4: {
                        float currentVolume = std::max(0.0f, ampParameters.masterVolume - 0.1f);
                        m_neuralModelAdapter.setMasterVolume(currentVolume);
                        
                        return MenuAction::None;
                    }
                    default:
                        return MenuAction::None;
                }
            } catch (...) {
                return MenuAction::None;
            }

            return MenuAction::None;
        }

    private:
        NeuralModelAdapter& m_neuralModelAdapter;
};
