/**
  *****************************************************************************
  * @file    zf_timer.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   �����ʱ��(����1ms)��ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_timer.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_list.h"
#include "zf_malloc.h"
#include "zf_time.h"
#include "zf_task.h"

/* �򿪶�ʱ�����г��� */
#define TIMER_PROCESS_ENABLE() mIsTimerProcessOn = true
/* �رն�ʱ�����г��� */
#define TIMER_PROCESS_DISABLE() mIsTimerProcessOn = false

/* ��ʱ�����г��򿪵ı�־ */
static bool mIsTimerProcessOn = false;

/* ��ʱ���б� */
static List *pmTimerList = NULL;

/******************************************************************************
 * ����  ��ˢ�¶�ʱ����ʱ����ʱ��
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
static void RefreshAlarmTime(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    pTimer->AlarmTime = ZF_SYSTIME_MS() + pTimer->Interval;
}

/******************************************************************************
 * ����  ��������ʱ��(�ڲ�����ռ�)
 * ����  ��(out)-ppTimer ��ʱ��ָ���ָ��
 * ����  ����
******************************************************************************/
bool Timer_create(Timer **ppTimer)
{
    ListNode *pNode;
    Timer *pTimer;
    
    ZF_ASSERT(ppTimer != (Timer **)0)
    
    if (!List_mallocNode(&pNode, (void **)&pTimer, sizeof(Timer)))
    {
        ZF_DEBUG(LOG_E, "malloc timer space error\r\n");
        
        return false;
    }
    
    pTimer->Priority = EVENT_LOWEST_PRIORITY;
    pTimer->Interval = 0;
    pTimer->IsAutoReset = true;
    pTimer->IsRunning = false;
    pTimer->pEventHandler = NULL;
    pTimer->TimerProcess = NULL;
    
    pTimer->Start = Timer_start;
    pTimer->Stop = Timer_stop;
    pTimer->Restart = Timer_restart;
    pTimer->Dispose = Timer_dispose;
    
    /* ������ʱ���б� */
    if (pmTimerList == NULL)
    {
        if (!List_create(&pmTimerList))
        {
            ZF_DEBUG(LOG_E, "malloc timer list space error\r\n");
            
            /* ��� */
            *ppTimer = NULL;
            return false;
        }
        
        /* �򿪶�ʱ�����г��� */
        TIMER_PROCESS_ENABLE();
    }
    
    /* ��ӵ���ʱ���б� */
    pmTimerList->Add(pmTimerList, pNode);
    
    /* ��� */
    *ppTimer = pTimer;
    
    return true;
}

/******************************************************************************
 * ����  ����ʼ��ʱ��
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
void Timer_start(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    if (!pTimer->IsRunning)
    {
        RefreshAlarmTime(pTimer);
        pTimer->IsRunning = true;
    }
}

/******************************************************************************
 * ����  ���رն�ʱ��
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
void Timer_stop(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    pTimer->IsRunning = false;
}

/******************************************************************************
 * ����  ���������ж�ʱ��
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
void Timer_restart(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    Timer_stop(pTimer);
    
    Timer_start(pTimer);
}

/******************************************************************************
 * ����  �����ٶ�ʱ��(�ͷſռ�)
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
bool Timer_dispose(Timer * const pTimer)
{
    ListNode *pNode;
    
    ZF_ASSERT(pTimer != (Timer *)0)
    ZF_ASSERT(pmTimerList != (List *)0)
    
    Timer_stop(pTimer);
    
    /* ɾ����ʱ�� */
    while(pmTimerList->GetElementByData(pmTimerList, pTimer, &pNode))
    {
        if (pNode == NULL)
        {
            break;
        }
        
        if (!pmTimerList->Delete(pmTimerList, pNode))
        {
            ZF_DEBUG(LOG_E, "delete timer node from list error\r\n");
            
            break;
        }
    }
    
    return true;
}

/******************************************************************************
 * ����  �����Ͷ�ʱ�¼�
 * ����  ��(in)-pTimer ��ʱ��ָ��
 * ����  ����
******************************************************************************/
static void Timer_postEvent(Timer *pTimer)
{
    /* �����¼� */
    Event *pEvent;
    Event_create(&pEvent);
    pEvent->Priority = pTimer->Priority;
    pEvent->EventProcess = (IEventProcess)pTimer->TimerProcess;
    pEvent->pArgList = NULL; 
    
    /* �����¼� */
    EVENT_POST(pTimer->pEventHandler, pEvent);
    
    /* ����ʱ�� */
    if (pTimer->pEventHandler == TASK_GET_IDLE_TASK_HANDLER())
    {
        /* ���ÿ�������ȼ� */
        if (TASK_GET_IDLE_TASK()->Priority > pTimer->Priority)
        {
            TASK_GET_IDLE_TASK()->Priority = pTimer->Priority;
        }
    }
}

/******************************************************************************
 * ����  ����̨���г���(����1ms�ڵ�ѭ�����)
 * ����  ����
 * ����  ����
******************************************************************************/
void Timer_process(void)
{
    /* û�д򿪶�ʱ�����г����򷵻� */
    if (!mIsTimerProcessOn)
    {
        return;
    }
    
    if (pmTimerList != NULL && pmTimerList->Count > 0)
    {
        int i = 0;
        ListNode *pNode;
        Timer *pTimer;
        
        /* �������ж�ʱ�� */
        for (i = 0; i < pmTimerList->Count; i++)
        {
            if (pmTimerList->GetElementAt(pmTimerList, i, &pNode))
            {
                pTimer = (Timer *)pNode->pData;
                
                /* ��ʱ���ڹ��� */
                if (pTimer->IsRunning)
                {
                    /* ���ﶨʱʱ�� */
                    if (ZF_SYSTIME_MS() >= pTimer->AlarmTime)
                    {
                        /* �ظ����� */
                        if(pTimer->IsAutoReset)
                        {
                            pTimer->Restart(pTimer);
                        }
                        else
                        {
                            pTimer->Stop(pTimer);
                        }
                        
                        /* ִ���¼� */
                        if (pTimer->TimerProcess != NULL)
                        {
                            if (pTimer->pEventHandler == NULL)
                            {
                                /* ����ִ�� */
                                pTimer->TimerProcess();
                            }
                            else
                            {
                                /* �����¼� */
                                Timer_postEvent(pTimer);
                            }
                        }
                    }
                }
            }
        }
    }
}

/******************************** END OF FILE ********************************/
