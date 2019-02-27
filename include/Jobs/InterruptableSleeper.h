#pragma once

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <mutex>
#include <thread>

namespace Jobs
{
    class InterruptableSleeper
    {
        using Clock = std::chrono::system_clock;

    public:
        // Ctor, Dtor
        InterruptableSleeper();
        ~InterruptableSleeper();

        // No copy constructors for the InterruptableSleeper
        InterruptableSleeper(const InterruptableSleeper& other) = delete;
        InterruptableSleeper(InterruptableSleeper&& other) noexcept = delete;

        // No assignment operators for the InterruptableSleeper
        InterruptableSleeper& operator=(const InterruptableSleeper& other) noexcept = delete;
        InterruptableSleeper& operator=(InterruptableSleeper&& other) noexcept = delete;

        // Sleep for a given durtaion
        void SleepFor(Clock::duration duration);

        // Sleep until a given time
        void SleepUntil(std::time_t time);
        void SleepUntil(Clock::time_point time);

        // Just sleep...
        void Sleep();

        // Interrupt the sleeper
        void Interrupt();

    private:
        bool m_Interrupted;
        std::mutex m_Mutex;
        std::condition_variable m_CV;
    };
}

