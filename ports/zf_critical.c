/**
  *****************************************************************************
  * @file    zf_critical.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   Ӳ����ز���(�ٽ�״̬���ֳ�������ָ���PendSV�쳣)��ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_critical.h"
#include "zf_assert.h"

/******************************************************************************
 * ����  �������жϼĴ�����ֵ��Ȼ����ж�
 * ����  ����
 * ����  ���жϼĴ�����ֵ
******************************************************************************/
__asm uint32_t ZF_SR_Save(void)
{
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}

/******************************************************************************
 * ����  ���ָ��жϼĴ�����ֵ
 * ����  ��(in)-sr �жϼĴ�����ֵ
 * ����  ����
******************************************************************************/
__asm void ZF_SR_Restore(uint32_t sr)
{
    MSR     PRIMASK, R0
    BX      LR
}

/******************************************************************************
 * ����  ���ر������ж�(���ǲ�����fault��NMI�ж�)
 * ����  ����
 * ����  ����
******************************************************************************/
__asm void INTERRUPT_DISABLE(void)
{
    CPSID   I
    BX      LR
}

/******************************************************************************
 * ����  �������ж��ܿ���
 * ����  ����
 * ����  ����
******************************************************************************/
__asm void INTERRUPT_ENABLE(void)
{
    CPSIE   I
    BX      LR
}

/******************************************************************************
 * ����  ����ʼ�������ջ
 * ����  ��(in)-pTask       ����ָ��
 *         (in)-taskProcess �������ָ��
 *         (in)-parg        �����������б�ָ��
 * ����  ����
******************************************************************************/
void ZF_initTaskStack(Task *pTask, ITaskProcess taskProcess, void *parg)
{
    ZF_STK_TYPE *pStkPtr;
    
    ZF_ASSERT(pTask != (Task *)0)
    ZF_ASSERT(taskProcess != (ITaskProcess)0)
    
    pStkPtr = (ZF_STK_TYPE *)((ZF_STK_TYPE)pTask->pStkBase
        + (ZF_STK_TYPE)pTask->StkSize);
    
    /* �쳣����ʱ�Զ�����ļĴ��� */
    *--pStkPtr = (ZF_STK_TYPE)0x01000000u;          /* xPSR��bit24������1 */
    *--pStkPtr = (ZF_STK_TYPE)taskProcess;          /* �������ڵ�ַ */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R14 (LR) */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R12 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R3 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R2 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R1 */
    *--pStkPtr = (ZF_STK_TYPE)parg;                 /* R0 : �����β� */
    
    /* �쳣����ʱ���ֶ�����ļĴ��� */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R11 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R10 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R9 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R8 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R7 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R6 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R5 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R4 */
    
    pTask->pStkPtr = pStkPtr;
}

/******************************************************************************
 * ����  �������жϣ�������PendSV�쳣(����ϵͳ��һ���������)
 * ����  ����
 * ����  ����
******************************************************************************/
__asm void SF_readyGo(void)
{
    /* ����PendSV�쳣���ȼ�Ϊ��� */
    LDR     R0, = NVIC_SYSPRI14
    LDR     R1, = NVIC_PENDSV_PRI
    STRB    R1, [R0]
    NOP
    
    /* ����psp��ֵΪ0����ʼ��һ���������л� */
    MOVS    R0, #0
    MSR     PSP, R0
    
    /* ����PendSV�쳣 */
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    
    /* ���ж� */
    CPSIE   I
    
/* һ����ѭ���������������������������� */
OSStartHang
    B       OSStartHang
    NOP
}

/******************************************************************************
 * ����  ��PendSV�쳣����
 * ����  ����
 * ����  ����
******************************************************************************/
__asm void PendSV_Handler(void)
{
    IMPORT  pCurrentTask
    IMPORT  pTopPriorityTask
    
    /* ����ı��棬����CPU�Ĵ�����ֵ�洢������Ķ�ջ�� */
    /* ���жϣ�NMI��HardFault���� */
    CPSID   I
    
    /* �ж��Ƿ��һ������ */
    MRS     R0, PSP
    CBZ     R0, PendSVHandler_NotSave
    
    /**
        �ڽ���PendSV�쳣��ʱ�򣬵�ǰCPU��xPSR��PC(������ڵ�ַ)��
        R14��R12��R3��R2��R1��R0���Զ��洢����ǰ�����ջ��ͬʱ�ݼ�PSP��ֵ
    **/
    /* �ֶ��洢CPU�Ĵ���R4-R11��ֵ����ǰ����Ķ�ջ */
    STMDB   R0!, {R4-R11}
    
    /* R0ָ��pCurrentTask�Ķ�ջָ��(ָ�����λ�õĶ���) */
    LDR     R1, = pCurrentTask
    LDR     R1, [R1]
    STR     R0, [R1]
    NOP
    
/* ������л���������һ��Ҫ���е�����Ķ�ջ���ݼ��ص�CPU�Ĵ����� */
PendSVHandler_NotSave

    /* ��Ч����pCurrentTask = pTopPriorityTask; */
    LDR     R0, = pCurrentTask
    LDR     R1, = pTopPriorityTask
    LDR     R2, [R1]
    STR     R2, [R0]
    
    /* pTopPriorityTask����Ϣ��ջ */
    LDR     R0, [R2]
    LDMIA   R0!, {R4-R11}
    
    /* ����PSPָ����һ��Ҫִ�е�����Ķ�ջ��ջ��(�ѵ����˼Ĵ�����Ϣ) */
    MSR     PSP, R0
    /* ȷ���쳣����ʹ�õĶ�ջָ����PSP */
    ORR     LR, LR, #0x04 /* ����LR�Ĵ�����λ2Ϊ1 */
    CPSIE   I /* ���ж� */
    
    /**
        �������أ����ʱ�������ջ�е�ʣ�����ݽ����Զ����ص�
        xPSR��PC��������ڵ�ַ����R14��R12��R3��R2��R1��R0��������βΣ�
        ͬʱPSP��ֵҲ�����£���ָ�������ջ��ջ����
        ��STM32�У���ջ���ɸߵ�ַ��͵�ַ������
    **/
    BX      LR
    NOP
}

/******************************** END OF FILE ********************************/
