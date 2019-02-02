#include "Jobs/Runner.h"
#include "Jobs/Job.h"
#include <algorithm>

namespace Jobs
{
    Runner::Runner()
    {
    }

    Runner::~Runner()
    {
        Clear();
    }

    void Runner::AddJob(Job* job)
    {
        if (job != nullptr)
        {
            m_Jobs.push_back(job);
        }
    }

    void Runner::RunPending()
    {   
        std::vector<Job*> runnableJobs;

        for (Job* job : m_Jobs)
        {
            if (job->ShouldRun())
            {
                runnableJobs.push_back(job);    
            }
        }

        // No jobs should be run
        if (runnableJobs.empty())
        {
            return;
        }
        
        std::sort(runnableJobs.begin(), runnableJobs.end());

        for (Job* job : runnableJobs)
        {
            RunJob(job);
        }
    }

    void Runner::RunAll()
    {
        std::vector<Job*> jobsToRun = m_Jobs;
        std::sort(jobsToRun.begin(), jobsToRun.end());

        for (Job* job : jobsToRun)
        {
            RunJob(job);
        }
    }

    void Runner::Clear()
    {
        for (Job* job : m_Jobs)
        {
            CancelJob(job);
        }

        m_Jobs.clear();
    }

    void Runner::CancelJob(Job* job)
    {
        // TODO(yuval): Remove the job from the jobs vector
        
        if (job != nullptr)
        {
            delete job;
        }
    }

    Job& Runner::Every(int interval)
    {
        return *(new Job(interval, this));
    }

    std::string Runner::NextRun() const
    {
        const tm* nextRunTime = NextRunningJobTime();

        if (nextRunTime == nullptr)
        {
            return std::string("No Job To Run");
        }

        return std::string(std::asctime(nextRunTime));
    }

    int Runner::IdleSeconds() const
    {
        tm* nextRunTime = NextRunningJobTime();

        if (nextRunTime == nullptr)
        {
            return -1;
        }

        return static_cast<int>(std::difftime(std::mktime(nextRunTime), std::time(nullptr)));
    }
    
    tm* Runner::NextRunningJobTime() const
    {
        if (m_Jobs.empty())
        {
            return nullptr;
        }
        
        Job* closestJob = *std::min_element(m_Jobs.begin(), m_Jobs.end());
        return closestJob == nullptr ? nullptr : closestJob->NextRun();
    }

    void Runner::RunJob(Job* job)
    {
        job->Run();
    }
}

