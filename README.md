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

and you receive comprehensive table with tasks and measures for debug purposes in complex project with many tasks executed the same time.