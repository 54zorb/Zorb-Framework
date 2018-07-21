/**
  *****************************************************************************
  * @file    zf_task_schedule.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ��������㷨��ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_TASK_SCHEDULE_H__
#define __ZF_TASK_SCHEDULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "zf_task.h"

/* ��ȡ������ȼ����� */
Task *Task_getTopPriorityTask(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TASK_SCHEDULE_H__ */

/******************************** END OF FILE ********************************/
