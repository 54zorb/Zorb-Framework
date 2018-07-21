/**
  *****************************************************************************
  * @file    zf_critical.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   Ӳ����ز���(�ٽ�״̬���ֳ�������ָ���PendSV�쳣)��ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
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

extern Task *pTopPriorityTask; /* ������ȼ����� */
extern Task *pCurrentTask;     /* ��ǰ���� */

/* �жϿ��Ƽ�״̬�Ĵ��� SCB_ICSR */
#define NVIC_INT_CTRL 0xE000ED04

/* ϵͳ���ȼ��Ĵ��� SCB_SHPR3��bit16~23 */
#define NVIC_SYSPRI14 0xE000ED22

/* ����PendSV�쳣��ֵ Bit28��PENDSVSET */
#define NVIC_PENDSVSET 0x10000000

/* PendSV ���ȼ���ֵ(���) */
#define NVIC_PENDSV_PRI 0xFF

/* ����PendSV�쳣�����������л� */
#define TASK_SWITCH() *((volatile uint32_t *)NVIC_INT_CTRL) = NVIC_PENDSVSET

/* �����ջ�������� */
typedef uint32_t ZF_STK_TYPE;

/* �жϼĴ�����ֵ */
#define ZF_SR_VAL() uint32_t ZF_SR

/* �����жϼĴ�����ֵ��Ȼ����ж� */
#define ZF_INT_DIS() do                 \
{                                       \
    ZF_SR = ZF_SR_Save();               \
} while (0)

/* �ָ��жϼĴ�����ֵ */
#define ZF_INT_EN() do                  \
{                                       \
    ZF_SR_Restore(ZF_SR);               \
} while (0)

/* �����ٽ��� */
#define ZF_CRITICAL_ENTER() do          \
{                                       \
    ZF_INT_DIS();                       \
} while (0)

/* �˳��ٽ��� */
#define ZF_CRITICAL_EXIT() do           \
{                                       \
    ZF_INT_EN();                        \
} while (0)

/* �жϼĴ�����ֵ */
extern uint32_t ZF_SR;

/* �����жϼĴ�����ֵ��Ȼ����ж� */
__asm uint32_t ZF_SR_Save(void);

/* �ָ��жϼĴ�����ֵ */
__asm void ZF_SR_Restore(uint32_t sr);

/* �ر������ж�(���ǲ�����fault��NMI�ж�) */
__asm void INTERRUPT_DISABLE(void);

/* �����ж��ܿ��� */
__asm void INTERRUPT_ENABLE(void);

/* ��ʼ�������ջ */
void ZF_initTaskStack(Task *pTask, ITaskProcess taskProcess, void *parg);

/* �����жϣ�������PendSV�쳣(����ϵͳ��һ���������) */
__asm void SF_readyGo(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_CRITICAL_H__ */

/******************************** END OF FILE ********************************/
