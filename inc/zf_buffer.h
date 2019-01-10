/**
  *****************************************************************************
  * @file    zf_buffer.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   环形缓冲器的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_BUFFER_H__
#define __ZF_BUFFER_H__

#ifdef __cplusplus 
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

/* 环形缓冲区数据结构 */
typedef struct _RingBuffer
{
    bool IsExternBuffer;  /* 是否外部缓冲区，是则销毁时不释放 */
    uint8_t *pBuf;        /* 缓冲区指针 */
    uint32_t Head;        /* 缓冲区头地址 */
    uint32_t Trail;       /* 缓冲区尾地址 */
    uint32_t Size;        /* 缓冲区大小 */
    uint32_t Count;       /* 数据字节数 */
    
    /* 缓冲器是否已满 */
    bool (*IsFull)(struct _RingBuffer * const pRb);
    
    /* 缓冲器是否空 */
    bool (*IsEmpty)(struct _RingBuffer * const pRb);
    
    /* 压入一个字节 */
    bool (*SaveByte)(struct _RingBuffer * const pRb, uint8_t byte);
    
    /* 压入n个字节的数据 */
    uint32_t (*SaveRange)(struct _RingBuffer * const pRb, uint8_t *pArray,
        uint32_t n);
    
    /* 取出一个字节 */
    bool (*GetByte)(struct _RingBuffer * const pRb, uint8_t *pByte);
    
    /* 读取缓冲器已使用字节个数 */
    uint32_t (*GetCount)(struct _RingBuffer * const pRb);
    
    /* 读取n个字节(n超过最大数据数时全部读出) */
    uint32_t (*ReadBytes)(struct _RingBuffer * const pRb, uint8_t *pArray,
        uint32_t n);
    
    /* 丢弃n个字节(n超过最大数据数时全部丢弃) */
    uint32_t (*DropBytes)(struct _RingBuffer * const pRb, uint32_t n);
    
    /* 清空缓冲器 */
    bool (*Clear)(struct _RingBuffer * const pRb);
    
    /* 释放缓冲器(不释放外部创建的缓冲区) */
    bool (*Dispose)(struct _RingBuffer * const pRb);
} RingBuffer;

/* 创建缓冲器(内部分配空间，size=0表示使用外部数据) */
bool RB_create(RingBuffer **ppRb, uint32_t size);

/* 缓冲器是否已满 */
bool RB_isFull(RingBuffer * const pRb);

/* 缓冲器是否空 */
bool RB_isEmpty(RingBuffer * const pRb);

/* 压入一个字节 */
bool RB_saveByte(RingBuffer * const pRb, uint8_t byte);

/* 压入n个字节的数据 */
uint32_t RB_saveRange(RingBuffer * const pRb, uint8_t *pArray, uint32_t n);

/* 取出一个字节 */
bool RB_getByte(RingBuffer * const pRb, uint8_t *pByte);

/* 读取缓冲器已使用字节个数 */
uint32_t RB_getCount(RingBuffer * const pRb);

/* 读取n个字节(n超过最大数据数时全部读出) */
uint32_t RB_readBytes(RingBuffer * const pRb, uint8_t *pArray, uint32_t n);

/* 丢弃n个字节(n超过最大数据数时全部丢弃) */
uint32_t RB_dropBytes(RingBuffer * const pRb, uint32_t n);

/* 清空缓冲器 */
bool RB_clear(RingBuffer * const pRb);

/* 释放缓冲器(不释放外部创建的缓冲区) */
bool RB_dispose(RingBuffer * const pRb);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_BUFFER_H__ */

/******************************** END OF FILE ********************************/
