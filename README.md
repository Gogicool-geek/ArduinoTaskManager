# ArduinoTaskManager

Manage many task's execution centralized by software timers. It was inspired by FreeRTOS approach, but FreeRTOS is too much for Arduino...

## How to deploy

You need to init library:

    #include <TaskManager.h>

Put in setup()

    TaskManager::begin();

Create tasks like this

    task = TaskManager::add_task( "Name 1",          // task name
                                  TASK_PRIORITY_MIN, // task priority
                                  1 * 1000UL,        // task period
                                  NULL,              // optional parameter
                                  task_callback,     // callback function
                                  true,              // is task enable?
                                  true);             // is task repeatable?

Put somewhere in loop() 

    TaskManager::do_tasks(); // do all TaskManager magic here

..and manage registred tasks in your code: start, stop, adjust and check status. It is delay()-free approach friendly.

## Manage your tasks

What you can do with your task:

- **add_task(...)** - add new task
- **do_tasks()** - call in loop() for execution TaskManager internal logic
- **show_status()** - show comprehensive statistic regarding tasks for debug purpose
- **free_memory()** - show free memeory
- **reset_timer(Task_t id)** - reset timer for certain task
- **enable_timer(Task_t id)** - enable certain task timer 
- **disable_timer(Task_t id)**- disable certain task timer 
- **set_autoreload(Task_t id, bool isAutoReload)** - set autoreload mode 
- **set_time(Task_t id, Period_t time)** - set new timer value
- **is_timer_enable(Task_t id)** - check if certain task timer is enable

### Create task

A few more explanation regarding how to create task. There are few parameters:

    TaskManager::add_task("Name 1",          // task name
                          TASK_PRIORITY_MIN, // task priority
                          1 * 1000UL,        // task period
                          NULL,              // optional parameter
                          task_callback,     // callback function
                          true,              // is task enable?
                          true);             // is task repeatable?

- **task name** - it is only for debug purpose. The size of name can be specify in *TaskManager.h* costant TASK_NAME_LENGHT
- **task priority** - it can be from TASK_PRIORITY_MIN to TASK_PRIORITY_MAX and can be specify in *TaskManager.h*. Actualy it affects only on task sorting. The task with the hjghest priority run first.
- **task period** - period of time in msec. when particular task is calling. If the task is repeatable, it calls every period of time, if not - calling once.
- **optional parameter** - it could be used to pass parameter *by reference* for callback function. See example below.
- **callback function** - callback fuction has following signature:

        static void callback_func( void* param)

    it is calling by timer and it must be **static**. 
    Some time it looks like tricky point how to pass parameters from your class object to callback function. In this case you can to do it like this:

        class MyClass
        ..
        my_task=TaskManager::add_task( ...
                                       (void*)this,     // parameter
                                       callback_func,   // callback func
                                       ..);            
    then, in order to access  class variables from callback function:

        static void callback_func( void* param){
            MyClass* obj = (MyClass*) param;     

            Serial.println( obj->SomeClassVariable );
        };
### Debug tools

There is amazing tool to check status and perfomace for all registred tasks, just call in your code:

    TaskManager::show_status();

and you receive comprehensive table with tasks and measures for debug purposes in complex project with many tasks executed the same time. It looks like this:

    RAM:146 bytes free, 6 tasks registred:
    ID      Task name       Status  Pr-t    Pd*1000 Count   Min     Max     Avr     % 
    ----------------------------------------------------------------------------------------
    1       TaskQueue       E R     1       300     1001    40      25388   413     8
    2       TaskDelay       D S     1       1000    0       0       0       0       0
    3       TaskRead        D R     1       300     63      24      28      27      0
    4       TaskTimeOut     D S     1       1000    22      92      96      93      0
    5       test_sms        E R     1       15000   22      1340    1936    1503    10
    6       show_stat       E R     1       10000   31      516912  555400  551000  9
    ----------------------------------------------------------------------------------------
    Together:                                       1040417 292     555708

There is free memory in bytes and number registred tasks showing on the top. 

Below the table with task measurments, where:
- ID - Task ID
- Task name 
- Status:
    - E timer task is enabled 
    - D timer task is disabled
    - R this is repeatable task now
    - S this task execute one time only
- Pr-t - Task's priority
- Pd*1000 - period of the task in msec.
- Count - how many time paricular task was called
- Min - min execution time in microseconds
- Max - max execution time in microseconds
- Avr - average execution time
- % - MCU time spent for task execution in %  

You can disable this feature by comment flag MEASURE_TASKS in *TaskManager.h*. it gives additional few free bytes in memory. 

### Miscellaneous

in order to optimize memory consumption you also can specify TASK_COUNT_MAX flag in your code (before  #include <TaskManager.h>  statement). By default it could not satisfy your needs, e. g.:

    TASK_COUNT_MAX=20
    #include <TaskManager.h>
    ..
