/**
  *****************************************************************************
  * @file    zf_task.c
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

#include "zf_task.h"
#include "zf_task_schedule.h"
#include "zf_critical.h"
#include "zf_malloc.h"
#include "zf_debug.h"
#include "zf_assert.h"
#include "zf_list.h"

/* 空闲任务堆栈大小 */
#define IDLE_TASK_STK_SIZE 512  /* 4字节对齐 */

List *pmTaskList = NULL;        /* 任务列表 */
Task *pTopPriorityTask = NULL;  /* 最高优先级任务 */
Task *pCurrentTask = NULL;      /* 当前任务 */
Task *pIdleTask = NULL;         /* 空闲任务 */
 /* 空闲任务事件处理器 */
EventHandler *pIdleTaskEventHandler;

/* 任务调度开的标志 */
bool mIsScheduleOn = true;

/* 任务系统是否开始的标志 */
bool mIsTaskSystemRun = false;

/* 空闲任务程序 */
static void Task_idleTaskProcess(void *pArg);

/******************************************************************************
 * 描述  ：获取任务列表
 * 参数  ：无
 * 返回  ：任务列表指针
******************************************************************************/
List *Task_getTaskList(void)
{
    return pmTaskList;
}

/******************************************************************************
 * 描述  ：创建任务
 * 参数  ：(out)-ppTask        任务指针的地址
 *         (in)-taskProcess    任务程序的指针
 *         (in)-parg           任务程序的参数指针(动态任务需自己销毁)
 *         (in)-priority       任务的优先级
 *         (in)-stkSize        任务的堆栈大小(内部分配)
 * 返回  ：-true       成功
 *         -false      失败
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
    
    /* 分配任务空间 */
    if (!List_mallocNode(&pNode, (void **)&pTask, sizeof(Task)))
    {
        ZF_DEBUG(LOG_E, "malloc task space error\r\n");
        
        return false;
    }
    
    pTask->Priority = priority;
    pTask->State = TASK_STATE_RUNNING;
    pTask->DelayTime = 0;
    pTask->RunTime = 0;
    
    /* 分配堆栈空间 */
    pStkBase = ZF_MALLOC(stkSize);
    if (pTask == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc task stack space error\r\n");
        
        return false;
    }
    
    pTask->pStkBase = pStkBase;
    pTask->StkSize = stkSize;
    
    /* 初始化堆栈值 */
    ZF_initTaskStack(pTask, taskProcess, pArg);
    
    /* 初始化方法 */
    pTask->Start = Task_start;
    pTask->Stop = Task_stop;
    pTask->Dispose = Task_dispose;
    
    /* 创建任务列表 */
    if (pmTaskList == NULL)
    {
        List_create(&pmTaskList);
        
        if (pmTaskList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc task list space error\r\n");
            
            /* 输出 */
            *ppTask = NULL;
            
            return false;
        }
    }
    
    /* 关闭任务调度 */
    TASK_SCHEDULE_DISABLE();
    
    /* 添加到任务列表 */
    pmTaskList->Add(pmTaskList, pNode);
    
    /* 开启任务调度 */
    TASK_SCHEDULE_ENABLE();
    
    /* 输出 */
    *ppTask = pTask;
    
    return true;
}

/******************************************************************************
 * 描述  ：开始任务(仅标记状态，Task_run才是所有任务执行的入口)
 * 参数  ：(in)-pTask  任务指针
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool Task_start(Task * const pTask)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    /* 关闭任务调度 */
    TASK_SCHEDULE_DISABLE();
    
    pTask->State = TASK_STATE_RUNNING;
    
    /* 开启任务调度 */
    TASK_SCHEDULE_ENABLE();
    
    return true;
}

