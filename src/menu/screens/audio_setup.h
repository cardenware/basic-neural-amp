#pragma once

#include <iostream>
#include <vector>

#include "../../common/types.h"
#include "../../audio_engine/audio_engine.h"
#include "base.h"

class AudioSetupScreen : public BaseScreen {
    public:
        AudioSetupScreen(AudioEngine& audioEngine) : m_audioEngine(audioEngine) {}
        ~AudioSetupScreen() {}

        void show() override {
            clear();

            m_deviceAvailables = m_audioEngine.getDevices();

            std::cout << "========================================" << std::endl;
            std::cout << "                AUDIO SETUP             " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;

            std::cout << "INPUT DEVICE(S)" << std::endl;

            for (std::size_t i = 0; i < m_deviceAvailables[0].size(); ++i) {
                std::cout << "\t[" << i << "] " << m_deviceAvailables[0][i].name << std::endl;
            }
            
            std::cout << "OUTPUT DEVICE(S)" << std::endl;

            for (std::size_t i = 0; i < m_deviceAvailables[1].size(); ++i) {
                std::cout << "\t[" << i << "] " << m_deviceAvailables[1][i].name << std::endl;
            }
        }

        MenuAction read() override {
            std::string input;
            std::size_t position = 0;
            int index = 0;
            unsigned int inputDeviceIdx, outputDeviceIdx;

            std::cout << "Enter an input device number: ";
            std::cin >> input;

            try {
                position = 0;
                index = std::stoi(input, &position);

                if (position == input.size() &&
                    index >= 0 &&
                    static_cast<unsigned int>(index) < m_deviceAvailables[0].size()) {
                    inputDeviceIdx = index;
                } else {
                    return MenuAction::None;
                }
            } catch (...) {
                return MenuAction::None;
            }

            std::cout << "Enter an output device number: ";
            std::cin >> input;

            try {
                position = 0;
                index = std::stoi(input, &position);

                if (position == input.size() &&
                    index >= 0 &&
                    static_cast<unsigned int>(index) < m_deviceAvailables[1].size()) {
                    outputDeviceIdx = index;
                } else {
                    return MenuAction::None;
                }
            } catch (...) {
                return MenuAction::None;
            }

            m_audioEngine.start(inputDeviceIdx, outputDeviceIdx);

            return MenuAction::None;
        }

    private:
        AudioEngine& m_audioEngine;
        std::vector<std::vector<Device>> m_deviceAvailables;
};