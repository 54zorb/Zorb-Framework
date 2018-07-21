/**
  *****************************************************************************
  * @file    zf_task_schedule.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ��������㷨��ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_task_schedule.h"
#include "zf_assert.h"
#include "stdlib.h"

/******************************************************************************
 * ����  ����ȡ������ȼ�����(��ռʽ�������)
 * ����  ����
 * ����  ��������ȼ�����
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
        
        /* ������������ */
        for (i = 0; i < pTaskList->Count; i++)
        {
            pTaski = (Task *)pTaskList->GetElementDataAt(pTaskList, i);
            if (pTaski != NULL)
            {
                /* ����������״̬��ͬʱ������ʱ����Բ������ȼ����� */
                if (pTaski->State == TASK_STATE_RUNNING
                    && pTaski->DelayTime == 0)
                {
                    /* ���ȼ����� */
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
