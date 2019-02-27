#include "Jobs/Runner.h"
#include "Jobs/Job.h"
#include <algorithm>
#include <ctime>

#define GET_FN_ADDR(fn) *(long*)(char*)&fn

namespace Jobs
{
    Runner::Runner(unsigned int maxJobs)
        : m_IsRunning(false), m_Threads(maxJobs + 1)
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

        m_Threads.push([this](int)
        {
            while (m_IsRunning)
            {
                if (m_Jobs.empty())
                {
                    m_Sleeper.Sleep();
                }
                else
                {
                    std::time_t firstTaskTime = m_Jobs.begin()->first;
                    m_Sleeper.SleepUntil(firstTaskTime);
                }

                RunPending();
            }
        });
    }

    void Runner::Stop()
    {
        m_IsRunning = false;
        m_Sleeper.Interrupt();
    }

    void Runner::AddJob(std::time_t time, Job* job)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // If the job exists, then adding it to the jobs map and interrupting the sleeper
        if (job != nullptr)
        {
            m_Jobs.emplace(time, job);
            m_Sleeper.Interrupt();
        }
    }

    void Runner::RunPending()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        JOB_MAP_ITER jobsToRunEnd = m_Jobs.upper_bound(std::time(nullptr));

        // There are no pending jobs
        if (jobsToRunEnd == m_Jobs.begin())
        {
            return;
        }

        // Running all the panding jobs
        for (JOB_MAP_ITER i = m_Jobs.begin(); i != jobsToRunEnd; ++i)
        {
            RunJob(i->second);
        }

        // Removing the completed jobs
        m_Jobs.erase(m_Jobs.begin(), jobsToRunEnd);

        // NOTE(yuval): The lock_guard that was initialized in the beggining of the method
        //              will make sure that the jobs will be re-added only after they are removed.
    }

    void Runner::RunAll()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        for (const std::pair<std::time_t, Job*>& elem : m_Jobs)
        {
            RunJob(elem.second);
        }

        // Removing all the jobs
        m_Jobs.clear();

        // NOTE(yuval): The lock_guard that was initialized in the beggining of the method
        //              will make sure that the jobs will be re-added only after they are removed.
    }

    void Runner::Clear()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        Job* currJob = nullptr;

        for (JOB_MAP_ITER i = m_Jobs.begin(); i != m_Jobs.end(); i++)
        {
            currJob = i->second;

            if (currJob != nullptr)
            {
                delete currJob;
                currJob = nullptr;
            }
        }

        m_Jobs.clear();
    }

    void Runner::CancelJob(Job* job)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        JOB_MAP_ITER iter = std::find_if(m_Jobs.begin(), m_Jobs.end(),
                                         [job](const std::pair<std::time_t, Job*>& element)
        {
            return job == element.second;
        });

        // If no job was found there is no job for us to cancel
        if (iter == m_Jobs.end())
        {
            return;
        }

        Job* jobToRemove = iter->second;

        // Deleting the job object
        if (jobToRemove != nullptr)
        {
            delete jobToRemove;
        }

        // Removing the job pointer from the job map
        m_Jobs.erase(iter);
    }

    Job* Runner::FindJob(const JOB_FUNC_TYPE& fn)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        JOB_MAP_ITER iter = std::find_if(m_Jobs.begin(), m_Jobs.end(),
                                         [fn](const std::pair<std::time_t, Job*>& element)
        {
            Job* currJob = element.second;
            return currJob != nullptr && GET_FN_ADDR(fn) == GET_FN_ADDR(currJob->JobFunc());
        });

        return iter == m_Jobs.end() ? nullptr : iter->second;
    }

    Job& Runner::Every(int interval)
    {
        return *(new Job(interval, this));
    }

    std::string Runner::NextRun()
    {
        const tm* nextRunTime = NextRunningJobTime();

        if (nextRunTime == nullptr)
        {
            return std::string("There Are No Pending Jobs To Run");
        }

        return "Next Run: " + std::string(std::asctime(nextRunTime));
    }

    int Runner::IdleSeconds()
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
        // Running the job in the thread pool, and then adding it back to the job map
        m_Threads.push([this, job](int)
        {
            job->Run();

            // NOTE(yuval): AddJob will wait for the Run function to release the mutex!
            AddJob(job->GetNextRun(), job);
        });
    }

    std::tm* Runner::NextRunningJobTime()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        if (m_Jobs.empty())
        {
            return nullptr;
        }

        return localtime(&m_Jobs.begin()->first);
    }
}

