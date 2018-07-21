/**
  *****************************************************************************
  * @file    zf_task.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ��������ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
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

/* �û��������ȼ�1-31,����ԽС,���ȼ�Խ�� */

/* ����������ȼ�(�û�������) */
#define TASK_HIGHEST_PRIORITY EVENT_HIGHEST_PRIORITY

/* ����������ȼ�(�û�������) */
#define TASK_LOWEST_PRIORITY EVENT_LOWEST_PRIORITY

/* ����������� */
#define TASK_SCHEDULE_ENABLE() Task_scheduleSwitch(true)

/* �ر�������� */
#define TASK_SCHEDULE_DISABLE() Task_scheduleSwitch(false)

/* ��ȡ������ȱ�־ */
#define TASK_IS_SCHEDULE_ON() mIsScheduleOn

/* ��ȡ����ϵͳ���б�־ */
#define TASK_IS_SYSTEM_RUN() mIsTaskSystemRun

/* ��ȡ�������� */
#define TASK_GET_IDLE_TASK() pIdleTask

/* ��ȡ���������¼������� */
#define TASK_GET_IDLE_TASK_HANDLER() pIdleTaskEventHandler

/* ��ȡ�����б� */
#define TASK_GET_TASK_LIST() Task_getTaskList()

/* ��ȡ�������� */
#define TASK_GET_TASK_COUNT() TASK_GET_TASK_LIST()->Count

/* ��������ʱ�� */
#define TASK_TIMER_CREATE(ppTimer_) do                     \
{                                                          \
    Timer_create(ppTimer_);                                \
    if (*ppTimer_ != NULL)                                 \
    {                                                      \
        *ppTimer_->pEventHandler = pIdleTaskEventHandler;  \
    }                                                      \
} while(0)

typedef void (*ITaskProcess)(void *pArg); /* ������������ */

/* ����״̬ */
typedef enum _TaskState
{
    TASK_STATE_STOP = 0,          /* ֹͣ */
    TASK_STATE_RUNNING            /* ���� */
} TaskState;

/* ����ṹ */
typedef struct _Task
{
    uint32_t *pStkPtr;            /* ��ջָ�� */
    uint32_t *pStkBase;           /* ��ջ����ַ */
    uint32_t StkSize;             /* ��ջ��С */
    uint32_t DelayTime;           /* ������ʱʱ��(ϵͳ����) */
    uint8_t Priority;             /* �������ȼ� */
    uint8_t State;                /* ����״̬ */
    uint32_t RunTime;             /* ����������ʱ��(ϵͳ����) */
    
    /* ��ʼ���� */
    bool (*Start)(struct _Task * const pTask);
    
    /* ֹͣ���� */
    bool (*Stop)(struct _Task * const pTask);
    
    /* �������� */
    void (*Dispose)(struct _Task * const pTask);
    
    /* ��ʱ���� */
    bool (*Delay)(struct _Task * const pTask, uint32_t tick);
} Task;

extern Task *pTopPriorityTask;              /* ������ȼ����� */
extern Task *pCurrentTask;                  /* ��ǰ���� */
extern Task *pIdleTask;                     /* �������� */
extern EventHandler *pIdleTaskEventHandler; /* ���������¼������� */
extern bool mIsScheduleOn;                  /* ������ȿ��ı�־ */
extern bool mIsTaskSystemRun;               /* ����ϵͳ�Ƿ�ʼ�ı�־ */

/* ��ȡ�����б� */
List *Task_getTaskList(void);

/* �������� */
bool Task_create(Task **ppTask, ITaskProcess taskProcess, void *pArg,
    uint8_t priority, uint32_t stkSize);

/* ��ʼ���� */
bool Task_start(Task * const pTask);

/* ֹͣ���� */
bool Task_stop(Task * const pTask);

/* �������� */
void Task_dispose(Task * const pTask);

/* ��ʱ���� */
void Task_delay(struct _Task * const pTask, uint32_t tick);

/* ������ȿ��� */
void Task_scheduleSwitch(bool on);

/* ������� */
void Task_schedule(void);

/* ����ʱ����� */
void Task_timeUpdate(void);

/* ������������,���򲻷��� */
void Task_run(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TASK_H__ */

/******************************** END OF FILE ********************************/
