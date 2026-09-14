#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include "../common/types.h"
#include "../common/app_state.h"

class Menu {
    public:
        Menu(AppState*);
        ~Menu();

        void show();
        void clear();

        MenuType getMenuType() { return m_menuType; }
        void setMenuType(MenuType menuType) { m_menuType = menuType; }

        void setModelAvailables(const std::vector<std::filesystem::path>& modelAvailables) {
            m_modelAvailables = modelAvailables;
        }

        void setDeviceAvailables(const std::vector<std::vector<Device>>& deviceAvailables) {
            m_deviceAvailables = deviceAvailables;
        }

        std::vector<int> getSelectedDeviceIndices() {
            return std::vector{m_inputDeviceIndex, m_outputDeviceIndex};
        }

        std::filesystem::path getSelectedModel() {
            return m_selectedModel;
        }

        MenuAction readAction();

    private:
        void showAudioSetupMenu();

        void showModelSelectionMenu();
        void createVolumeBar();
        void showMainMenu();

        MenuAction readAudioSetupAction();
        MenuAction readModelSelectionAction();
        MenuAction readMainMenuAction(char);
        
        std::vector<std::filesystem::path> m_modelAvailables;
        std::filesystem::path m_selectedModel;

        std::vector<std::vector<Device>> m_deviceAvailables;
        int m_inputDeviceIndex = -1;
        int m_outputDeviceIndex = -1;

        char m_option;

        MenuType m_menuType;
        AppState* m_appState;
};
