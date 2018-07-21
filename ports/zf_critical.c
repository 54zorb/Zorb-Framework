/**
  *****************************************************************************
  * @file    zf_critical.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   硬件相关操作(临界状态、现场保护与恢复、PendSV异常)的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_critical.h"
#include "zf_assert.h"

/******************************************************************************
 * 描述  ：保存中断寄存器的值，然后关中断
 * 参数  ：无
 * 返回  ：中断寄存器的值
******************************************************************************/
__asm uint32_t ZF_SR_Save(void)
{
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}

/******************************************************************************
 * 描述  ：恢复中断寄存器的值
 * 参数  ：(in)-sr 中断寄存器的值
 * 返回  ：无
******************************************************************************/
__asm void ZF_SR_Restore(uint32_t sr)
{
    MSR     PRIMASK, R0
    BX      LR
}

/******************************************************************************
 * 描述  ：关闭所有中断(但是不包括fault和NMI中断)
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
__asm void INTERRUPT_DISABLE(void)
{
    CPSID   I
    BX      LR
}

/******************************************************************************
 * 描述  ：开启中断总开关
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
__asm void INTERRUPT_ENABLE(void)
{
    CPSIE   I
    BX      LR
}

/******************************************************************************
 * 描述  ：初始化任务堆栈
 * 参数  ：(in)-pTask       任务指针
 *         (in)-taskProcess 任务程序指针
 *         (in)-parg        任务程序参数列表指针
 * 返回  ：无
******************************************************************************/
void ZF_initTaskStack(Task *pTask, ITaskProcess taskProcess, void *parg)
{
    ZF_STK_TYPE *pStkPtr;
    
    ZF_ASSERT(pTask != (Task *)0)
    ZF_ASSERT(taskProcess != (ITaskProcess)0)
    
    pStkPtr = (ZF_STK_TYPE *)((ZF_STK_TYPE)pTask->pStkBase
        + (ZF_STK_TYPE)pTask->StkSize);
    
    /* 异常发生时自动保存的寄存器 */
    *--pStkPtr = (ZF_STK_TYPE)0x01000000u;          /* xPSR的bit24必须置1 */
    *--pStkPtr = (ZF_STK_TYPE)taskProcess;          /* 任务的入口地址 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R14 (LR) */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R12 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R3 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R2 */
    *--pStkPtr = (ZF_STK_TYPE)0x00000000u;          /* R1 */
    *--pStkPtr = (ZF_STK_TYPE)parg;                 /* R0 : 任务形参 */
    
    /* 异常发生时需手动保存的寄存器 */
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
 * 描述  ：开启中断，并触发PendSV异常(用于系统第一次任务调度)
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
__asm void SF_readyGo(void)
{
    /* 设置PendSV异常优先级为最低 */
    LDR     R0, = NVIC_SYSPRI14
    LDR     R1, = NVIC_PENDSV_PRI
    STRB    R1, [R0]
    NOP
    
    /* 设置psp的值为0，开始第一次上下文切换 */
    MOVS    R0, #0
    MSR     PSP, R0
    
    /* 触发PendSV异常 */
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    
    /* 开中断 */
    CPSIE   I
    
/* 一个死循环，程序正常运作不会来到这里 */
OSStartHang
    B       OSStartHang
    NOP
}

/******************************************************************************
 * 描述  ：PendSV异常处理
 * 参数  ：无
 * 返回  ：无
******************************************************************************/
__asm void PendSV_Handler(void)
{
    IMPORT  pCurrentTask
    IMPORT  pTopPriorityTask
    
    /* 任务的保存，即把CPU寄存器的值存储到任务的堆栈中 */
    /* 关中断，NMI和HardFault除外 */
    CPSID   I
    
    /* 判断是否第一次运行 */
    MRS     R0, PSP
    CBZ     R0, PendSVHandler_NotSave
    
    /**
        在进入PendSV异常的时候，当前CPU的xPSR，PC(任务入口地址)，
        R14，R12，R3，R2，R1，R0会自动存储到当前任务堆栈，同时递减PSP的值
    **/
    /* 手动存储CPU寄存器R4-R11的值到当前任务的堆栈 */
    STMDB   R0!, {R4-R11}
    
    /* R0指向pCurrentTask的堆栈指针(指向空闲位置的顶部) */
    LDR     R1, = pCurrentTask
    LDR     R1, [R1]
    STR     R0, [R1]
    NOP
    
/* 任务的切换，即把下一个要运行的任务的堆栈内容加载到CPU寄存器中 */
PendSVHandler_NotSave

    /* 等效操作pCurrentTask = pTopPriorityTask; */
    LDR     R0, = pCurrentTask
    LDR     R1, = pTopPriorityTask
    LDR     R2, [R1]
    STR     R2, [R0]
    
    /* pTopPriorityTask的信息出栈 */
    LDR     R0, [R2]
    LDMIA   R0!, {R4-R11}
    
    /* 设置PSP指向下一个要执行的任务的堆栈的栈底(已弹出了寄存器信息) */
    MSR     PSP, R0
    /* 确保异常返回使用的堆栈指针是PSP */
    ORR     LR, LR, #0x04 /* 设置LR寄存器的位2为1 */
    CPSIE   I /* 开中断 */
    
    /**
        函数返回，这个时候任务堆栈中的剩下内容将会自动加载到
        xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
        同时PSP的值也将更新，即指向任务堆栈的栈顶。
        在STM32中，堆栈是由高地址向低地址生长的
    **/
    BX      LR
    NOP
}

/******************************** END OF FILE ********************************/
