#pragma once

#include <functional>
#include <vector>

#include "../common/types.h"
#include <miniaudio.h>


class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

        std::vector<std::vector<Device>> getDevices();
        
        void setProcessor(
            std::function<void(
                const float* input,
                float* output,
                unsigned int frameCount)> processor)
        {
            m_processor = std::move(processor);
        }

        void start(unsigned int inputDeviceIndex, unsigned int outputDeviceIndex);
        void startRecord();
        void stopRecord();
    private:
        bool m_isRecording;
        std::vector<Device> m_inputDevices, m_outputDevices;

        ma_context m_context;
        ma_device m_device;
        ma_encoder_config m_encoderConfig;
        ma_encoder m_encoder;

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
            AudioEngine* self = static_cast<AudioEngine*>(device->pUserData);

            if (!self || !self->m_processor) {
                return;
            }

            self->m_processor(
                static_cast<const float*>(input),
                static_cast<float*>(output),
                frameCount
            );

            if (self->m_isRecording) {
                ma_uint64 framesWritten = 0;

                ma_encoder_write_pcm_frames(
                    &self->m_encoder,
                    output,
                    frameCount,
                    &framesWritten
                );
            }
        }
};
