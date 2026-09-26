#pragma once

#include <algorithm>

#include "../audio_processor.h"

class BiquadFilter : public AudioProcessor {
    public:
        void setCoefficients(double b0, double b1, double b2, double a1, double a2) {
            m_b0 = b0;
            m_b1 = b1;
            m_b2 = b2;
            m_a1 = a1;
            m_a2 = a2;
        }

        void process(float* inputBuffer, std::size_t frameCount) override {
            for (int i = 0; i < frameCount; ++i) {
                float sample = inputBuffer[i];
                const double x = sample;
                const double y = (
                    m_b0 * x + 
                    m_b1 * m_x1 + 
                    m_b2 * m_x2 - 
                    m_a1 * m_y1 - 
                    m_a2 * m_y2
                );

                m_x2 = m_x1;
                m_x1 = x;
                m_y2 = m_y1;
                m_y1 = y;

                inputBuffer[i] = static_cast<float>(y);
            }
        }

        void reset() override {
            m_x1 = 0.0;
            m_x2 = 0.0;
            m_y1 = 0.0;
            m_y2 = 0.0;
        }

        void setBypass(bool bypass) override {
            AudioProcessor::setBypass(bypass);
        }

        std::string getName() const override { return ""; }

    private:
        double m_b0 = 0.0;
        double m_b1 = 0.0;
        double m_b2 = 0.0;
        double m_a1 = 0.0;
        double m_a2 = 0.0;
        double m_x1 = 0.0;
        double m_x2 = 0.0;
        double m_y1 = 0.0;
        double m_y2 = 0.0;
};
