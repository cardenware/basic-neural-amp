#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "../../../common/types.h"
#include "../../../audio_processor/chain/chain.h"
#include "../base.h"

class ReorderProcessorScreen : public BaseScreen {
    public:
        explicit ReorderProcessorScreen(AudioChain& audioChain)
            : m_audioChain(audioChain) {}

        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "          REORDER PROCESSOR           " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            m_audioChain.list();

            std::cout << std::endl;
            std::cout << "[B] Back" << std::endl << std::endl;
            std::cout << "Enter the current position and the target position separated by a comma (e.g. 2,4): " << std::flush;
        }

        MenuAction read() override {
            std::string input;
            std::getline(std::cin, input);

            if (input == "b" || input == "B") {
                return MenuAction::Back;
            }

            std::stringstream ss(input);
            std::string token1, token2;

            std::getline(ss, token1, ',');
            std::getline(ss, token2);

            try {
                int src = std::stoi(token1) - 1;
                int dst = std::stoi(token2) - 1;
                
                m_audioChain.swap(src, dst);

                return MenuAction::Back;
            } catch (...) {
                return MenuAction::None;
            }
        }

    private:
        AudioChain& m_audioChain;
};
