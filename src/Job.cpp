#include "Jobs/Job.h"
#include "Jobs/Runner.h"
#include <algorithm>
#include <iterator>
#include <sstream>

#define CHECK_INTERVAL_AND_THROW(type) if (m_Interval != 1) \
    { \
        throw Jobs::JobException("Use " type "s instead of " type); \
    }

namespace Jobs
{
    JobException::JobException(const std::string& msg)
        : m_Msg(msg)
    {
    }

    const char* JobException::what() const throw()
    {
        return m_Msg.c_str();
    }
    
    Job::Job(int interval, Runner* runner)
        : m_Interval(interval), m_Latest(-1), m_StartDay(-1),
          m_AtTime(nullptr), m_LastRun(nullptr), m_NextRun(nullptr),
          m_Runner(runner), m_Gen(m_Rd())
    {
    }

    Job::~Job()
    {
        if (m_AtTime != nullptr)
        {
            delete m_AtTime;
        }
    }

    bool Job::operator<(const Job& other)
    {
        if (m_NextRun == nullptr)
        {
            return false;
        }

        return std::mktime(m_NextRun) < std::mktime(other.NextRun());
    }

    Job& Job::Second()
    {
        CHECK_INTERVAL_AND_THROW("Second");
        return Seconds();
    }

    Job& Job::Seconds()
    {
        m_Unit = JobUnit::Seconds;
        return *this;
    }

    Job& Job::Minute()
    {
        CHECK_INTERVAL_AND_THROW("Minute");
        return Minutes();
    }

    Job& Job::Minutes()
    {
        m_Unit = JobUnit::Minutes;
        return *this;
    }

    Job& Job::Hour()
    {
        CHECK_INTERVAL_AND_THROW("Hour");
        return Hours();
    }

    Job& Job::Hours()
    {
        m_Unit = JobUnit::Hours;
        return *this;
    }

    Job& Job::Day()
    {
        CHECK_INTERVAL_AND_THROW("Day");
        return Days();
    }

    Job& Job::Days()
    {
        m_Unit = JobUnit::Days;
        return *this;
    }

    Job& Job::Week()
    {
        CHECK_INTERVAL_AND_THROW("Week");
        return Weeks();
    }

    Job& Job::Weeks()
    {
        m_Unit = JobUnit::Weeks;
        return *this;
    }

    // TODO(yuval): Replace week day methods' body with a call to a general day method
    Job& Job::Sunday()
    {
        CHECK_INTERVAL_AND_THROW("Sunday");
        m_StartDay = WeekDay::Sunday;
        return Weeks();
    }

    Job& Job::Monday()
    {
        CHECK_INTERVAL_AND_THROW("Monday");
        m_StartDay = WeekDay::Monday;
        return Weeks();
    }

    Job& Job::Tuesday()
    {
        CHECK_INTERVAL_AND_THROW("Tuesday");
        m_StartDay = WeekDay::Tuesday;
        return Weeks();
    }

    Job& Job::Wednesday()
    {
        CHECK_INTERVAL_AND_THROW("Wednesday");
        m_StartDay = WeekDay::Wednesday;
        return Weeks();
    }

    Job& Job::Thursday()
    {
        CHECK_INTERVAL_AND_THROW("Thursday");
        m_StartDay = WeekDay::Thursday;
        return Weeks();
    }

    Job& Job::Friday()
    {
        CHECK_INTERVAL_AND_THROW("Friday");
        m_StartDay = WeekDay::Friday;
        return Weeks();
    }

    Job& Job::Saturday()
    {
        CHECK_INTERVAL_AND_THROW("Saturday");
        m_StartDay = WeekDay::Saturday;
        return Weeks();
    }

    Job& Job::At(const std::string& time)
    {
        if (m_Unit != JobUnit::Days && m_Unit != JobUnit::Hours && m_Unit != JobUnit::Minutes
            && m_StartDay == -1)
        {
            throw JobException("Invalid Unit");
        }

        std::vector<std::string> splitTime = SplitString(time, ':');
        std::size_t splitTimeSize = splitTime.size();
        
        int hour = 0;
        int minute = 0;
        int second = 0;
        bool shouldThrow = false;
        
        try
        {
            if ((m_Unit == JobUnit::Days || m_StartDay != -1) && splitTimeSize >= 2)
            {
                hour = std::stoi(splitTime[0]);
                minute = std::stoi(splitTime[1]);

                if (splitTimeSize == 3)
                {
                    second = std::stoi(splitTime[2]);
                }
            }
            else if (m_Unit == JobUnit::Hours && splitTimeSize == 1)
            {
                minute = std::stoi(splitTime[0]);
            }
            else if (m_Unit == JobUnit::Minutes && splitTimeSize == 1)
            {
                second = std::stoi(splitTime[0]);
            }
            else
            {
                shouldThrow = true;
            }
        }
        catch (std::exception&)
        {
            shouldThrow = true;
        }

        if (shouldThrow)
        {
            throw JobException("Invalid Time");
        }

        if (m_AtTime != nullptr)
        {
            delete m_AtTime;
        }
        
        m_AtTime = new tm;
        m_AtTime->tm_hour = hour;
        m_AtTime->tm_min = minute;
        m_AtTime->tm_sec = second;
        
        return *this;
    }

