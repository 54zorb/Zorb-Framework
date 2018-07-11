/**
  *****************************************************************************
  * @file    zf_time.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ϵͳʱ���ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_TIME_H__
#define __ZF_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

/* ϵͳ�δ�����(ms) */
#define ZF_TICK_PERIOD 1

/* ��ȡϵͳ�δ��� */
#define ZF_SYSTICK() ZF_getSystemTick()

/* ��ȡϵͳʱ��(ms) */
#define ZF_SYSTIME_MS() ZF_getSystemTimeMS()

/* ϵͳ��ʱ(ms) */
#define ZF_DELAY_MS(ms_) do                            \
{                                                      \
    if (ms_ % ZF_TICK_PERIOD)                          \
    {                                                  \
        ZF_delayTick((ms_ / ZF_TICK_PERIOD) + 1);      \
    }                                                  \
    else                                               \
    {                                                  \
        ZF_delayTick(ms_ / ZF_TICK_PERIOD);            \
    }                                                  \
} while(0)

/* ��ȡϵͳ�δ��� */
uint32_t ZF_getSystemTick(void);

/* ��ȡϵͳʱ��(ms) */
uint32_t ZF_getSystemTimeMS(void);

/* ϵͳ��ʱ */
void ZF_delayTick(uint32_t tick);

/* ϵͳ�δ����(�����Ӳ����ʱ���ж����) */
void ZF_timeTick (void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TIME_H__ */

/******************************** END OF FILE ********************************/
