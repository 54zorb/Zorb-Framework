/**
  *****************************************************************************
  * @file    zf_buffer.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���λ�������ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
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

/* ���λ��������ݽṹ */
typedef struct _RingBuffer
{
    bool IsExternBuffer;  /* �Ƿ��ⲿ����������������ʱ���ͷ� */
    uint8_t *pBuf;        /* ������ָ�� */
    uint32_t Head;        /* ������ͷ��ַ */
    uint32_t Trail;       /* ������β��ַ */
    uint32_t Size;        /* ��������С */
    uint32_t Count;       /* �����ֽ��� */
    
    /* �������Ƿ����� */
    bool (*IsFull)(struct _RingBuffer * const pRb);
    
    /* �������Ƿ�� */
    bool (*IsEmpty)(struct _RingBuffer * const pRb);
    
    /* ѹ��һ���ֽ� */
    bool (*SaveByte)(struct _RingBuffer * const pRb, uint8_t byte);
    
    /* ѹ��n���ֽڵ����� */
    uint32_t (*SaveRange)(struct _RingBuffer * const pRb, uint8_t *pArray,
        uint32_t n);
    
    /* ȡ��һ���ֽ� */
    bool (*GetByte)(struct _RingBuffer * const pRb, uint8_t *pByte);
    
    /* ��ȡ��������ʹ���ֽڸ��� */
    uint32_t (*GetCount)(struct _RingBuffer * const pRb);
    
    /* ��ȡn���ֽ�(n�������������ʱȫ������) */
    uint32_t (*ReadBytes)(struct _RingBuffer * const pRb, uint8_t *pArray,
        uint32_t n);
    
    /* ����n���ֽ�(n�������������ʱȫ������) */
    uint32_t (*DropBytes)(struct _RingBuffer * const pRb, uint32_t n);
    
    /* ��ջ����� */
    bool (*Clear)(struct _RingBuffer * const pRb);
    
    /* �ͷŻ�����(���ͷ��ⲿ�����Ļ�����) */
    bool (*Dispose)(struct _RingBuffer * const pRb);
} RingBuffer;

/* ����������(�ڲ�����ռ䣬size=0��ʾʹ���ⲿ����) */
bool RB_create(RingBuffer **ppRb, uint32_t size);

/* �������Ƿ����� */
bool RB_isFull(RingBuffer * const pRb);

/* �������Ƿ�� */
bool RB_isEmpty(RingBuffer * const pRb);

/* ѹ��һ���ֽ� */
bool RB_saveByte(RingBuffer * const pRb, uint8_t byte);

/* ѹ��n���ֽڵ����� */
uint32_t RB_saveRange(RingBuffer * const pRb, uint8_t *pArray, uint32_t n);

/* ȡ��һ���ֽ� */
bool RB_getByte(RingBuffer * const pRb, uint8_t *pByte);

/* ��ȡ��������ʹ���ֽڸ��� */
uint32_t RB_getCount(RingBuffer * const pRb);

/* ��ȡn���ֽ�(n�������������ʱȫ������) */
uint32_t RB_readBytes(RingBuffer * const pRb, uint8_t *pArray, uint32_t n);

/* ����n���ֽ�(n�������������ʱȫ������) */
uint32_t RB_dropBytes(RingBuffer * const pRb, uint32_t n);

/* ��ջ����� */
bool RB_clear(RingBuffer * const pRb);

/* �ͷŻ�����(���ͷ��ⲿ�����Ļ�����) */
bool RB_dispose(RingBuffer * const pRb);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_BUFFER_H__ */

/******************************** END OF FILE ********************************/
