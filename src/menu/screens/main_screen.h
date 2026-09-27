#pragma once

#include <iostream>
#include <conio.h>

#include "../../common/types.h"
#include "../../common/app_state.h"
#include "base.h"

class MainScreen : public BaseScreen {
    public:
        MainScreen(AppState& appState) : m_appState(appState) {}
        ~MainScreen() {}

        void show() override {
            clear();

            std::cout << "========================================" << std::endl;
            std::cout << "             BASIC NEURAL AMP           " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            std::cout << "AUDIO CHAIN" << std::endl << std::endl;
            std::cout << (m_appState.audioChain.empty() ? "Not configured": m_appState.audioChain) << std::endl << std::endl;

            std::string recordingState = m_appState.isRecording.load(std::memory_order_relaxed) ? "Recording" : "Stopped";

            std::cout << "[1] Edit audio chain" << std::endl; 
            std::cout << "[2] Start/Stop record: " << recordingState << std::endl;
            std::cout << "[3] Quit" << std::endl << std::endl;
            std::cout << "Enter a number to select an option: ";
        }


        MenuAction read() override {
            std::cin >> m_choice;

            if (m_choice == "1") {
                return MenuAction::OpenAudioChain;
            }
            else if (m_choice == "2") {
                return MenuAction::ToggleRecording;
            }
            else if (m_choice == "3") {
                return MenuAction::Quit;
            }
            else {
                return MenuAction::None;
            }
        }

    private:
        AppState& m_appState;
};