/******************************************************************************
 * 描述  ：停止任务
 * 参数  ：(in)-pTask  任务指针
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool Task_stop(Task * const pTask)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    /* 空闲任务不能关闭 */
    if (pTask == pIdleTask)
    {
        return false;
    }
    
    /* 要关闭的任务正在运行 */
    if (mIsTaskSystemRun && pTask->State == TASK_STATE_RUNNING)
    {
        /* 关闭任务调度 */
        TASK_SCHEDULE_DISABLE();
        
        pTask->State = TASK_STATE_STOP;
        
        /* 开启任务调度 */
        TASK_SCHEDULE_ENABLE();
        
        /* 触发调度 */
        Task_schedule();
    }
    else
    {
        /* 关闭任务调度 */
        TASK_SCHEDULE_DISABLE();
        
        pTask->State = TASK_STATE_STOP;
        
        /* 开启任务调度 */
        TASK_SCHEDULE_ENABLE();
    }
    
    return true;
}

/******************************************************************************
 * 描述  ：空闲任务执行销毁任务
 * 参数  ：(in)-pArgList  参数列表
 * 返回  ：无
******************************************************************************/
static void Task_disposeByIdleTask(List *pArgList)
{
    /* SR变量 */
    ZF_SR_VAL();
    
    ListNode *pNode;
    Task *pTask;
    
    ZF_ASSERT(pArgList != (List *)0)
    ZF_ASSERT(pmTaskList != (List *)0)
    
    /* 空闲任务不能销毁 */
    if (pTask == pIdleTask)
    {
        return;
    }
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 节点0数据区指向的内容为任务指针 */
    pTask = *((Task **)pArgList->GetElementDataAt(pArgList, 0));
    
    /* 移除任务 */
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
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
}

/******************************************************************************
 * 描述  ：销毁任务(真正销毁动作在空闲任务完成)
 * 参数  ：(in)-pTask  任务指针
 * 返回  ：无
******************************************************************************/
void Task_dispose(Task * const pTask)
{
    /* SR变量 */
    ZF_SR_VAL();
    
    Event *pEvent;
    
    ZF_ASSERT(pTask != (Task *)0)
    ZF_ASSERT(pmTaskList != (List *)0)
    
    /* 空闲任务不能销毁 */
    if (pTask == pIdleTask)
    {
        return;
    }
    
    /* 创建销毁事件 */
    Event_create(&pEvent);
    
    pEvent->Priority = TASK_HIGHEST_PRIORITY;
    pEvent->EventProcess = (IEventProcess)Task_disposeByIdleTask;
    /* 参数为任务的指针 */
    pEvent->AddArg(pEvent, (void *)&pTask, sizeof(Task *));
    
    /* 推送销毁事件到空闲任务 */
    EVENT_POST(pIdleTaskEventHandler, pEvent);
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 停止任务 */
    pTask->State = TASK_STATE_STOP;
    
    /* 设置空闲任务等级最高 */
    pIdleTask->Priority = TASK_HIGHEST_PRIORITY;
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    /* 任务调度 */
    Task_schedule();
}

/******************************************************************************
 * 描述  ：延时任务
 * 参数  ：(in)-pTask  任务指针
 *         (in)-time   要延时的系统周期数
 * 返回  ：无
******************************************************************************/
void Task_delay(struct _Task * const pTask, uint32_t tick)
{
    ZF_ASSERT(pTask != (Task *)0)
    
    pTask->DelayTime = tick;
}

/******************************************************************************
 * 描述  ：任务调度开关
 * 参数  ：(in)-on      true:开调度 flase:关调度
 * 返回  ：无
******************************************************************************/
void Task_scheduleSwitch(bool on)
{
    mIsScheduleOn = on;
}

/******************************************************************************
 * 描述  ：任务调度
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
void Task_schedule(void)
{
    /* SR变量 */
    ZF_SR_VAL();
    
    /* 任务调度没开则退出 */
    if (!mIsTaskSystemRun || !mIsScheduleOn)
    {
        return;
    }
    
    /* 没有任务则退出 */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        return;
    }
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 查找最高优先级的任务 */
    pTopPriorityTask = Task_getTopPriorityTask();
    
    /* 获取任务失败 */
    if (pTopPriorityTask == NULL)
    {
        /* 退出临界区 */
        ZF_CRITICAL_EXIT();
        
        ZF_DEBUG(LOG_D, "\r\n");
        ZF_DEBUG(LOG_E, "get top priority task error\r\n");
        ZF_DEBUG(LOG_E, "task schedule stop\r\n");
        
        /* 关闭任务调度 */
        TASK_SCHEDULE_DISABLE();
        
        return;
    }
    
    /* 如果最高优先级的任务是当前任务则直接返回，不进行任务切换 */
    if (pTopPriorityTask == pCurrentTask)
    {
        /* 退出临界区 */
        ZF_CRITICAL_EXIT();
        
        return;
    }
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    /* 任务切换 */
    TASK_SWITCH();
}

