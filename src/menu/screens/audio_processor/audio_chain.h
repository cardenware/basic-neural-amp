#pragma once

#include <iostream>
#include "../../../common/types.h"
#include "../../../audio_processor/chain/chain.h"
#include "../base.h"

class AudioChainScreen : public BaseScreen {
    public:
        AudioChainScreen(AudioChain& audioChain) : m_audioChain(audioChain) {}
        ~AudioChainScreen() {}
        
        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "                AUDIO CHAIN             " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;
            
            std::cout << "CHAIN" << std::endl << std::endl;
            std::cout << 
                (m_audioChain.getAudioChainStr().empty() ? "Not configured": m_audioChain.getAudioChainStr()) << 
                std::endl <<
                std::endl;
            
            std::cout << "[1] Add processor" << std::endl;
            std::cout << "[2] Edit processor" << std::endl;
            std::cout << "[3] Remove processor" << std::endl;
            std::cout << "[4] Swap processor" << std::endl;

            std::cout << "\n[B] Back" << std::endl << std::endl;

            std::cout << "Enter a number to select an option or B to go back: ";
        }

        MenuAction read() override {
            std::cin >> m_choice;
            
            MenuAction action = MenuAction::None;

            if (m_choice == "b" || m_choice == "B") {
                action = MenuAction::Back;
            }
            else if (m_choice == "1") {
                action = MenuAction::OpenProcessorAdd;
            }
            else if (m_choice == "2") {
                action = MenuAction::OpenProcessorEdit;
            }
            else if (m_choice == "3") {
                action = MenuAction::OpenProcessorRemove;
            }
            else if (m_choice == "4") {
                action = MenuAction::OpenProcessorReorder;
            }

            return action;
        }
    private:
        AudioChain& m_audioChain;
};