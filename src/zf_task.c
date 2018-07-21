/**
  *****************************************************************************
  * @file    zf_task.c
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

#include "zf_task.h"
#include "zf_task_schedule.h"
#include "zf_critical.h"
#include "zf_malloc.h"
#include "zf_debug.h"
#include "zf_assert.h"
#include "zf_list.h"

/* ���������ջ��С */
#define IDLE_TASK_STK_SIZE 512  /* 4�ֽڶ��� */

List *pmTaskList = NULL;        /* �����б� */
Task *pTopPriorityTask = NULL;  /* ������ȼ����� */
Task *pCurrentTask = NULL;      /* ��ǰ���� */
Task *pIdleTask = NULL;         /* �������� */
 /* ���������¼������� */
EventHandler *pIdleTaskEventHandler;

/* ������ȿ��ı�־ */
bool mIsScheduleOn = true;

/* ����ϵͳ�Ƿ�ʼ�ı�־ */
bool mIsTaskSystemRun = false;

/* ����������� */
static void Task_idleTaskProcess(void *pArg);

/******************************************************************************
 * ����  ����ȡ�����б�
 * ����  ����
 * ����  �������б�ָ��
******************************************************************************/
List *Task_getTaskList(void)
{
    return pmTaskList;
}

/******************************************************************************
 * ����  ����������
 * ����  ��(out)-ppTask        ����ָ��ĵ�ַ
 *         (in)-taskProcess    ��������ָ��
 *         (in)-parg           �������Ĳ���ָ��(��̬�������Լ�����)
 *         (in)-priority       ��������ȼ�
 *         (in)-stkSize        ����Ķ�ջ��С(�ڲ�����)
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool Task_create(Task **ppTask, ITaskProcess taskProcess, void *pArg,
    uint8_t priority, uint32_t stkSize)
{
    Task *pTask;
    uint32_t *pStkBase;
    ListNode *pNode;
    
    ZF_ASSERT(ppTask != (Task **)0)
    ZF_ASSERT(taskProcess != (ITaskProcess)0)
    ZF_ASSERT(stkSize > 255)
    ZF_ASSERT(priority <= TASK_LOWEST_PRIORITY)
    
    /* ��������ռ� */
    if (!List_mallocNode(&pNode, (void **)&pTask, sizeof(Task)))
    {
        ZF_DEBUG(LOG_E, "malloc task space error\r\n");
        
        return false;
    }
    
    pTask->Priority = priority;
    pTask->State = TASK_STATE_RUNNING;
    pTask->DelayTime = 0;
    pTask->RunTime = 0;
    
    /* �����ջ�ռ� */
    pStkBase = ZF_MALLOC(stkSize);
    if (pTask == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc task stack space error\r\n");
        
        return false;
    }
    
    pTask->pStkBase = pStkBase;
    pTask->StkSize = stkSize;
    
    /* ��ʼ����ջֵ */
    ZF_initTaskStack(pTask, taskProcess, pArg);
    
    /* ��ʼ������ */
    pTask->Start = Task_start;
    pTask->Stop = Task_stop;
    pTask->Dispose = Task_dispose;
    
    /* ���������б� */
    if (pmTaskList == NULL)
    {
        List_create(&pmTaskList);
        
        if (pmTaskList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc task list space error\r\n");
            
            /* ��� */
            *ppTask = NULL;
            
            return false;
        }
    }
    
    /* �ر�������� */
    TASK_SCHEDULE_DISABLE();
    
    /* ��ӵ������б� */
    pmTaskList->Add(pmTaskList, pNode);
    
    /* ����������� */
    TASK_SCHEDULE_ENABLE();
    
    /* ��� */
    *ppTask = pTask;
    
    return true;
}

/******************************************************************************
 * ����  ����ʼ����(�����״̬��Task_run������������ִ�е����)
 * ����  ��(in)-pTask  ����ָ��
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool Task_start(Task * const pTask)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    /* �ر�������� */
    TASK_SCHEDULE_DISABLE();
    
    pTask->State = TASK_STATE_RUNNING;
    
    /* ����������� */
    TASK_SCHEDULE_ENABLE();
    
    return true;
}

