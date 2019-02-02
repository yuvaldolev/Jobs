# Jobs
Job Scheduling In C++ Made Easy.

Jobs is library that provides an easy to use API that lets you schedule C++ functions to run at a specified time(s) using a human readable syntax.

# Getting Started
Jobs uses [bake](https://github.com/SanderMertens/bake) as its build system. bake makes it easy to build Jobs and link it to any project on your computer. For details on how to use bake see [bake's github repository](https://github.com/SanderMertens/bake).

#### Using Jobs with your project:
First, clone the Jobs repository using:
```
git clone https://github.com/yuvaldolev/Jobs
```

Then, Add to following line to your project.json under the value key:
```
"use": ["Jobs"]
```

Then run the bake command.
If both projects' directories (Jobs, and the project that uses it) are in the same directory, then all you need to do is run bake in the root directory.
If not, you need to run the bake in Jobs' directory, the run bake in the dependent project's directory.

# Functionality
The following functions are available in Jobs:

### Job Scheduling Functions:
| Function | Description |
|--------- | ----------- |
| Every(interval: int) | Schedules a new job with the given interval |
| Second() | Makes the job run every second |
| Seconds() | Makes the job run every interval seconds |
| Minute() | Make the job run every minute |
| Minutes() | Makes the job run every interval minutes |
| Hour() | Makes the job run every hour |
| Hours() | Makes the job run every interval hours |
| Day() | Makes the job run every day |
| Days() | Makes the job run every interval days |
| Week() | Makes the job run every week |
| Weeks() | Makes the job run every interval weeks |
| Sunday() | Makes the job run every Sunday |
| Monday() | Makes the job run every Monday |
| Tuesday() | Makes the job run every Tuesday |
| Wednesday() | Makes the job run every Wednesday |
| Thursday() | Makes the job run every Thursday |
| Friday() | Makes the job run every Friday |
| Saturday() | Makes the job run every Saturday |
| At() | Makes the job at a specific time (for example - 10:30:22) |
| To(latest: int) | Makes the job run in a random time in range: interval - latest |
| Do(jobFunc: std::function<void()>) | Specified the job function that will be called every time the job runs |

### Job Running Functions:
| Function | Description |
|--------- | ----------- |
| Run() | Starts the job run loop |
| RunAsync() | Starts the job run loop asynchronously |
| Stop() | Stops the job run loop |
| RunPending() | Runs all the pending jobs once |
| RunAll() | Runs all the jobs once |

### Job Management Functions:
| Function | Description |
|--------- | ----------- |
| Clear() | Cancels and removes all the jobs |
| CancelJob(job: Job*) | Cancels and removes a specific job |

### Job Info Functions:
| Function | Description |
|--------- | ----------- |
| NextRun() | Returns a string that date and time when the next job should run |
| IdleSeconds() | Returns the number of seconds until the next run |
