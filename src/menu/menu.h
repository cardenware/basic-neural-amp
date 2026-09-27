#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include "../common/types.h"
#include "../common/app_state.h"
#include "../audio_engine/audio_engine.h"
#include "../audio_processor/chain/chain.h"
#include "../audio_processor/neural_model_adapter/neural_model_adapter.h"

#include "screens/main_screen.h"
#include "screens/audio_setup.h"
#include "screens/audio_processor/audio_chain.h"
#include "screens/audio_processor/add_processor.h"
#include "screens/audio_processor/edit_processor.h"
#include "screens/audio_processor/remove_processor.h"
#include "screens/audio_processor/reorder_processor.h"
#include "screens/audio_processor/edit_neural_model_adapter.h"
#include "screens/audio_processor/neural_model_selector.h"

class Menu {
    public:
        Menu(AppState&, AudioEngine&, AudioChain&);
        ~Menu();

        void show();

        void push(MenuType menuType) { m_stack.push_back(menuType); }
        void pop()  { m_stack.pop_back(); }

        MenuAction read();

    private:
        std::vector<MenuType> m_stack;

        AudioSetupScreen m_audioSetupScreen;
        MainScreen m_mainScreen;
        AudioChainScreen m_audioChainScreen;
        AddProcessorScreen m_addProcessorScreen;
        EditProcessorScreen m_editProcessorScreen;
        RemoveProcessorScreen m_removeProcessorScreen;
        ReorderProcessorScreen m_reorderProcessorScreen;
        
        NeuralModelAdapter* m_selectedNeuralAdapter;
};
