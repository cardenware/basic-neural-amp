#pragma once

#include <iostream>
#include <string>

#include "../../../common/types.h"
#include "../../../audio_processor/chain/chain.h"
#include "../base.h"

class EditProcessorScreen : public BaseScreen {
    public:
        explicit EditProcessorScreen(AudioChain& audioChain)
            : m_audioChain(audioChain) { m_processorSelectedIndex = -1; }

        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "             EDIT PROCESSOR            " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            m_audioChain.list();

            std::cout << std::endl;
            std::cout << "[B] Back" << std::endl << std::endl;
            std::cout << "Select a processor to edit: " << std::flush;
        }

        template<typename T>
        T* getProcessorSelected() {
            if (m_processorSelectedIndex == -1) {
                return nullptr;
            }
            int auxIdx = m_processorSelectedIndex;
            m_processorSelectedIndex = -1;
            return m_audioChain.getProcessorByIndex<NeuralModelAdapter>(auxIdx);
        }

        MenuAction read() override {
            MenuAction action = MenuAction::None;
            
            std::string input;
            std::cin >> input;

            if (input == "b" || input == "B") {
                return MenuAction::Back;
            }

            try {
                const int index = std::stoi(input);
                if (index >= 1) {
                    m_processorSelectedIndex = index - 1;
                    return MenuAction::OpenNeuralModelAdapterEdit;
                }
            } catch (...) {
                return MenuAction::None;
            }

            return MenuAction::None;
        }

    private:
        AudioChain& m_audioChain;
        int m_processorSelectedIndex;
};
