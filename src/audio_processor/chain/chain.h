#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "../audio_processor.h"

class AudioChain {
    public:
        template<typename T, typename... Args>
        std::size_t addProcessor(Args&&... args) {
            auto processor = std::make_unique<T>(std::forward<Args>(args)...);
            const std::size_t processorId = processor->getId();

            m_audioProcessors.push_back(std::move(processor));

            return processorId;
        }
        
        template<typename T>
        T* getProcessor(std::size_t id) {
            for (const auto& processor : m_audioProcessors) {
                if (processor->getId() == id) {
                    return dynamic_cast<T*>(processor.get());
                }
            }

            return nullptr;
        }

        void process(float* input, std::size_t frameCount) {
            if (frameCount == 0) {
                return;
            }

            for (std::size_t i = 0; i < m_audioProcessors.size(); ++i) {
                if (!m_audioProcessors[i]) {
                    continue;
                }
                m_audioProcessors[i]->process(input, frameCount);
            }
        }

        std::string getAudioChainStr() {
            std::string audioChainStr = "";

            for (std::size_t i = 0; i < m_audioProcessors.size(); ++i) {
                if (!m_audioProcessors[i]) {
                    continue;
                }
                audioChainStr += m_audioProcessors[i]->getName();
                if (i < m_audioProcessors.size() - 1) {
                    audioChainStr += "->";
                }
            }

            return audioChainStr;
        }

    private:
        std::vector<std::unique_ptr<AudioProcessor>> m_audioProcessors;
};
