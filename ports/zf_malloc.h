/**
  *****************************************************************************
  * @file    zf_malloc.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   内存管理的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_MALLOC_H__
#define __ZF_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "string.h"

#define _ZF_MALLOC             /* 定义内存管理功能 */

#ifdef _ZF_MALLOC
    /* 分配函数 */
    #define ZF_MALLOC(size_) malloc(size_)
    /* 释放函数 */
    #define ZF_FREE(ptr_) free(ptr_)
    /* 拷贝函数 */
    #define ZF_MEMCPY(des_, src_, len_) memcpy(des_, src_, len_)
#endif /* _ZF_MALLOC */

#ifdef __cplusplus
}
#endif

#endif /* __ZF_MALLOC_H__ */

/******************************** END OF FILE ********************************/
