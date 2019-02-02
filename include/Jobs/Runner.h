#pragma once

#include <string>
#include <vector>

// TODO(yuval): Mabe use a sorted data structure to store jobs?

namespace Jobs
{
    class Job;
    
    class Runner
    {
    // Public Methods
    public:
        Runner();
        ~Runner();

        // Adding Jobs
        void AddJob(Job* job);
        
        // Job Running
        void RunPending();
        void RunAll();

        // Job Canceling
        void Clear();
        static void CancelJob(Job* job);

        // Schedules a new job
        Job& Every(int interval = 1);

        // Returns a string that represents the date and time when
        // the next job should run
        std::string NextRun() const;

        // Returns the number of seconds until the next run
        int IdleSeconds() const;

    // Private Methods
    private:            
        // Runs the given job
        static void RunJob(Job* job);
        
        // Finds the next running job and returns its run time
        tm* NextRunningJobTime() const;

    // Private Fields
    private:
        std::vector<Job*> m_Jobs;
    };    
}

