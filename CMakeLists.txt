cmake_minimum_required(VERSION 3.12)

project(
    BasicNeuralAmp
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(
    third_party/NeuralAudio
    ${CMAKE_BINARY_DIR}/NeuralAudio
)

add_executable(
    BasicNeuralAmp
    src/menu/menu.cpp
    src/audio_engine/audio_engine.cpp
    third_party/miniaudio/miniaudio.c
    src/basic_neural_amp.cpp
)

target_include_directories(
    BasicNeuralAmp
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/miniaudio
)

target_link_libraries(
    BasicNeuralAmp
    PRIVATE
    NeuralAudio
)
