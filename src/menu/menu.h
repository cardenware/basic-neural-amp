#pragma once

#include <iostream>
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

        void setMenuType(MenuType menuType) { m_menuType = menuType; }
        void setDeviceAvailables(std::vector<std::vector<Device>> *deviceAvailables) {
            m_deviceAvailables = deviceAvailables;
        }

        std::vector<int> getSelectedDeviceIndices() {
            return std::vector{m_inputDeviceIndex, m_outputDeviceIndex};
        }


        MenuAction readAction();

    private:
        void showAudioSetupMenu();

        void createVolumeBar();
        void showMainMenu();

        void readAudioSetupOption();
        MenuAction readMainMenuOption(char);
        
        std::vector<std::vector<Device>>* m_deviceAvailables;
        int m_inputDeviceIndex = -1;
        int m_outputDeviceIndex = -1;

        char m_option;

        MenuType m_menuType;
        AppState* m_appState;
};
