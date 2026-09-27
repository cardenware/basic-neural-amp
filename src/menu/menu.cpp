#include "menu.h"
#include "conio.h"

Menu::Menu(AppState& appState, AudioEngine& audioEngine, AudioChain& audioChain)
    :   m_mainScreen(appState), 
        m_audioSetupScreen(audioEngine),
        m_audioChainScreen(audioChain),
        m_addProcessorScreen(audioChain),
        m_editProcessorScreen(audioChain),
        m_removeProcessorScreen(audioChain),
        m_reorderProcessorScreen(audioChain) {
            m_selectedNeuralAdapter = nullptr;
        }

Menu::~Menu() {}

void Menu::show() {
    MenuType currentMenu = m_stack.back();

    switch (currentMenu)
    {
        case MenuType::MainMenu:
            m_mainScreen.show();
            break;
        case MenuType::AudioSetup:
            m_audioSetupScreen.show();
            break;
        case MenuType::AudioChain:
            m_audioChainScreen.show();
            break;
        case MenuType::ProcessorAdd:
            m_addProcessorScreen.show();
            break;
        case MenuType::ProcessorEdit:
            m_editProcessorScreen.show();
            break;
        case MenuType::ProcessorRemove:
            m_removeProcessorScreen.show();
            break;
        case MenuType::ProcessorReorder:
            m_reorderProcessorScreen.show();
            break;
        case MenuType::NeuralModelAdapterEdit:
            if (m_selectedNeuralAdapter) {
                EditNeuralModelAdapterScreen screen(*m_selectedNeuralAdapter);
                screen.show();
            }
            break;
        case MenuType::NeuralModelSelector:
            if (m_selectedNeuralAdapter) {
                NeuralModelSelectorScreen screen(*m_selectedNeuralAdapter);
                screen.show();
            }
            break;
        default:
            pop();
            m_mainScreen.show();
            break;
    }
}

MenuAction Menu::read() {
    MenuType currentMenu = m_stack.back();
    MenuAction action = MenuAction::None;

    switch (currentMenu) {
        case MenuType::MainMenu:
            action = m_mainScreen.read();
            break;
        case MenuType::AudioSetup:
            action = m_audioSetupScreen.read();
            break;
        case MenuType::AudioChain:
            action = m_audioChainScreen.read();
            break;
        case MenuType::ProcessorAdd:
            action = m_addProcessorScreen.read();
            break;
        case MenuType::ProcessorEdit: {
            action = m_editProcessorScreen.read();
            m_selectedNeuralAdapter = m_editProcessorScreen.getProcessorSelected<NeuralModelAdapter>();

            if (m_selectedNeuralAdapter) {
                m_stack.push_back(MenuType::NeuralModelAdapterEdit);
            }
            break;
        }
        case MenuType::ProcessorRemove:
            action = m_removeProcessorScreen.read();
            break;
        case MenuType::ProcessorReorder:
            action = m_reorderProcessorScreen.read();
            break;
        case MenuType::NeuralModelAdapterEdit: {
            if (m_selectedNeuralAdapter) {
                EditNeuralModelAdapterScreen screen(*m_selectedNeuralAdapter);
                action = screen.read();

                if (action == MenuAction::Back) {
                    m_selectedNeuralAdapter = nullptr;
                    pop();
                }
            }
            break;
        }
        case MenuType::NeuralModelSelector:
            if (m_selectedNeuralAdapter) {
                NeuralModelSelectorScreen screen(*m_selectedNeuralAdapter);
                action = screen.read();

                if (action == MenuAction::Back) {
                    pop();
                }
            }
            break;
        default:
            break;
    }

    return action;
}
