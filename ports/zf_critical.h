/**
  *****************************************************************************
  * @file    zf_critical.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   硬件相关操作(临界状态、现场保护与恢复、PendSV异常)的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_CRITICAL_H__
#define __ZF_CRITICAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "zf_task.h"

extern Task *pTopPriorityTask; /* 最高优先级任务 */
extern Task *pCurrentTask;     /* 当前任务 */

/* 中断控制及状态寄存器 SCB_ICSR */
#define NVIC_INT_CTRL 0xE000ED04

/* 系统优先级寄存器 SCB_SHPR3：bit16~23 */
#define NVIC_SYSPRI14 0xE000ED22

/* 触发PendSV异常的值 Bit28：PENDSVSET */
#define NVIC_PENDSVSET 0x10000000

/* PendSV 优先级的值(最低) */
#define NVIC_PENDSV_PRI 0xFF

/* 触发PendSV异常，用于任务切换 */
#define TASK_SWITCH() *((volatile uint32_t *)NVIC_INT_CTRL) = NVIC_PENDSVSET

/* 任务堆栈数据类型 */
typedef uint32_t ZF_STK_TYPE;

/* 中断寄存器的值 */
#define ZF_SR_VAL() uint32_t ZF_SR

/* 保存中断寄存器的值，然后关中断 */
#define ZF_INT_DIS() do                 \
{                                       \
    ZF_SR = ZF_SR_Save();               \
} while (0)

/* 恢复中断寄存器的值 */
#define ZF_INT_EN() do                  \
{                                       \
    ZF_SR_Restore(ZF_SR);               \
} while (0)

/* 进入临界区 */
#define ZF_CRITICAL_ENTER() do          \
{                                       \
    ZF_INT_DIS();                       \
} while (0)

/* 退出临界区 */
#define ZF_CRITICAL_EXIT() do           \
{                                       \
    ZF_INT_EN();                        \
} while (0)

/* 中断寄存器的值 */
extern uint32_t ZF_SR;

/* 保存中断寄存器的值，然后关中断 */
__asm uint32_t ZF_SR_Save(void);

/* 恢复中断寄存器的值 */
__asm void ZF_SR_Restore(uint32_t sr);

/* 关闭所有中断(但是不包括fault和NMI中断) */
__asm void INTERRUPT_DISABLE(void);

/* 开启中断总开关 */
__asm void INTERRUPT_ENABLE(void);

/* 初始化任务堆栈 */
void ZF_initTaskStack(Task *pTask, ITaskProcess taskProcess, void *parg);

/* 开启中断，并触发PendSV异常(用于系统第一次任务调度) */
__asm void SF_readyGo(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_CRITICAL_H__ */

/******************************** END OF FILE ********************************/
