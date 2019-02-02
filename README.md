# Jobs
Job Scheduling In C++ Made Easy.

Jobs is library that provides an easy to use API that lets you schedule C++ functions to run at a specified time(s) using a human readable syntax.

# Getting Started
Jobs uses [bake](https://github.com/SanderMertens/bake) as its build system. bake makes it easy to build Jobs and link it to any project on your computer. For details on how to use bake see [bake's github repository](https://github.com/SanderMertens/bake).

#### Using Jobs with your project
Add to following line to your project.json:
"use": ["Jobs"]

Then run the bake command.
If both projects' directories (Jobs, and the project that uses it) are in the same directory, then all you need to do is run bake in the root directory.
If not, you need to run the bake in Jobs' directory, the run bake in the dependent project's directory.

# Functionality
The following functions are available in Jobs:

Function | Description
---------|------|------
Every(interval: int) | Schedules a new job with the given interval
Second() | Makes the job run every 1 second
Seconds() | Makes the job run every interval seconds
Minute() | Make the job run every 1 minute
Minutes() | Makes the job run every interval minutes
Hour() | Makes the job run every 1 hour
language | Match or return target language
id | Return project identifier

Function | Description
---------|------|------
locate | Locate project paths in the bake environment
os | Match or return operating system
language | Match or return target language
id | Return project identifier
