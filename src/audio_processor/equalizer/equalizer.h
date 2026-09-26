#pragma once

#include <numbers>
#include <cmath>
#include <vector>

#include "../../common/types.h"
#include "../audio_processor.h"
#include "../filters/biquad.h"

class Equalizer : public AudioProcessor {
    public:
        Equalizer(double sampleRate=44100.0f) : m_sampleRate(sampleRate) {}
        void process(float* inputBuffer, std::size_t frameCount) override {
            if (m_bypass) {
                return;
            }
            
            for (auto& band : m_bands) {
                band.process(inputBuffer, frameCount);
            }
        }

        void reset() override {
            for (auto& band : m_bands) {
                band.reset();
            }
        }

        std::string getName() const override  {
            return std::string("Equalizer_") + std::to_string(m_id);
        }

        void setBand(
            FilterType type,
            double frequency,
            double Q,
            double gainDb = 0.0
        ) {
            const double omega = 2.0 * std::numbers::pi * frequency / m_sampleRate;
            const double sinw = std::sin(omega);
            const double cosw = std::cos(omega);
            const double alpha = sinw / (2.0 * Q);
            const double A = std::pow(10.0, gainDb / 40.0);

            double b0 = 0.0, b1 = 0.0, b2 = 0.0;
            double a0 = 1.0, a1 = 0.0, a2 = 0.0;

            switch (type) {
                case FilterType::LowPass:
                    b0 = (1.0 - cosw) / 2.0;
                    b1 = 1.0 - cosw;
                    b2 = (1.0 - cosw) / 2.0;
                    a0 = 1.0 + alpha;
                    a1 = -2.0 * cosw;
                    a2 = 1.0 - alpha;
                    break;

                case FilterType::HighPass:
                    b0 = (1.0 + cosw) / 2.0;
                    b1 = -(1.0 + cosw);
                    b2 = (1.0 + cosw) / 2.0;
                    a0 = 1.0 + alpha;
                    a1 = -2.0 * cosw;
                    a2 = 1.0 - alpha;
                    break;

                case FilterType::Peaking: {
                    const double alphaA = alpha * A;
                    const double alphaDivA = alpha / A;

                    b0 = 1.0 + alphaA;
                    b1 = -2.0 * cosw;
                    b2 = 1.0 - alphaA;
                    a0 = 1.0 + alphaDivA;
                    a1 = -2.0 * cosw;
                    a2 = 1.0 - alphaDivA;
                    break;
                }

                default:
                    return;
            }

            BiquadFilter band;
            band.setCoefficients(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
            m_bands.push_back(band);
        }

    private:
        double m_sampleRate;
        std::vector<BiquadFilter> m_bands;
};
