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
        T* getProcessorById(std::size_t id) {
            for (const auto& processor : m_audioProcessors) {
                if (processor->getId() == id) {
                    return dynamic_cast<T*>(processor.get());
                }
            }

            return nullptr;
        }

        template<typename T>
        T* getProcessorByIndex(std::size_t index) {
            return dynamic_cast<T*>(m_audioProcessors[index].get());
        }

        void removeProcessorByIndex(std::size_t index) {
            m_audioProcessors.erase(m_audioProcessors.begin() + index);
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

        void swap(int src, int dst) {
            std::swap(m_audioProcessors[src], m_audioProcessors[dst]);
        }

        std::string getAudioChainStr() {
            std::string audioChainStr = "";

            for (std::size_t i = 0; i < m_audioProcessors.size(); ++i) {
                if (!m_audioProcessors[i]) {
                    continue;
                }
                audioChainStr += m_audioProcessors[i]->getName();
                if (i < m_audioProcessors.size() - 1) {
                    audioChainStr += " -> ";
                }
            }

            return audioChainStr;
        }

        void list() {
            for (std::size_t i = 0; i < m_audioProcessors.size(); ++i) {
                std::cout << "[" << i+1 << "] " << m_audioProcessors[i]->getName() << std::endl;
            }
        }

    private:
        std::vector<std::unique_ptr<AudioProcessor>> m_audioProcessors;
};
