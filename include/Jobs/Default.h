#pragma once

#include "Runner.h"
#include "Job.h"

namespace Jobs
{
    Runner defaultRunner;

    void Run()
    {
        defaultRunner.Run();
    }

    void RunAsync()
    {
        defaultRunner.RunAsync();
    }

    Job& Every(int interval = 1)
    {
        return defaultRunner.Every(interval);
    }

    void Stop()
    {
        defaultRunner.Stop();
    }
    
    void RunPending()
    {
        defaultRunner.RunPending();
    }

    void RunAll()
    {
        defaultRunner.RunAll();
    }

    void Clear()
    {
        defaultRunner.Clear();
    }

    void CancelJob(Job* job)
    {
        defaultRunner.CancelJob(job);
    }

    std::string NextRun()
    {
        return defaultRunner.NextRun();
    }

    int IdleSeconds()
    {
        return defaultRunner.IdleSeconds();
    }
}



