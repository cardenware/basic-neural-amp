#include "model_catalog.h"
#include <iostream>

ModelCatalog::ModelCatalog() {
    const char* nam_dir = "nam_files";

    if (!std::filesystem::exists(nam_dir)) {
        if (std::filesystem::create_directory(nam_dir)) {
            std::cout << "nam_files directory created" << std::endl;
        }
        else {
            std::cout << "nam_files directory couldn't be created" << std::endl;
        }
    }   // else, the directory already exists
    
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