/******************************************************************************
 * ����  ��ֹͣ����
 * ����  ��(in)-pTask  ����ָ��
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool Task_stop(Task * const pTask)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    /* ���������ܹر� */
    if (pTask == pIdleTask)
    {
        return false;
    }
    
    /* Ҫ�رյ������������� */
    if (mIsTaskSystemRun && pTask->State == TASK_STATE_RUNNING)
    {
        /* �ر�������� */
        TASK_SCHEDULE_DISABLE();
        
        pTask->State = TASK_STATE_STOP;
        
        /* ����������� */
        TASK_SCHEDULE_ENABLE();
        
        /* �������� */
        Task_schedule();
    }
    else
    {
        /* �ر�������� */
        TASK_SCHEDULE_DISABLE();
        
        pTask->State = TASK_STATE_STOP;
        
        /* ����������� */
        TASK_SCHEDULE_ENABLE();
    }
    
    return true;
}

/******************************************************************************
 * ����  ����������ִ����������
 * ����  ��(in)-pArgList  �����б�
 * ����  ����
******************************************************************************/
static void Task_disposeByIdleTask(List *pArgList)
{
    /* SR���� */
    ZF_SR_VAL();
    
    ListNode *pNode;
    Task *pTask;
    
    ZF_ASSERT(pArgList != (List *)0)
    ZF_ASSERT(pmTaskList != (List *)0)
    
    /* �������������� */
    if (pTask == pIdleTask)
    {
        return;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* �ڵ�0������ָ�������Ϊ����ָ�� */
    pTask = *((Task **)pArgList->GetElementDataAt(pArgList, 0));
    
    /* �Ƴ����� */
    while(pmTaskList->GetElementByData(pmTaskList, pTask, &pNode))
    {
        if (pNode == NULL)
        {
            break;
        }
        
        if (!pmTaskList->Delete(pmTaskList, pNode))
        {
            ZF_DEBUG(LOG_E, "delete task node from list error\r\n");
            
            break;
        }
    }
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
}

/******************************************************************************
 * ����  ����������(�������ٶ����ڿ����������)
 * ����  ��(in)-pTask  ����ָ��
 * ����  ����
******************************************************************************/
void Task_dispose(Task * const pTask)
{
    /* SR���� */
    ZF_SR_VAL();
    
    Event *pEvent;
    
    ZF_ASSERT(pTask != (Task *)0)
    ZF_ASSERT(pmTaskList != (List *)0)
    
    /* �������������� */
    if (pTask == pIdleTask)
    {
        return;
    }
    
    /* ���������¼� */
    Event_create(&pEvent);
    
    pEvent->Priority = TASK_HIGHEST_PRIORITY;
    pEvent->EventProcess = (IEventProcess)Task_disposeByIdleTask;
    /* ����Ϊ�����ָ�� */
    pEvent->AddArg(pEvent, (void *)&pTask, sizeof(Task *));
    
    /* ���������¼����������� */
    EVENT_POST(pIdleTaskEventHandler, pEvent);
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* ֹͣ���� */
    pTask->State = TASK_STATE_STOP;
    
    /* ���ÿ�������ȼ���� */
    pIdleTask->Priority = TASK_HIGHEST_PRIORITY;
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    /* ������� */
    Task_schedule();
}

/******************************************************************************
 * ����  ����ʱ����
 * ����  ��(in)-pTask  ����ָ��
 *         (in)-time   Ҫ��ʱ��ϵͳ������
 * ����  ����
******************************************************************************/
void Task_delay(struct _Task * const pTask, uint32_t tick)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    pTask->DelayTime = tick;
}

/******************************************************************************
 * ����  ��������ȿ���
 * ����  ��(in)-on      true:������ flase:�ص���
 * ����  ����
******************************************************************************/
void Task_scheduleSwitch(bool on)
{
    mIsScheduleOn = on;
}

