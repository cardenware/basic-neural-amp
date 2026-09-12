#include "menu.h"
#include "conio.h"

Menu::Menu(AppState* appState) { m_menuType = MenuType::AudioSetup; m_appState = appState; }

Menu::~Menu() {}

void Menu::show() {
    clear();
    
    switch (m_menuType)
    {
        case MenuType::MainMenu:
            showMainMenu();
            break;
        
        default:
            showAudioSetupMenu();
            break;
    }
}

void Menu::clear() {
    // \033[H moves the cursor to the top-left home position
    // \033[2J clears the entire screen
    std::cout << "\033[H\033[2J" << std::flush;
}

MenuAction Menu::readAction() {
    switch (m_menuType)
    {
        case MenuType::MainMenu:
            return readMainMenuOption(_getch());        
        case MenuType::AudioSetup:
            readAudioSetupOption();
            return MenuAction::None;
        default:
            return MenuAction::None;
    }
}

void Menu::showAudioSetupMenu() {
    std::cout << "========================================" << std::endl;
    std::cout << "                AUDIO SETUP             " << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    std::cout << "INPUT DEVICE(S)" << std::endl;

    for (std::size_t i = 0; i < (*m_deviceAvailables)[0].size(); ++i) {
        std::cout << "\t[" << i << "] " << (*m_deviceAvailables)[0][i].name << std::endl;
    }
    
    std::cout << "OUTPUT DEVICE(S)" << std::endl;

    for (std::size_t i = 0; i < (*m_deviceAvailables)[1].size(); ++i) {
        std::cout << "\t[" << i << "] " << (*m_deviceAvailables)[1][i].name << std::endl;
    }
    
    readAction();
}

void Menu::createVolumeBar() {
std::cout <<"[";
for (int i = 0; i < 10; ++i) {
    char symbol = (
        i < (m_appState->masterVolume.load(std::memory_order_relaxed) * 10.0f)
    ) ? '#' : '-';
    std::cout << symbol;
}
std::cout << "]: " << static_cast<int>(
    std::lround(m_appState->masterVolume.load(std::memory_order_relaxed) * 100.0f)
) << "%" << std::endl << std::endl;
}

void Menu::showMainMenu() {
    std::cout << "========================================" << std::endl;
    std::cout << "             BASIC NEURAL AMP           " << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    std::cout << "AMP + CAB" << std::endl;
    std::cout << m_appState->modelName << std::endl << std::endl;

    std::cout << "MASTER VOLUME" << std::endl;
    createVolumeBar();

    std::cout << "CONTROLS" << std::endl;

    std::string bypass = m_appState->bypass.load(std::memory_order_relaxed) ? "On" : "Off";
    std::string recordingState = m_appState->isRecording.load(std::memory_order_relaxed) ? "Recording" : "Stopped";

    std::cout << "[1] Bypass: " << bypass << std::endl;
    std::cout << "[2] Increase volume +10%" << std::endl;
    std::cout << "[3] Decrease volume -10%" << std::endl;
    std::cout << "[4] Start/Stop record: " << recordingState << std::endl;
    std::cout << "[5] Quit." << std::endl << std::endl;
    std::cout << "Press a number to select an option." << std::flush;
}

MenuAction Menu::readMainMenuOption(char option) {
    switch (option) {
        case '1': return MenuAction::ToggleBypass;
        case '2': return MenuAction::IncreaseVolume;
        case '3': return MenuAction::DecreaseVolume;
        case '4': return MenuAction::ToggleRecording;
        case '5': return MenuAction::Quit;
        default:  return MenuAction::None;
    }
}

void Menu::readAudioSetupOption() {
    std::cout << "Select your device input: ";
    std::cin >> m_inputDeviceIndex;

    std::cout << "Select your device output: ";
    std::cin >> m_outputDeviceIndex;
}
