/**
  *****************************************************************************
  * @file    zf_timer.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   软件定时器(精度1ms)的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
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

/* 定时器处理程序 */
typedef void (*ITimerProcess)(void);

/* 定时器结构 */
typedef struct _Timer
{
    uint8_t Priority;               /* 事件优先级 */
    uint32_t Interval;              /* 时间间隔(ms) */
    uint32_t AlarmTime;             /* 定时到达时间 */
    bool IsAutoReset;               /* 重复运行(默认开) */
    bool IsRunning;                 /* 是否正在运行(默认关) */
    /* 事件的处理者，事件将推送到处理者的队列 */
    /* 不设置处理者则本地执行(挂载Timer_process的地方) */
    EventHandler *pEventHandler;
    /* 处理事件 */
    ITimerProcess TimerProcess;
    
    /* 开始定时器 */
    void (*Start)(struct _Timer * const pTimer);
    
    /* 关闭定时器 */
    void (*Stop)(struct _Timer * const pTimer);
    
    /* 重新运行定时器 */
    void (*Restart)(struct _Timer * const pTimer);
    
    /* 销毁定时器(释放空间) */
    bool (*Dispose)(struct _Timer * const pTimer);
} Timer;

/* 创建定时器(内部分配空间) */
bool Timer_create(Timer ** ppTimer);

/* 开始定时器 */
void Timer_start(Timer * const pTimer);

/* 关闭定时器 */
void Timer_stop(Timer * const pTimer);

/* 重新运行定时器 */
void Timer_restart(Timer * const pTimer);

/* 销毁定时器(释放空间) */
bool Timer_dispose(Timer * const pTimer);

/* 后台运行程序(放在1ms内的循环里边) */
void Timer_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_TIMER_H__ */

/******************************** END OF FILE ********************************/
