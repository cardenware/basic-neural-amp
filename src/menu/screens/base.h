#pragma once

#include <iostream>
#include <string>
#include "../../common/types.h"

class BaseScreen {
    public:
        ~BaseScreen() = default;

        virtual void show() = 0;
        virtual MenuAction read() = 0;

        virtual void clear() {
            // \033[H moves the cursor to the top-left home position
            // \033[2J clears the entire screen
            std::cout << "\033[H\033[2J" << std::flush;
        }
    protected:
        std::string m_choice;
};
