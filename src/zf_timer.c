/**
  *****************************************************************************
  * @file    zf_timer.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   软件定时器(精度1ms)的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
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

/* 打开定时器运行程序 */
#define TIMER_PROCESS_ENABLE() mIsTimerProcessOn = true
/* 关闭定时器运行程序 */
#define TIMER_PROCESS_DISABLE() mIsTimerProcessOn = false

/* 定时器运行程序开的标志 */
static bool mIsTimerProcessOn = false;

/* 定时器列表 */
static List *pmTimerList = NULL;

/******************************************************************************
 * 描述  ：刷新定时器定时到达时间
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
******************************************************************************/
static void RefreshAlarmTime(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    pTimer->AlarmTime = ZF_SYSTIME_MS() + pTimer->Interval;
}

/******************************************************************************
 * 描述  ：创建定时器(内部分配空间)
 * 参数  ：(out)-ppTimer 定时器指针的指针
 * 返回  ：无
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
    
    /* 创建定时器列表 */
    if (pmTimerList == NULL)
    {
        if (!List_create(&pmTimerList))
        {
            ZF_DEBUG(LOG_E, "malloc timer list space error\r\n");
            
            /* 输出 */
            *ppTimer = NULL;
            return false;
        }
        
        /* 打开定时器运行程序 */
        TIMER_PROCESS_ENABLE();
    }
    
    /* 添加到定时器列表 */
    pmTimerList->Add(pmTimerList, pNode);
    
    /* 输出 */
    *ppTimer = pTimer;
    
    return true;
}

/******************************************************************************
 * 描述  ：开始定时器
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
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
 * 描述  ：关闭定时器
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
******************************************************************************/
void Timer_stop(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    pTimer->IsRunning = false;
}

/******************************************************************************
 * 描述  ：重新运行定时器
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
******************************************************************************/
void Timer_restart(Timer * const pTimer)
{
    ZF_ASSERT(pTimer != (Timer *)0)
    
    Timer_stop(pTimer);
    
    Timer_start(pTimer);
}

/******************************************************************************
 * 描述  ：销毁定时器(释放空间)
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
******************************************************************************/
bool Timer_dispose(Timer * const pTimer)
{
    ListNode *pNode;
    
    ZF_ASSERT(pTimer != (Timer *)0)
    ZF_ASSERT(pmTimerList != (List *)0)
    
    Timer_stop(pTimer);
    
    /* 删除定时器 */
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
 * 描述  ：推送定时事件
 * 参数  ：(in)-pTimer 定时器指针
 * 返回  ：无
******************************************************************************/
static void Timer_postEvent(Timer *pTimer)
{
    /* 创建事件 */
    Event *pEvent;
    Event_create(&pEvent);
    pEvent->Priority = pTimer->Priority;
    pEvent->EventProcess = (IEventProcess)pTimer->TimerProcess;
    pEvent->pArgList = NULL; 
    
    /* 推送事件 */
    EVENT_POST(pTimer->pEventHandler, pEvent);
    
    /* 任务定时器 */
    if (pTimer->pEventHandler == TASK_GET_IDLE_TASK_HANDLER())
    {
        /* 设置空闲任务等级 */
        if (TASK_GET_IDLE_TASK()->Priority > pTimer->Priority)
        {
            TASK_GET_IDLE_TASK()->Priority = pTimer->Priority;
        }
    }
}

/******************************************************************************
 * 描述  ：后台运行程序(放在1ms内的循环里边)
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
void Timer_process(void)
{
    /* 没有打开定时器运行程序则返回 */
    if (!mIsTimerProcessOn)
    {
        return;
    }
    
    if (pmTimerList != NULL && pmTimerList->Count > 0)
    {
        int i = 0;
        ListNode *pNode;
        Timer *pTimer;
        
        /* 遍历所有定时器 */
        for (i = 0; i < pmTimerList->Count; i++)
        {
            if (pmTimerList->GetElementAt(pmTimerList, i, &pNode))
            {
                pTimer = (Timer *)pNode->pData;
                
                /* 定时器在工作 */
                if (pTimer->IsRunning)
                {
                    /* 到达定时时间 */
                    if (ZF_SYSTIME_MS() >= pTimer->AlarmTime)
                    {
                        /* 重复运行 */
                        if(pTimer->IsAutoReset)
                        {
                            pTimer->Restart(pTimer);
                        }
                        else
                        {
                            pTimer->Stop(pTimer);
                        }
                        
                        /* 执行事件 */
                        if (pTimer->TimerProcess != NULL)
                        {
                            if (pTimer->pEventHandler == NULL)
                            {
                                /* 本地执行 */
                                pTimer->TimerProcess();
                            }
                            else
                            {
                                /* 推送事件 */
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
