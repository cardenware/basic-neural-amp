#pragma once

#include <filesystem>
#include "../../common/types.h"

class ModelCatalog {
    public:
        ModelCatalog();
        ~ModelCatalog();

        const std::vector<std::filesystem::path>& get() { return m_catalog; }
        void update();
        
    private:
        std::vector<std::filesystem::path> m_catalog;
};
