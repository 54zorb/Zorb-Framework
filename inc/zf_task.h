/**
  *****************************************************************************
  * @file    zf_task.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   任务管理的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_TASK_H__
#define __ZF_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "zf_list.h"
#include "zf_timer.h"
#include "zf_event.h"

/* 用户任务优先级1-31,数字越小,优先级越高 */

/* 任务最高优先级(用户不可用) */
#define TASK_HIGHEST_PRIORITY EVENT_HIGHEST_PRIORITY

/* 任务最低优先级(用户不可用) */
#define TASK_LOWEST_PRIORITY EVENT_LOWEST_PRIORITY

/* 开启任务调度 */
#define TASK_SCHEDULE_ENABLE() Task_scheduleSwitch(true)

/* 关闭任务调度 */
#define TASK_SCHEDULE_DISABLE() Task_scheduleSwitch(false)

/* 获取任务调度标志 */
#define TASK_IS_SCHEDULE_ON() mIsScheduleOn

/* 获取任务系统运行标志 */
#define TASK_IS_SYSTEM_RUN() mIsTaskSystemRun

/* 获取空闲任务 */
#define TASK_GET_IDLE_TASK() pIdleTask

/* 获取空闲任务事件处理器 */
#define TASK_GET_IDLE_TASK_HANDLER() pIdleTaskEventHandler

/* 获取任务列表 */
#define TASK_GET_TASK_LIST() Task_getTaskList()

/* 获取任务数量 */
#define TASK_GET_TASK_COUNT() TASK_GET_TASK_LIST()->Count

/* 创建任务定时器 */
#define TASK_TIMER_CREATE(ppTimer_) do                     \
{                                                          \
    Timer_create(ppTimer_);                                \
    if (*ppTimer_ != NULL)                                 \
    {                                                      \
        *ppTimer_->pEventHandler = pIdleTaskEventHandler;  \
    }                                                      \
} while(0)

typedef void (*ITaskProcess)(void *pArg); /* 程序任务类型 */

/* 任务状态 */
typedef enum _TaskState
{
    TASK_STATE_STOP = 0,          /* 停止 */
    TASK_STATE_RUNNING            /* 运行 */
} TaskState;

/* 任务结构 */
typedef struct _Task
{
    uint32_t *pStkPtr;            /* 堆栈指针 */
    uint32_t *pStkBase;           /* 堆栈基地址 */
    uint32_t StkSize;             /* 堆栈大小 */
    uint32_t DelayTime;           /* 任务延时时间(系统周期) */
    uint8_t Priority;             /* 任务优先级 */
    uint8_t State;                /* 任务状态 */
    uint32_t RunTime;             /* 任务总运行时间(系统周期) */
    
    /* 开始任务 */
    bool (*Start)(struct _Task * const pTask);
    
    /* 停止任务 */
    bool (*Stop)(struct _Task * const pTask);
    
    /* 销毁任务 */
    void (*Dispose)(struct _Task * const pTask);
    
    /* 延时任务 */
    bool (*Delay)(struct _Task * const pTask, uint32_t tick);
} Task;

extern Task *pTopPriorityTask;              /* 最高优先级任务 */
extern Task *pCurrentTask;                  /* 当前任务 */
extern Task *pIdleTask;                     /* 空闲任务 */
extern EventHandler *pIdleTaskEventHandler; /* 空闲任务事件处理器 */
extern bool mIsScheduleOn;                  /* 任务调度开的标志 */
extern bool mIsTaskSystemRun;               /* 任务系统是否开始的标志 */

/* 获取任务列表 */
List *Task_getTaskList(void);

/* 创建任务 */
bool Task_create(Task **ppTask, ITaskProcess taskProcess, void *pArg,
    uint8_t priority, uint32_t stkSize);

/* 开始任务 */
bool Task_start(Task * const pTask);

/* 停止任务 */
bool Task_stop(Task * const pTask);

/* 销毁任务 */
void Task_dispose(Task * const pTask);

/* 延时任务 */
void Task_delay(struct _Task * const pTask, uint32_t tick);

/* 任务调度开关 */
void Task_scheduleSwitch(bool on);

/* 任务调度 */
void Task_schedule(void);

/* 任务时间更新 */
void Task_timeUpdate(void);

/* 运行所有任务,程序不返回 */
void Task_run(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TASK_H__ */

/******************************** END OF FILE ********************************/
