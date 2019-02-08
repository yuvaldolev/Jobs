#include "Jobs/Runner.h"
#include "Jobs/Job.h"
#include <algorithm>
#include <iostream>
#include <typeinfo>

#define GET_FN_ADDR(fn) *(long*)(char*)&fn

namespace Jobs
{
    Runner::Runner()
        : m_IsRunning(false), m_AsyncRunner(nullptr)
    {
    }

    Runner::~Runner()
    {
        if (m_IsRunning)
        {
            Stop();
        }
        
        Clear();
    }

    void Runner::Run()
    {
        if (m_IsRunning)
        {
            return;
        }

        m_IsRunning = true;
        
        while (m_IsRunning)
        {
            RunPending();
        }
    }

    void Runner::RunAsync()
    {
        m_AsyncRunner = new std::thread(&Runner::Run, this);
    }

    void Runner::Stop()
    {
        m_IsRunning = false;
        TerminateAsyncRunner();
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
        m_Jobs.erase(std::remove(m_Jobs.begin(), m_Jobs.end(), job), m_Jobs.end());
        
        if (job != nullptr)
        {
            delete job;
        }
    }

    Job* Runner::FindJob(const JOB_FUNC_TYPE& fn)
    {
        std::vector<Job*>::iterator it = std::find_if(m_Jobs.begin(), m_Jobs.end(), [fn](Job* job)
        {
            return job != nullptr && GET_FN_ADDR(fn) == GET_FN_ADDR(job->JobFunc());
        });
        
        return it == m_Jobs.end() ? nullptr : *it;
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

        return "Next Run: " + std::string(std::asctime(nextRunTime));
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

    void Runner::RunJob(Job* job)
    {
        job->Run();
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

    void Runner::TerminateAsyncRunner()
    {
        if (m_AsyncRunner != nullptr)
        {
            m_AsyncRunner->join();
            delete m_AsyncRunner;
            m_AsyncRunner = nullptr;
        }
    }
}