    Job& Job::To(int latest)
    {
        m_Latest = latest;
        return *this;
    }

    Job& Job::Do(const JOB_FUNC_TYPE& jobFunc)
    {
        m_JobFunc = jobFunc;
        ScheduleNextRun();

        if (m_Runner != nullptr)
        {
            m_Runner->AddJob(this);
        }

        return *this;
    }

    void Job::Run()
    {
        m_JobFunc();        
        m_LastRun = GetLocalTime();
        ScheduleNextRun();
    }

    void Job::RunEvery(int interval)
    {
        m_Interval = interval;
    }

    void Job::ScheduleNextRun()
    {
        int interval = 1;
        
        if (m_Latest != -1)
        {
            if (m_Latest < m_Interval)
            {
                throw JobException("Latest Must Be Greater Then Or Equal To Interval");
            }
            
            std::uniform_int_distribution<uint32_t> uintDist(m_Interval, m_Latest);
            interval = uintDist(m_Gen);
        }
        else
        {
            interval = m_Interval;
        }

        if (m_StartDay != -1 && m_Unit != JobUnit::Weeks)
        {
            throw JobException("Start Day Can Only Be Used With Weeks Unit");
        }

        if (m_NextRun == nullptr)
        {
            m_NextRun = GetLocalTime();
        }
        
        CalcNextRun(interval);
    }
    
    void Job::CalcNextRun(int interval)
    {
        switch (m_Unit)
        {
        case JobUnit::Seconds:
            AdjustSeconds(interval);
            break;

        case JobUnit::Minutes:
            AdjustMinutes(interval);
            break;

        case JobUnit::Hours:
            AdjustHours(interval);
            break;

        case JobUnit::Days:
            AdjustDays(interval);
            break;

        case JobUnit::Weeks:
            AdjustWeeks(interval);
            break;
        }

        std::mktime(m_NextRun);
    }
    
    void Job::AdjustSeconds(int interval)
    {
        m_NextRun->tm_sec += interval;
    }

    void Job::AdjustMinutes(int interval)
    {
        AtSecond();
        m_NextRun->tm_min += interval;
    }
    
    void Job::AdjustHours(int interval)
    {
        AtMinute();
        m_NextRun->tm_hour += interval;
    }
    
    void Job::AdjustDays(int interval)
    {
        AtTime();
        
        m_NextRun->tm_mday += interval;
    }
    
    void Job::AdjustWeeks(int interval)
    {
        AtTime();
        
        if (m_StartDay == -1)
        {
            m_NextRun->tm_mday += interval * 7;
        }
        else
        {
            int daysAhead = m_StartDay - m_NextRun->tm_wday + 1;

            if (daysAhead < 0)
            {
                daysAhead += 7;
            }

            m_NextRun->tm_mday += daysAhead;
        }
    }

    void Job::AtTime()
    {
        AtSecond();
        AtMinute();
        AtHour();
    }
    
    void Job::AtSecond()
    {
        if (m_AtTime != nullptr)
        {
            m_NextRun->tm_sec = m_AtTime->tm_sec;
        }
    }
    
    void Job::AtMinute()
    {
        if (m_AtTime != nullptr)
        {
            m_NextRun->tm_min = m_AtTime->tm_min;
        }
    }
    
    void Job::AtHour()
    {
        if (m_AtTime != nullptr)
        {
            m_NextRun->tm_hour = m_AtTime->tm_hour;
        }
    }
        
    tm* Job::GetLocalTime()
    {
        time_t now = std::time(nullptr);
        return localtime(&now);
    }

    std::vector<std::string> Job::SplitString(const std::string& str, char delim)
    {
        std::vector<std::string> split;
        std::stringstream stream(str);
        std::string currToken;

        while (std::getline(stream, currToken, delim))
        {
            split.push_back(currToken);
        }

        return split;
    }
}

