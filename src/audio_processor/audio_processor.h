#pragma once

#include <string>

class AudioProcessor {
    public:
        AudioProcessor() : m_id(s_nextId++) {}
        virtual ~AudioProcessor() = default;

        virtual std::size_t getId() { return m_id; }
        virtual void process(
            float* inputBuffer, 
            std::size_t frameCount
        ) = 0;
        virtual void reset() = 0;

        virtual bool getBypass() {
            return m_bypass;
        }
        virtual void setBypass(bool bypass) {
            m_bypass = bypass;
        }

        virtual std::string getName() const = 0;

    protected:
        bool m_bypass = false;
        std::size_t m_id;
        inline static std::size_t s_nextId = 0;
};
