#include "Jobs/InterruptableSleeper.h"

#define MUTEX_UNIQUE_LOCK(name) std::unique_lock<std::mutex> name(m_Mutex)
#define SLEEP_IMPL(sleepFunc) MUTEX_UNIQUE_LOCK(lock); \
    sleepFunc; \
    m_Interrupted = false;

namespace Jobs
{
    InterruptableSleeper::InterruptableSleeper()
        : m_Interrupted(false)
    {
    }

    InterruptableSleeper::~InterruptableSleeper()
    {
    }

    void InterruptableSleeper::SleepFor(Clock::duration duration)
    {
        // Sleeping for the given duration or until the sleeper gets interrupted
        SLEEP_IMPL(m_CV.wait_for(lock, duration, [this] { return m_Interrupted; }));
    }

    void InterruptableSleeper::SleepUntil(std::time_t time)
    {
        SleepUntil(Clock::from_time_t(time));
    }

    void InterruptableSleeper::SleepUntil(Clock::time_point time)
    {
        // Sleeping until the given time point or until the sleeper gets interrupted
        SLEEP_IMPL(m_CV.wait_until(lock, time, [this] { return m_Interrupted; }));
    }

    void InterruptableSleeper::Sleep()
    {
        // Sleeping until the sleeper gets interrupted
        SLEEP_IMPL(m_CV.wait(lock, [this] { return m_Interrupted; }));
    }

    void InterruptableSleeper::Interrupt()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // Interrupting the sleeper
        m_Interrupted = true;
        m_CV.notify_one();
    }
}

