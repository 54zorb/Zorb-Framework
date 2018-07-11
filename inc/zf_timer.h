/**
  *****************************************************************************
  * @file    zf_timer.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   �����ʱ��(����1ms)��ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_TIMER_H__
#define __ZF_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "zf_event.h"

/* ��ʱ��������� */
typedef void (*ITimerProcess)(void);

/* ��ʱ���ṹ */
typedef struct _Timer
{
    uint8_t Priority;               /* �¼����ȼ� */
    uint32_t Interval;              /* ʱ����(ms) */
    uint32_t AlarmTime;             /* ��ʱ����ʱ�� */
    bool IsAutoReset;               /* �ظ�����(Ĭ�Ͽ�) */
    bool IsRunning;                 /* �Ƿ���������(Ĭ�Ϲ�) */
    /* �¼��Ĵ����ߣ��¼������͵������ߵĶ��� */
    /* �����ô������򱾵�ִ��(����Timer_process�ĵط�) */
    EventHandler *pEventHandler;
    /* �����¼� */
    ITimerProcess TimerProcess;
    
    /* ��ʼ��ʱ�� */
    void (*Start)(struct _Timer * const pTimer);
    
    /* �رն�ʱ�� */
    void (*Stop)(struct _Timer * const pTimer);
    
    /* �������ж�ʱ�� */
    void (*Restart)(struct _Timer * const pTimer);
    
    /* ���ٶ�ʱ��(�ͷſռ�) */
    bool (*Dispose)(struct _Timer * const pTimer);
} Timer;

/* ������ʱ��(�ڲ�����ռ�) */
bool Timer_create(Timer ** ppTimer);

/* ��ʼ��ʱ�� */
void Timer_start(Timer * const pTimer);

/* �رն�ʱ�� */
void Timer_stop(Timer * const pTimer);

/* �������ж�ʱ�� */
void Timer_restart(Timer * const pTimer);

/* ���ٶ�ʱ��(�ͷſռ�) */
bool Timer_dispose(Timer * const pTimer);

/* ��̨���г���(����1ms�ڵ�ѭ�����) */
void Timer_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TIMER_H__ */

/******************************** END OF FILE ********************************/
