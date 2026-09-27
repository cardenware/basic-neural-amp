#pragma once

#include <iostream>
#include <string>

#include "../../../common/types.h"
#include "../../../audio_processor/chain/chain.h"
#include "../base.h"

class RemoveProcessorScreen : public BaseScreen {
    public:
        explicit RemoveProcessorScreen(AudioChain& audioChain)
            : m_audioChain(audioChain) {}

        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "           REMOVE PROCESSOR           " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            m_audioChain.list();

            std::cout << std::endl;
            std::cout << "[B] Back" << std::endl << std::endl;
            std::cout << "Choose the processor to remove: " << std::flush;
        }

        MenuAction read() override {
            std::string input;
            std::cin >> input;

            if (input == "b" || input == "B") {
                return MenuAction::Back;
            }

            try {
                const int index = std::stoi(input);
                if (index >= 1) {
                    m_audioChain.removeProcessorByIndex(index - 1);
                    return MenuAction::Back;
                }
            } catch (...) {
                return MenuAction::None;
            }

            return MenuAction::None;
        }

    private:
        AudioChain& m_audioChain;
};
