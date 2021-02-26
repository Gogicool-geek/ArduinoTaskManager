#include "TaskManager.h"

uint32_t tab(uint32_t num)
{
    if (num > 9999999)
        return 0;
    else
        return num;
}

// init static class' variables
TaskCollection initTaskCollection()
{
    TaskCollection collection;
    return collection;
}
TaskCollection TaskManager::_tasks = initTaskCollection();

#ifdef MEASURE_TASKS
uint32_t TaskManager::_totalRuntimeMax = 0;
uint32_t TaskManager::_totalRuntimeMin = UINT32_MAX;
uint32_t TaskManager::_totalRuntimeCount = 0;
uint32_t TaskManager::_totalRuntime = 0;
#endif

void TaskManager::begin()
{
    _tasks.begin(TASK_COUNT_MAX);
}

Task_t TaskManager::add_task(const char *name,
                         Priority_t priority,
                         Period_t period,
                         CallBackFunc_t callback,
                         void *parameter,
                         bool enable,
                         bool autoReload)
{
    static Task_t id_count = 1;

    if (_tasks.getSize() >= TASK_COUNT_MAX)
    {
        Serial.println(F("TaskManager if full!"));
        while (1)
            ;
    }
    TaskItem item;
    item.description.id = id_count;
    strncpy(item.description.name, name, TASK_NAME_LENGHT);
    item.description.name[TASK_NAME_LENGHT - 1] = '\0'; // гарантированно нуль терминатор в конце строки
    item.description.period = period;
    item.description.priority = priority;
    item.description.callback = callback;
    item.description.parameter = parameter;
#ifdef MEASURE_TASKS
    item.runtime.count = 0;
    item.runtime.max = 0;
    item.runtime.min = UINT32_MAX;
    item.runtime.time = 0;
#endif
    item.runtime.lastCall = millis();
    item.runtime.isEnable = enable;
    item.runtime.isAutoReload = autoReload;
    _tasks.addItem(item);
    // сортировка методом пузырька по приоритету задачи. Задачи с самым высоким приоритетом - в начале
    for (size_t i = (_tasks.getSize() - 1); i > 0; i--)
    {
        if (_tasks.get_item_link(i)->description.priority > _tasks.get_item_link(i - 1)->description.priority)
        {
            item = _tasks.getItem(i - 1);
            _tasks.setItem(i - 1, _tasks.getItem(i));
            _tasks.setItem(i, item);
        }
        else
            break;
    }
    return id_count++;
}

void TaskManager::do_tasks()
{
#ifdef MEASURE_TASKS
    uint32_t totalTime = micros();
#endif

    for (size_t i = 0; i < _tasks.getSize(); i++)
    {
        yield();
        if ((_tasks.getItem(i).runtime.isEnable) && ((millis() - _tasks.getItem(i).runtime.lastCall) > _tasks.getItem(i).description.period))
        {

#ifdef MEASURE_TASKS
            uint32_t timer = micros();
#endif
            if (_tasks.get_item_link(i)->runtime.isAutoReload == false)
                _tasks.get_item_link(i)->runtime.isEnable = false;
            // выполняем задачу
            _tasks.get_item_link(i)->description.callback(_tasks.get_item_link(i)->description.parameter);
            //обновляем таймер
            _tasks.get_item_link(i)->runtime.lastCall = millis();
#ifdef MEASURE_TASKS
            // Measure task's perfomance for DEBUG purpose
            timer = micros() - timer; // вычисляем время выполнения функции
            _tasks.get_item_link(i)->runtime.time = timer + _tasks.get_item_link(i)->runtime.time;
            _tasks.get_item_link(i)->runtime.count = 1 + _tasks.get_item_link(i)->runtime.count;
            if (timer > _tasks.get_item_link(i)->runtime.max)
                _tasks.get_item_link(i)->runtime.max = timer; // max
            if (timer < _tasks.get_item_link(i)->runtime.min)
                _tasks.get_item_link(i)->runtime.min = timer; // min
#endif
        }
    }

#ifdef MEASURE_TASKS
    totalTime = micros() - totalTime;
    _totalRuntimeMax = (totalTime > _totalRuntimeMax) ? totalTime : _totalRuntimeMax;
    _totalRuntimeMin = (totalTime < _totalRuntimeMin) ? totalTime : _totalRuntimeMin;
    _totalRuntimeCount++;
    _totalRuntime += totalTime;
#endif
}

