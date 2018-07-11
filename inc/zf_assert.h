/**
  *****************************************************************************
  * @file    zf_assert.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���Ե�ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_ASSERT_H__
#define __ZF_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define _ZF_ASSERT              /* ������Թ��� */
#define ZF_ASSERT_ON true       /* ���ö��Թ��� */

#ifdef _ZF_ASSERT
    #if ZF_ASSERT_ON
         #define ZF_ASSERT(expression_) ((expression_) ?\
            (void)0 : ZF_assertHandle((uint8_t *)__FILE__, (int)__LINE__));
    #else
         #define ZF_ASSERT(expression_)
    #endif /* ZF_ASSERT_ON */
#endif /* _ZF_ASSERT */

/* ���Բ���ʱ�Ĵ��� */
void ZF_assertHandle(uint8_t *pFileName, int line);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_ASSERT_H__ */

/******************************** END OF FILE ********************************/