/******************************************************************************
 * ����  ���������
 * ����  ����
 * ����  ����
******************************************************************************/
void Task_schedule(void)
{
    /* SR���� */
    ZF_SR_VAL();
    
    /* �������û�����˳� */
    if (!mIsTaskSystemRun || !mIsScheduleOn)
    {
        return;
    }
    
    /* û���������˳� */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        return;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* ����������ȼ������� */
    pTopPriorityTask = Task_getTopPriorityTask();
    
    /* ��ȡ����ʧ�� */
    if (pTopPriorityTask == NULL)
    {
        /* �˳��ٽ��� */
        ZF_CRITICAL_EXIT();
        
        ZF_DEBUG(LOG_D, "\r\n");
        ZF_DEBUG(LOG_E, "get top priority task error\r\n");
        ZF_DEBUG(LOG_E, "task schedule stop\r\n");
        
        /* �ر�������� */
        TASK_SCHEDULE_DISABLE();
        
        return;
    }
    
    /* ���������ȼ��������ǵ�ǰ������ֱ�ӷ��أ������������л� */
    if (pTopPriorityTask == pCurrentTask)
    {
        /* �˳��ٽ��� */
        ZF_CRITICAL_EXIT();
        
        return;
    }
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    /* �����л� */
    TASK_SWITCH();
}

/******************************************************************************
 * ����  ������ʱ�����
 * ����  ����
 * ����  ����
******************************************************************************/
void Task_timeUpdate(void)
{
    /* SR���� */
    ZF_SR_VAL();
    
    uint32_t i;
    ListNode *pNode;
    Task *pTask;
    
    /* ����ϵͳû�������˳� */
    if (!mIsTaskSystemRun)
    {
        return;
    }
    
    /* û���������˳� */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        return;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* ������������ */
    for (i = 0; i < pmTaskList->Count; i++)
    {
        if (pmTaskList->GetElementAt(pmTaskList, i, &pNode))
        {
            pTask = (Task *)pNode->pData;
            
            /* ������ʱ������ */
            if (pTask->DelayTime > 0)
            {
                pTask->DelayTime--;
            }
            
            /* ��������ʱ����� */
            if (pTask == pCurrentTask)
            {
                pTask->RunTime++;
            }
        }
    }
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
}

/******************************************************************************
 * ����  �������������
 * ����  ��(in)-pArg   �����б�
 * ����  ����
******************************************************************************/
void Task_idleTaskProcess(void *pArg)
{
    Event *pEvent;
    
    while(1)
    {
        /* �¼����������� */
        pIdleTaskEventHandler->Execute(pIdleTaskEventHandler);
        
        /* �����¼����� */
        if (pIdleTaskEventHandler->GetEventCount(pIdleTaskEventHandler) > 0)
        {
            /* ��ȡ�¼� */
            pEvent = (Event *)pIdleTaskEventHandler->pEventList
                ->GetElementDataAt(pIdleTaskEventHandler->pEventList, 0);
            
            if (pEvent == NULL || pEvent->EventProcess == NULL)
            {
                ZF_DEBUG(LOG_E, "idle task get event error:event is null\r\n");
                while(1);
            }
            
            /* ������������ȼ������¼������ȼ� */
            pIdleTask->Priority = pEvent->Priority;
            
            /* ��ʼ���� */
            Task_schedule();
        }
        /* û���¼����� */
        else
        {
            /* ��������ָ���������ȼ� */
            if (pIdleTask->Priority != TASK_LOWEST_PRIORITY)
            {
                pIdleTask->Priority = TASK_LOWEST_PRIORITY;
                
                /* ��ʼ���� */
                Task_schedule();
            }
            
            /* ��ʵ�ֵ͹��� */
        }
    }
}

/******************************************************************************
 * ����  ��������������,���򲻷���
 * ����  ����
 * ����  ����
******************************************************************************/
void Task_run(void)
{
    /* û���������˳� */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        ZF_DEBUG(LOG_E, "run task system error\r\n");
        return;
    }
    
    /* ������������ */
    if (!Task_create(&pIdleTask, Task_idleTaskProcess, NULL,
        TASK_LOWEST_PRIORITY, IDLE_TASK_STK_SIZE))
    {
        ZF_DEBUG(LOG_E, "create idle task error\r\n");
        
        while(1);
    }
    
    /* �������������¼������� */
    EventHandler_create(&pIdleTaskEventHandler);
    
    /* ���ϵͳ���� */
    mIsTaskSystemRun = true;
    
    /* ����������ȼ������� */
    pTopPriorityTask = Task_getTopPriorityTask();
    
    /* ����������� */
    SF_readyGo();
}

/******************************** END OF FILE ********************************/
