#pragma once

#include <iostream>
#include <string>

#include "../../../common/types.h"
#include "../../../audio_processor/neural_model_adapter/neural_model_adapter.h"
#include "../../../neural_models/model_catalog/model_catalog.h"
#include "../base.h"

class NeuralModelSelectorScreen : public BaseScreen {
    public:
        explicit NeuralModelSelectorScreen(NeuralModelAdapter& neuralModelAdapter)
            : m_neuralModelAdapter(neuralModelAdapter) {}

        void show() override {
            std::string prevFolder = "";

            std::cout << "========================================" << std::endl;
            std::cout << "              MODEL SELECTION           " << std::endl;
            std::cout << "========================================" << std::endl << std::endl;
            
            const auto modelAvailables = m_modelCatalog.get();

            for (size_t i = 0; i < modelAvailables.size(); ++i) {
                std::filesystem::path entry = modelAvailables[i];
                std::string parentFolder = entry.parent_path().filename().string();

                if (prevFolder != parentFolder) { // Folder has changed
                    std::cout << "- " << parentFolder << "/" << std::endl;
                    prevFolder = parentFolder;
                }
                
                std::cout << "\t[" << i << "] " << entry.filename().string() << std::endl;
            }

            std::cout << "[B] Back" << std::endl << std::endl;

            std::cout << "Enter a number to select an option: ";
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
                    std::filesystem::path modelSelected = m_modelCatalog.get()[index];
                    m_neuralModelAdapter.setActiveModel(modelSelected);

                    return MenuAction::Back;
                }
            } catch (...) {
                return MenuAction::None;
            }

            return MenuAction::None;
        }

    private:
        NeuralModelAdapter& m_neuralModelAdapter;
        ModelCatalog m_modelCatalog;
};
