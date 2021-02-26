# ArduinoTaskManager

Manage many task's execution centralized by software timers. It was inspired by FreeRTOS approach, but FreeRTOS is too much for Arduino...


You just create tasks like this:

    task = TaskManager::add_task( "Name 1",          // task name
                                  TASK_PRIORITY_MIN, // task priority
                                  1 * 1000UL,        // task period
                                  task_callback,     // optional parameter for callack function
                                  NULL,              // call back function
                                  true,              // is task enable?
                                  true);             // is task repeatable?


put somewhere in loop() 

    TaskManager::do_tasks(); // do all TaskManager magic 

and manage them in your code: start, stop and check status

**Debug tools**

There is amazing tool to check status and perfomace for all registred tasks, just call in your code:

    TaskManager::show_status();

and you receive comprehensive table with tasks and measures for debug purposes in complex project with many tasks executed the same time. It looks like this:

    RAM:146 bytes free, 6 tasks registred:
    ID      Task name       Status  Pr-t    Pd*1000 Count   Min     Max     Avr     % E-3
    --------------------------------------------------------------------------------------------------------
    1       TaskQueue       E R     1       300     1001    40      25388   413     8
    2       TaskDelay       D S     1       1000    0       0       0       0       0
    3       TaskRead        D R     1       300     63      24      28      27      0
    4       TaskTimeOut     D S     1       1000    22      92      96      93      0
    5       test_sms        E R     1       15000   22      1340    1936    1503    10
    6       show_stat       E R     1       10000   31      516912  555400  551000  9
    --------------------------------------------------------------------------------------------------------
    Together:                                       1040417 292     555708

There is free memory in bytes and number registred tasks showing on the top. 

Below the table with task measurments, where:
- ID - Task ID
- Task name 
- Status:
    - E timer task is enabled 
    - D it is disabled
    - R this is repeatable task now
    - S this task execute one time
- Pr-t - prioritet of the task
- Pd*1000 - period of the task in msec.
- Count - how many time paricular task was called
- Min - min execution time in microseconds
- Max - max execution time in microseconds
- Avr - average execution time
- % - MCU time spent for task execution in %  

