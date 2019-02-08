#pragma once

#include <ctime>
#include <exception>
#include <functional>
#include <random>
#include <string>
#include <vector>

#define JOB_FUNC_TYPE std::function<void(void)>

#define BIND_FN(fn) std::bind(&fn)
#define BIND_METHOD(method, obj) std::bind(&method, obj)

namespace Jobs
{
    class Runner;
    
    namespace JobUnit
    {
        enum Unit
        {
            Seconds = 0,
            Minutes,
            Hours,
            Days,
            Weeks                
        };
    }

    namespace WeekDay
    {
        enum Day
        {
            Sunday = 1,
            Monday,
            Tuesday,
            Wednesday, 
            Thursday,
            Friday,
            Saturday
        };
    }

    class JobException : public std::exception
    {
    public:
        JobException(const std::string& msg);
        const char* what() const throw() override;
        
    private:
        std::string m_Msg;
    };
    
    class Job
    {
    // Public Methods
    public:
        // Ctor, Dtor
        Job(int interval, Runner* runner = nullptr);
        ~Job();

        // Returns the next job run time
        inline tm* NextRun() const
        {
            return m_NextRun;
        }

        // Returns the job function
        inline const JOB_FUNC_TYPE& JobFunc() const
        {
            return m_JobFunc;
        }
        
        // Checks if the job should be run now
        inline bool ShouldRun() const
        {
            if (m_NextRun == nullptr)
            {
                return false;
            }
            
            return std::time(nullptr) >= std::mktime(m_NextRun);
        }
        
        // Operator Overloadings
        bool operator<(const Job& other);

        // Seconds
        Job& Second();
        Job& Seconds();

        // Minutes
        Job& Minute();
        Job& Minutes();

        // Hours
        Job& Hour();
        Job& Hours();

        // Days
        Job& Day();
        Job& Days();

        // Weeks
        Job& Week();
        Job& Weeks();

        // Week days
        Job& Sunday();
        Job& Monday();
        Job& Tuesday();
        Job& Wednesday();
        Job& Thursday();
        Job& Friday();
        Job& Saturday();

        // Particular time to run the job
        // Parameter should be in one of the following formats: "HH:MM:SS", "MM:MM", "MM", "SS"
        Job& At(const std::string& time);

        // Schedules the job to run in a random time in range: from 'every' to 'latests'
        Job& To(int latest);

        // Specifies the function that will be called every time the job runs
        Job& Do(const JOB_FUNC_TYPE& jobFunc);

        // Runs the job and immediately reschedules it
        void Run();

        // Changes the job's interval
        void RunEvery(int interval);
        
    // Private Methods
    private:
        // Computes the instant when this job should run next
        void ScheduleNextRun();
        void CalcNextRun(int interval);
        
        // Date Time Adjustment Functions
        void AdjustSeconds(int interval);
        void AdjustMinutes(int interval);
        void AdjustHours(int interval);
        void AdjustDays(int interval);
        void AdjustWeeks(int interval);

        void AtTime();
        void AtSecond();
        void AtMinute();
        void AtHour();

        static tm* GetLocalTime();
        static std::vector<std::string> SplitString(const std::string& str, char delim = ' ');

    // Private Fields
    private:
        int m_Interval; // Pause interval * unit between runs
        int m_Latest; // Upper limit to the random interval
        int m_StartDay; // Day of week on which to start running the job
        tm* m_AtTime; // Optional time at which the job runs
        tm* m_LastRun; // Date and time of the last run
        tm* m_NextRun; // Date and time of the next run
        JOB_FUNC_TYPE m_JobFunc; // The job function to run
        JobUnit::Unit m_Unit; // Time units, e.g. Minutes, Seconds, etc...
        Runner* m_Runner; // The job runner

        // Random Ints
        std::random_device m_Rd;
        std::mt19937 m_Gen;

    };
}

