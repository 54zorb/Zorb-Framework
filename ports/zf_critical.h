/**
  *****************************************************************************
  * @file    zf_critical.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   硬件相关操作的头文件
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


#ifdef __cplusplus
}
#endif

#endif /* __ZF_CRITICAL_H__ */

/******************************** END OF FILE ********************************/
