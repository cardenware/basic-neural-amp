#pragma once

#include <functional>
#include <vector>

#include <miniaudio.h>

typedef struct {
    ma_device_id id;
    std::string name;
} Device;

class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

        void enumerateDevices();

        void setProcessor(
            std::function<void(
                const float* input,
                float* output,
                unsigned int frameCount)> processor)
        {
            m_processor = std::move(processor);
        }

        void start(unsigned int inputDeviceIndex, unsigned int outputDeviceIndex);

    private:
        std::vector<Device> m_inputDevices, m_outputDevices;

        ma_context m_context;
        ma_device m_device;

        std::function<void(
            const float*,
            float*,
            unsigned int
        )> m_processor;

        static void dataCallback(
            ma_device* device,
            void* output,
            const void* input,
            ma_uint32 frameCount) {
            AudioEngine* self =
                static_cast<AudioEngine*>(device->pUserData);

            if (!self || !self->m_processor) {
                return;
            }

            self->m_processor(
                static_cast<const float*>(input),
                static_cast<float*>(output),
                frameCount
            );
        }
};
