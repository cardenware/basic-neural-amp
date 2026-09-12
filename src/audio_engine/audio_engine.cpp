#include <iostream>

#include "audio_engine.h"

AudioEngine::AudioEngine() {
    m_isRecording = false;
}

AudioEngine::~AudioEngine() {
    ma_device_uninit(&m_device);
    ma_context_uninit(&m_context);
}

std::vector<std::vector<Device>> AudioEngine::getDevices() {
    ma_result result = ma_context_init(NULL, 0, NULL, &m_context);
    if (result != MA_SUCCESS) {
        return std::vector<std::vector<Device>>{};
    }

    ma_device_info* InputDeviceInfos;
    ma_uint32 inputDeviceCount;
    
    ma_device_info* outputDeviceInfos;
    ma_uint32 outputDeviceCount;

    result = ma_context_get_devices(&m_context, &outputDeviceInfos, &outputDeviceCount, &InputDeviceInfos, &inputDeviceCount);
    if (result != MA_SUCCESS) {
        ma_context_uninit(&m_context);
        return std::vector<std::vector<Device>>{};
    }

    for (ma_uint32 i = 0; i < inputDeviceCount; ++i) {
        const ma_device_info* DeviceInfo = &InputDeviceInfos[i];
        m_inputDevices.push_back({DeviceInfo->id, DeviceInfo->name});
    }
    
    for (ma_uint32 i = 0; i < outputDeviceCount; ++i) {
        const ma_device_info* DeviceInfo = &outputDeviceInfos[i];
        m_outputDevices.push_back({DeviceInfo->id, DeviceInfo->name});
    }
    ma_context_uninit(&m_context);

    return {
        m_inputDevices,
        m_outputDevices
    };
}

void AudioEngine::start(unsigned int inputDeviceIndex, unsigned int outputDeviceIndex) {

    std::cout << "Initializing audio context..." << std::endl;
    ma_result result = ma_context_init(NULL, 0, NULL, &m_context);
    if (result != MA_SUCCESS) {
        return;
    }
    
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_duplex);

    std::cout << "Setting up device with input index: " << inputDeviceIndex << " and output index: " << outputDeviceIndex << std::endl;

    deviceConfig.sampleRate = 48000;
    deviceConfig.periodSizeInFrames = 128;
    deviceConfig.periods = 2;
    deviceConfig.performanceProfile = ma_performance_profile_low_latency;
    deviceConfig.playback.pDeviceID = &m_outputDevices[outputDeviceIndex].id;
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.capture.pDeviceID = &m_inputDevices[inputDeviceIndex].id;
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 1;
    deviceConfig.dataCallback = &AudioEngine::dataCallback;
    deviceConfig.pUserData = this;
    
    std::cout << "Initializing device..." << std::endl;
    result = ma_device_init(&m_context, &deviceConfig, &m_device);

    if (result != MA_SUCCESS) {
        return;
    }
    std::cout << "Device setup successful." << std::endl;

    result = ma_device_start(&m_device);

    std::cout << "Starting device..." << std::endl;
    if (result != MA_SUCCESS) {
        ma_device_uninit(&m_device);
        return;
    }
    std::cout << "Device started successfully." << std::endl;
}

void AudioEngine::startRecord() {
    m_encoderConfig = ma_encoder_config_init(
        ma_encoding_format_wav, 
        ma_format_f32, 
        2, 
        48000
    );

    if (ma_encoder_init_file("output.wav", &m_encoderConfig, &m_encoder) != MA_SUCCESS) {
        return;
    }

    m_isRecording = true;
}

void AudioEngine::stopRecord() {
    if (!m_isRecording) {
        return;
    }

    ma_encoder_uninit(&m_encoder);
    m_isRecording = false;
}