/******************************************************************************
 * 描述  ：任务时间更新
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
void Task_timeUpdate(void)
{
    /* SR变量 */
    ZF_SR_VAL();
    
    uint32_t i;
    ListNode *pNode;
    Task *pTask;
    
    /* 任务系统没启动则退出 */
    if (!mIsTaskSystemRun)
    {
        return;
    }
    
    /* 没有任务则退出 */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        return;
    }
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 遍历所有任务 */
    for (i = 0; i < pmTaskList->Count; i++)
    {
        if (pmTaskList->GetElementAt(pmTaskList, i, &pNode))
        {
            pTask = (Task *)pNode->pData;
            
            /* 任务延时倒计数 */
            if (pTask->DelayTime > 0)
            {
                pTask->DelayTime--;
            }
            
            /* 任务运行时间计数 */
            if (pTask == pCurrentTask)
            {
                pTask->RunTime++;
            }
        }
    }
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
}

/******************************************************************************
 * 描述  ：空闲任务程序
 * 参数  ：(in)-pArg   参数列表
 * 返回  ：无
******************************************************************************/
void Task_idleTaskProcess(void *pArg)
{
    Event *pEvent;
    
    while(1)
    {
        /* 事件处理器运行 */
        pIdleTaskEventHandler->Execute(pIdleTaskEventHandler);
        
        /* 还有事件处理 */
        if (pIdleTaskEventHandler->GetEventCount(pIdleTaskEventHandler) > 0)
        {
            /* 获取事件 */
            pEvent = (Event *)pIdleTaskEventHandler->pEventList
                ->GetElementDataAt(pIdleTaskEventHandler->pEventList, 0);
            
            if (pEvent == NULL || pEvent->EventProcess == NULL)
            {
                ZF_DEBUG(LOG_E, "idle task get event error:event is null\r\n");
                while(1);
            }
            
            /* 空闲任务的优先级等于事件的优先级 */
            pIdleTask->Priority = pEvent->Priority;
            
            /* 开始调度 */
            Task_schedule();
        }
        /* 没有事件处理 */
        else
        {
            /* 空闲任务恢复到最低优先级 */
            if (pIdleTask->Priority != TASK_LOWEST_PRIORITY)
            {
                pIdleTask->Priority = TASK_LOWEST_PRIORITY;
                
                /* 开始调度 */
                Task_schedule();
            }
            
            /* 可实现低功耗 */
        }
    }
}

/******************************************************************************
 * 描述  ：运行所有任务,程序不返回
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
void Task_run(void)
{
    /* 没有任务则退出 */
    if (pmTaskList == NULL || pmTaskList->Count == 0)
    {
        ZF_DEBUG(LOG_E, "run task system error\r\n");
        return;
    }
    
    /* 创建空闲任务 */
    if (!Task_create(&pIdleTask, Task_idleTaskProcess, NULL,
        TASK_LOWEST_PRIORITY, IDLE_TASK_STK_SIZE))
    {
        ZF_DEBUG(LOG_E, "create idle task error\r\n");
        
        while(1);
    }
    
    /* 创建空闲任务事件处理器 */
    EventHandler_create(&pIdleTaskEventHandler);
    
    /* 标记系统运行 */
    mIsTaskSystemRun = true;
    
    /* 查找最高优先级的任务 */
    pTopPriorityTask = Task_getTopPriorityTask();
    
    /* 启动任务调度 */
    SF_readyGo();
}

/******************************** END OF FILE ********************************/
