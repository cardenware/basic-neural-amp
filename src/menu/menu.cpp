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
        case MenuType::AudioSetup:
            showAudioSetupMenu();
            break;
        case MenuType::ModelSelection:
            showModelSelectionMenu();
            break;
        default:
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
            return readMainMenuAction(_getch());        
        case MenuType::AudioSetup:
            return readAudioSetupAction();
        case MenuType::ModelSelection:
            return readModelSelectionAction();
        default:
            return MenuAction::None;
    }
}

void Menu::showAudioSetupMenu() {
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

void Menu::showModelSelectionMenu() {
    std::string prevFolder = "";

    std::cout << "========================================" << std::endl;
    std::cout << "              MODEL SELECTION           " << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    for (size_t i = 0; i < m_modelAvailables.size(); ++i) {
        std::filesystem::path entry = m_modelAvailables[i];
        std::string parentFolder = entry.parent_path().filename().string();

        if (prevFolder != parentFolder) { // Folder has changed
            std::cout << "- " << parentFolder << "/" << std::endl;
            prevFolder = parentFolder;
        }
        
        std::cout << "\t[" << i << "] " << entry.filename().string() << std::endl;
    }

    std::cout << "[B] Back" << std::endl << std::endl;
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
    std::cout << (m_appState->modelName.empty() ? "None": m_appState->modelName) << std::endl << std::endl;

    std::cout << "MASTER VOLUME" << std::endl;
    createVolumeBar();

    std::cout << "CONTROLS" << std::endl;

    std::string bypass = m_appState->bypass.load(std::memory_order_relaxed) ? "On" : "Off";
    std::string recordingState = m_appState->isRecording.load(std::memory_order_relaxed) ? "Recording" : "Stopped";

    std::cout << "[1] Select model" << std::endl; 
    std::cout << "[2] Bypass: " << bypass << std::endl;
    std::cout << "[3] Increase volume +10%" << std::endl;
    std::cout << "[4] Decrease volume -10%" << std::endl;
    std::cout << "[5] Start/Stop record: " << recordingState << std::endl;
    std::cout << "[6] Quit" << std::endl << std::endl;
    std::cout << "Press a number to select an option." << std::flush;
}

MenuAction Menu::readMainMenuAction(char option) {
    switch (option) {
        case '1': return MenuAction::SelectModel;
        case '2': return MenuAction::ToggleBypass;
        case '3': return MenuAction::IncreaseVolume;
        case '4': return MenuAction::DecreaseVolume;
        case '5': return MenuAction::ToggleRecording;
        case '6': return MenuAction::Quit;
        default:  return MenuAction::None;
    }
}

MenuAction Menu::readModelSelectionAction() {
    std::cout << "Enter a model number, or press B to go back: ";

    std::string input;
    std::cin >> input;

    if (input == "b" || input == "B") {
        return MenuAction::Back;
    }

    try {
        std::size_t position = 0;
        int index = std::stoi(input, &position);

        if (position == input.size() ||
            index >= 0 ||
            static_cast<std::size_t>(index) <= m_modelAvailables.size()
        ) {
            m_selectedModel = m_modelAvailables[index];
        }
    }
    catch (...) {
        m_selectedModel = std::filesystem::path{};
        return MenuAction::None;
    }
    return MenuAction::None;
}

MenuAction Menu::readAudioSetupAction() {
    std::string input;
    std::size_t position;
    int index;

    std::cout << "Enter an input device number: ";
    std::cin >> input;
    
    try {
        position = 0;
        index = std::stoi(input, &position);

        if (position == input.size() ||
            index >= 0 ||
            static_cast<std::size_t>(index) <= m_modelAvailables.size()
        ) {
            m_inputDeviceIndex = index;
        }
    }
    catch (...) {
        return MenuAction::None;
    }

    std::cout << "Enter an output device number: ";
    std::cin >> input;

    try {
        position = 0;
        index = std::stoi(input, &position);

        if (position == input.size() ||
            index >= 0 ||
            static_cast<std::size_t>(index) <= m_modelAvailables.size()
        ) {
            m_outputDeviceIndex = index;
        }
    }
    catch (...) {
        return MenuAction::None;
    }

    return MenuAction::None;
}
