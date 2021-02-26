#include <Arduino.h>
#include <TaskManager.h>

Task_t task1, task2, task3;

void task1_callback(void *param)
{
    Serial.println(F("task1 callback"));
    TaskManager::show_status();
}

void task2_callback(void *param)
{
    Serial.println(F("task2 callback"));
    TaskManager::enable_timer(task3);
}

void task3_callback(void *param)
{
    char *txt = (char *)param;
    Serial.println(txt);
}

void task3_callback(void *param) { Serial.println(F("task3_callback")); }

void setup()
{
    Serial.begin(9600);
    TaskManager::begin();

    task1 = TaskManager::add_task("Name 1",          // task name
                                  TASK_PRIORITY_MIN, // task priority
                                  1 * 1000UL,        // task period
                                  task1_callback,    // optional parameter for callack function
                                  NULL,              // call back function
                                  true,              // is task enable?
                                  true);             // is task repeatable?

    task2 = TaskManager::add_task("Name 2",          // task name
                                  TASK_PRIORITY_MIN, // task priority
                                  5 * 1000UL,        // task period
                                  task2_callback,    // optional parameter for callack function
                                  NULL,              // call back function
                                  true,              // is task enable?
                                  true);             // is task repeatable?

    task3 = TaskManager::add_task("Name 3",                         // task name
                                  TASK_PRIORITY_MAX,                // task priority
                                  2 * 1000UL,                       // task period
                                  task3_callback,                   // optional parameter for callack function
                                  (void *)"hello World from task3", // call back function
                                  false,                            // is task enable?
                                  false);                           // is task repeatable?
}


void loop()
{
    TaskManager::do_tasks(); // do all TaskManager magic 
}
