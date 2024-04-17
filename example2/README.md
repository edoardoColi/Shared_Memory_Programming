# Releted to Lesson 10-1h+2h and 11. Slides 'C++ Concurrency Basics 1'
Thread usage in cpp. Test spawn and join threads and threads with lambda functions.  
Also using Promise-Future tools. In future headers also we have the opportunity to create task objects.
## Here it is a structure of the example folder
```
example/
│
├── include/
│   └── hpc_helpers.hpp
│
├── spawn-and-join.cpp              (Passing arguments to threads by value or by reference)
├── fibo.cpp                        (Each thread compute a value of fibonacci and store in an array)
├── fibopromise.cpp                 (As above but using the promise passing to the function)
├── make_task.cpp                   (Sequential code to show the association among future and task. Shows packaged_task that create a general task with associated future)
├── fibotasks.cpp
├── fiboasync.cpp                   (Use asynch to sincronize tasks, enbedded future)
│
├── alarm_clock.cpp                 (Like an alarm clock for a sleeping process)
├── alarm_clock_wrong.cpp           (Wrong because the nofity is loose and program get stuck)
├── one_shot_alarm_clock.cpp        (Example using futures of alarm clock but one to multi, use shared_future)
├── matrix_vector.cpp               (Different implementation for static data distribution policies)
└── Makefile
```
