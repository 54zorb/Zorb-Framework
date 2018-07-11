/**
  *****************************************************************************
  * @file    zf_debug.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���������ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_DEBUG_H__
#define __ZF_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdbool.h"

#define LOG_D 0; /* ��Ϣ�ȼ������� */
#define LOG_W 1; /* ��Ϣ�ȼ����澯 */
#define LOG_E 2; /* ��Ϣ�ȼ������� */

#define _ZF_DEBUG             /* ������Թ��� */
#define ZF_DEBUG_ON true      /* ���õ��Թ��� */

#ifdef _ZF_DEBUG
    #if ZF_DEBUG_ON
        #define ZF_DEBUG(rank, x...) do     \
        {                                   \
            char code[10] = "[rank=0]";     \
            code[6] = '0' + (char)rank;     \
            if (code[6] != '0')             \
            {                               \
                printf("%s", code);         \
            }                               \
            printf(x);                      \
        } while(0)
    #else
        #define ZF_DEBUG(rank, x...)
    #endif /* ZF_DEBUG_ON */
#endif /* _ZF_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __ZF_DEBUG_H__ */

/******************************** END OF FILE ********************************/