TaskItem *TaskManager::get_task_link(Task_t id)
{
    for (size_t i = 0; i < _tasks.getSize(); i++)
    {
        if (_tasks.get_item_link(i)->description.id == id)
            return _tasks.get_item_link(i);
    }
    return NULL;
}

void TaskManager::reset_timer(Task_t id)
{
    TaskItem *task = get_task_link(id);
    if (task != NULL)
        task->runtime.lastCall = millis();
}

// разрешает работу таймеру задачи и сбрасывает таймер
// @param id id задачи
void TaskManager::enable_timer(Task_t id)
{
    TaskItem *task = get_task_link(id);
    if (task != NULL)
    {
        task->runtime.isEnable = true;
        task->runtime.lastCall = millis();
    }
}
// запрещает работу таймеру задачи
// @param id id задачи
void TaskManager::disable_timer(Task_t id)
{
    TaskItem *task = get_task_link(id);
    if (task != NULL)
        task->runtime.isEnable = false;
}

// возвращает статус работы таймера задачи
// @param id id задачи
bool TaskManager::is_timer_enable(Task_t id)
{
    TaskItem *task = get_task_link(id);
    return (task == NULL) ? false : task->runtime.isEnable;
}
// устанавливает режим автоматической перезагрузки таймера
// @param id id задачи
// @param isAutoReload true - цикличный таймер. false - one shot
void TaskManager::set_autoreload(Task_t id, bool isAutoReload)
{
    TaskItem *task = get_task_link(id);
    if (task != NULL)
        task->runtime.isAutoReload = isAutoReload;
}
// устанавливает время таймера
// @param id id задачи
// @param time новое значение времени
void TaskManager::set_time(Task_t id, Period_t time)
{
    TaskItem *task = get_task_link(id);
    if (task != NULL)
    {
        task->description.period = time;
    };
}

void TaskManager::show_status()
{
    Serial.print(F("\n***\nRAM:"));
    Serial.print(TaskManager::free_memory());
    Serial.print(F(" bytes free, "));
    Serial.print(_tasks.getSize());
    Serial.print(F(" tasks registred:\n"));
    Serial.println(F("ID\tTask name\tStatus\tPr-t\tPd*1000\tCount\tMin\tMax\tAvr\t%"));
    Serial.println(F("---------------------------------------------------------------------------------------"));
    for (size_t i = 0; i < _tasks.getSize(); i++)
    {
        Serial.print(_tasks.get_item_link(i)->description.id);
        Serial.print(F("\t"));
        Serial.print(_tasks.get_item_link(i)->description.name);
        Serial.print(F("\t"));
        Serial.print(_tasks.get_item_link(i)->runtime.isEnable ? "E" : "D");
        Serial.print(F(" "));
        Serial.print(_tasks.get_item_link(i)->runtime.isAutoReload ? "R" : "S");
        Serial.print(F("\t"));
        Serial.print(_tasks.get_item_link(i)->description.priority);
        Serial.print(F("\t"));
        Serial.print(_tasks.get_item_link(i)->description.period);
        Serial.print(F("\t"));
#ifdef MEASURE_TASKS
        Serial.print(tab(_tasks.get_item_link(i)->runtime.count));
        Serial.print(F("\t"));
        Serial.print(tab(_tasks.get_item_link(i)->runtime.min));
        Serial.print(F("\t"));
        Serial.print(tab(_tasks.get_item_link(i)->runtime.max));
        Serial.print(F("\t"));
        uint32_t count = _tasks.get_item_link(i)->runtime.count;
        Serial.print((count > 1) ? _tasks.get_item_link(i)->runtime.time / count : 0);
        Serial.print(F("\t"));
        Serial.print((_totalRuntime > 0) ? _tasks.get_item_link(i)->runtime.time * 100000 / _totalRuntime : 0);
#endif
        Serial.println(F(""));
    }
    Serial.println(F("---------------------------------------------------------------------------------------"));
    Serial.print(F("Together:\t\t\t\t\t"));
#ifdef MEASURE_TASKS
    Serial.print(_totalRuntimeCount);
    Serial.print(F("\t"));
    Serial.print(_totalRuntimeMin);
    Serial.print(F("\t"));
    Serial.println(_totalRuntimeMax);
#endif
    Serial.println();
}

#ifdef ESP8266
// for ESP8266
uint32_t TaskManager::free_memory()
{
    return ESP.getFreeHeap();
}

#else
// forMEGA2560
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

uint32_t TaskManager::free_memory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

#endif // ESP8266