/**
  *****************************************************************************
  * @file    zf_assert.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   断言的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_ASSERT_H__
#define __ZF_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define _ZF_ASSERT              /* 定义断言功能 */
#define ZF_ASSERT_ON true       /* 启用断言功能 */

#ifdef _ZF_ASSERT
    #if ZF_ASSERT_ON
         #define ZF_ASSERT(expression_) ((expression_) ?\
            (void)0 : ZF_assertHandle((uint8_t *)__FILE__, (int)__LINE__));
    #else
         #define ZF_ASSERT(expression_)
    #endif /* ZF_ASSERT_ON */
#endif /* _ZF_ASSERT */

/* 断言产生时的处理 */
void ZF_assertHandle(uint8_t *pFileName, int line);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_ASSERT_H__ */

/******************************** END OF FILE ********************************/
