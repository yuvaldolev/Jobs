#pragma once

#include "Runner.h"
#include "Job.h"

namespace Jobs
{
    void Run();
    Job& Every(int interval = 1);
    void Stop();
    void RunPending();
    void RunAll();
    void Clear();
    void CancelJob(Job* job);
    Job* FindJob(const JOB_FUNC_TYPE& fn);
    std::string NextRun();
    int IdleSeconds();
}

