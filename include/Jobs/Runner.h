#pragma once

#include <string>
#include <vector>
#include <thread>

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

        // Job Run Loop
        void Run();
        
        // Async Job Run Loop
        void RunAsync();

        // Stops The Job Run Loop
        void Stop();
        
        // Adding Jobs
        void AddJob(Job* job);
        
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
        std::string NextRun() const;

        // Returns the number of seconds until the next run
        int IdleSeconds() const;

    // Private Methods
    private:            
        // Runs the given job
        static void RunJob(Job* job);
        
        // Finds the next running job and returns its run time
        tm* NextRunningJobTime() const;

        // Joins and deletes the async runner thread
        void TerminateAsyncRunner();

    // Private Fields
    private:
        std::vector<Job*> m_Jobs;
        bool m_IsRunning;
        std::thread* m_AsyncRunner;
    };    
}


