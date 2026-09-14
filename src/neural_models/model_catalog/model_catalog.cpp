#include "model_catalog.h"
#include <iostream>

ModelCatalog::ModelCatalog() {
    // look for .nam files recursively...
    update();
}

ModelCatalog::~ModelCatalog() {}

void ModelCatalog::update() {
    m_catalog.clear();
    
    for (const auto& entry: std::filesystem::recursive_directory_iterator("nam_files")) {
        if (entry.is_regular_file() && entry.path().extension() == ".nam") {
            m_catalog.push_back(entry.path());
        }
    }
}
