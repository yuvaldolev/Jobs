#pragma once

#include "Jobs/InterruptableSleeper.h"
#include "vendor/CTPL/ctpl_stl.h"
#include <ctime>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define JOB_MAP_TYPE std::multimap<std::time_t, Job*>
#define JOB_MAP_ITER JOB_MAP_TYPE::iterator

namespace Jobs
{
    class Job;

    class Runner
    {
    // Public Methods
    public:
        Runner(unsigned int maxJobs = 12);
        ~Runner();

        // Job Run Loop
        void Run();

        // Stops The Job Run Loop
        void Stop();

        // Adding Jobs
        void AddJob(std::time_t time, Job* job);

        // Job Running
        void RunPending();
        void RunAll();

        // Job Canceling
        void Clear();
        void CancelJob(Job* job);

        // Finds a job and returns a pointer to it
        // TODO(yuval): Define the function pointer type in Job.h!!!
        Job* FindJob(const std::function<void(void)>& fn);

        // Schedules a new job
        Job& Every(int interval = 1);

        // Returns a string that represents the date and time when
        // the next job should run
        std::string NextRun();

        // Returns the number of seconds until the next run
        int IdleSeconds();

    // Private Methods
    private:
        // Runs the given job
        void RunJob(Job* job);

        // Finds the next running job and returns its run time
        std::tm* NextRunningJobTime();

    // Private Fields
    private:
        bool m_IsRunning;
        JOB_MAP_TYPE m_Jobs;
        std::mutex m_Mutex;
        InterruptableSleeper m_Sleeper;
        ctpl::thread_pool m_Threads;
    };
}

