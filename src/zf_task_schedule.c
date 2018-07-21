/**
  *****************************************************************************
  * @file    zf_task_schedule.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   任务调度算法的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_task_schedule.h"
#include "zf_assert.h"
#include "stdlib.h"

/******************************************************************************
 * 描述  ：获取最高优先级任务(抢占式任务调度)
 * 参数  ：无
 * 返回  ：最高优先级任务
******************************************************************************/
Task *Task_getTopPriorityTask(void)
{
    Task *pTask = NULL;
    List *pTaskList;
    
    pTaskList = TASK_GET_TASK_LIST();
    if (pTaskList == NULL)
    {
        return NULL;
    }
    
    if (pTaskList->Count > 0)
    {
        Task *pTaski = NULL;
        uint32_t i;
        
        pTask = TASK_GET_IDLE_TASK();
        if (pTask == NULL)
        {
            return NULL;
        }
        
        /* 遍历所有任务 */
        for (i = 0; i < pTaskList->Count; i++)
        {
            pTaski = (Task *)pTaskList->GetElementDataAt(pTaskList, i);
            if (pTaski != NULL)
            {
                /* 任务在运行状态，同时不在延时则可以参与优先级排序 */
                if (pTaski->State == TASK_STATE_RUNNING
                    && pTaski->DelayTime == 0)
                {
                    /* 优先级排序 */
                    if (pTask->Priority > pTaski->Priority)
                    {
                        pTask = pTaski;
                    }
                }
            }
        }
        
        if (pTask->State != TASK_STATE_RUNNING || pTask->DelayTime > 0)
        {
            pTask = NULL;
        }
    }
    
    return pTask;
}

/******************************** END OF FILE ********************************/
