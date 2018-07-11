/**
  *****************************************************************************
  * @file    zf_critical.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   Ӳ����ز�����ͷ�ļ�
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


#ifdef __cplusplus
}
#endif

#endif /* __ZF_CRITICAL_H__ */

/******************************** END OF FILE ********************************/
