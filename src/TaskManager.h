#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include "Collection.h"

#define MEASURE_TASKS // флаг измерения временных характеритик задач

#ifndef TASK_COUNT_MAX
#define TASK_COUNT_MAX 40 // максимальное количество задач
#endif

#define TASK_NAME_LENGHT 8         // максимальная длина наименования задачи
#define TASK_PRIORITY_MIN 1         // минимальный приоритет задачи
#define TASK_PRIORITY_MAX 255       // максимальный приоритет задачи

typedef void (*CallBackFunc_t)(void *); // сигнатура call-back функции задачи
typedef uint8_t Priority_t;             // тип приоритета задачи
typedef uint32_t Period_t;              // тип счетчиков времени
typedef uint8_t Task_t;                   // тип ID задачи

// структура используется для сбора и анализа статистики работы задач
struct Runtime
{
#ifdef MEASURE_TASKS
    uint32_t count; // счетчик вызовов задачи
    uint32_t min;   // минимальное зарегистрированное время выполнения задачи, мсек.
    uint32_t max;   // максимальное зарегистрированное время выполнения задачи, мсек.
    uint32_t time;  // общее время выполнения задачи
#endif
    uint32_t lastCall;     // время когда нужно вызывать задачу, мсек.
    bool isEnable : 1;     // true - задача запущена, False - остановлена (stop)
    bool isAutoReload : 1; // true -  задача повторяется автоматически, false - после одного цикла переходит в режим stop
};
// Структура описания задачи
struct TaskDescription
{
    Task_t id;                     // id задачи
    char name[TASK_NAME_LENGHT]; // название задачи
    Priority_t priority;         // приоритет выполнения задачи (0 - самый маленький, 255 - самый большой)
    Period_t period;             // период вызова задачи, мсек.
    CallBackFunc_t callback;     // адрес задачи для вызова
    void *parameter;             // ссылка на параметр задачи
};

struct TaskItem
{
    TaskDescription description;
    Runtime runtime;
};

typedef Collection<TaskItem, TASK_COUNT_MAX> TaskCollection;

// обеспечивает централизованное управление и выполнение задачами по таймеру.
// Не использует таймеров микроконтроллера.
class TaskManager
{
public:
    // инициализация очереди задач
    // @param none
    static void begin();
    // добавляет задачу в пул задач
    // @param name название задачи (для удобства отладки)
    // @param priority приоритет выполнения задачи (0 - low, 255 - high)
    // @param period период вызова задачи в мсек.
    // @param callback функция вызова задачи
    // @returns ID созданной задачи
    static Task_t add_task(const char *name,
                         Priority_t priority,
                         Period_t period,
                         CallBackFunc_t callback,
                         void *parameter,
                         bool enable = true,
                         bool autoReload = true);
    // перебирает все задачи и проверяет их время вызова.
    // Если время пришло, то вызывает и обновляет отладочную информации
    // @param none
    static void do_tasks();
    // показывает статистику по всем задачам в виде таблицы
    // @param none
    static void show_status();
    // возвращает размер свободной памяти
    // @return размер свободной памяти
    static uint32_t free_memory();
    // запускает сначала таймер задачи
    // @param id id задачи
    static void reset_timer(Task_t id);
    // разрешает работу таймеру задачи и запускает его сначала
    // @param id id задачи
    static void enable_timer(Task_t id);
    // запрещает работу таймера задачи
    // @param id id задачи
    static void disable_timer(Task_t id);
    // устанавливает режим автоматической перезагрузки таймера
    // @param id id задачи
    // @param isAutoReload true - цикличный таймер. false - one shot
    static void set_autoreload(Task_t id, bool isAutoReload);
    // устанавливает время таймера
    // @param id id задачи
    // @param time новое значение времени
    static void set_time(Task_t id, Period_t time);
    // возвращает статус работы таймера задачи
    // @param id id задачи
    static bool is_timer_enable(Task_t id);

private:
    TaskManager(){};
    static TaskCollection _tasks; // коллекция задач
#ifdef MEASURE_TASKS
    static uint32_t _totalRuntimeMax;   // максимальное время выполнения всех задач
    static uint32_t _totalRuntimeMin;   // максимальное время выполнения всех задач
    static uint32_t _totalRuntimeCount; // общее число циклов перебора задач
    static uint32_t _totalRuntime;      // общее время перебора задач
#endif
    // возвращает ссылку на задачу по его ID
    // @param id id задачи
    // @return ссылка на задачу в коллекции или NULL если не найдено
    static TaskItem *get_task_link(Task_t id);
};

#endif