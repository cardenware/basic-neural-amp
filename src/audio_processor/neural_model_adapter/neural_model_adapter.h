#pragma once

#include <cmath>
#include <filesystem>
#include <mutex>

#include "../../common/types.h"
#include "../audio_processor.h"
#include "NeuralAudio/NeuralModel.h"

class NeuralModelAdapter: public AudioProcessor {
    public:
        NeuralModelAdapter() {
            m_ampParameters.modelName.clear();
            m_ampParameters.recommendedOutputdB = 1.0f;
            m_ampParameters.masterVolume = 1.0f;
            m_bypass = false;
        }

        void setActiveModel(
            std::filesystem::path& modelPath
        ) {
            std::unique_ptr<NeuralAudio::NeuralModel> replacement{
                m_loader.CreateFromFile(modelPath)
            };

            if (!replacement) {
                return;
            }
            
            std::lock_guard lock(modelMutex);

            m_activeModel = std::move(replacement);
            m_ampParameters.modelName = modelPath.stem().string();
            m_ampParameters.recommendedOutputdB = m_activeModel->GetRecommendedOutputDBAdjustment();
        }

        const NeuralAudio::NeuralModel* getActiveModel() const {
            std::lock_guard lock(modelMutex);
            return m_activeModel.get();
        }

        void process(float* inputBuffer, std::size_t frameCount) override {
            if (m_bypass) {
                return;
            }
            
            if (m_activeModel) {
                std::lock_guard lock(modelMutex);
                std::unique_ptr<float[]> outputBuffer = std::make_unique<float[]>(frameCount);
                std::copy(inputBuffer, inputBuffer + frameCount, outputBuffer.get());

                m_activeModel->Process(inputBuffer, outputBuffer.get(), frameCount);
                
                float currentVolume = m_ampParameters.masterVolume;
                float recommendedOutput = m_ampParameters.recommendedOutputdB;
                const float outputGain = std::pow(
                    10.0f, 
                    recommendedOutput / 20.0f) * ((currentVolume * 100.0f) / 100.0f
                );
                
                for (int i = 0; i < frameCount; ++i) {
                    outputBuffer[i] *= outputGain;
                }
                
                std::copy(outputBuffer.get(), outputBuffer.get() + frameCount, inputBuffer);
            }
        }

        void reset() override {
            m_activeModel.reset();
            m_ampParameters.modelName.clear();
            m_ampParameters.recommendedOutputdB = 1.0f;
            m_ampParameters.masterVolume = 1.0f;
        }

        bool getBypass() override {
            return m_bypass;
        }

        void setBypass(bool bypass) override {
            m_bypass = bypass;
        }

        float getRecommendedOutputdBModel() const {
            return m_ampParameters.recommendedOutputdB;
        }

        void setRecommendedOutputdBModel(float recommendedOutputdB) {
            std::lock_guard lock(modelMutex);
            m_ampParameters.recommendedOutputdB = recommendedOutputdB;
        }

        float getMasterVolume() const {
            std::lock_guard<std::mutex> lock(modelMutex);
            return m_ampParameters.masterVolume;
        }

        void setMasterVolume(float masterVolume) {
            std::lock_guard lock(modelMutex);
            m_ampParameters.masterVolume = masterVolume;
        }

        const AmpParameters& getAmpParameters() const {
            std::lock_guard lock(modelMutex);
            return m_ampParameters;
        }

        std::string getName() const override  {
            return std::string("NeuralModelAdapter_") + std::to_string(m_id);
        }

    private:
        mutable std::mutex modelMutex;
        NeuralAudio::NeuralModelLoader m_loader;
        std::unique_ptr<NeuralAudio::NeuralModel> m_activeModel;

        AmpParameters m_ampParameters;
};
