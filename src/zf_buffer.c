/**
  *****************************************************************************
  * @file    zf_buffer.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���λ�������ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_buffer.h"
#include "zf_assert.h"
#include "zf_malloc.h"
#include "zf_debug.h"

/******************************************************************************
 * ����  ���������λ�����(�ڲ�����ռ䣬size=0��ʾʹ���ⲿ����)
 * ����  ��(out)-ppRb  ���λ������ṹ��ָ���ָ��
 *         (in)-size   ��������С
 *         (out)-ppBuf �������ռ�ָ���ָ��
 * ����  ����
******************************************************************************/
bool RB_create(RingBuffer **ppRb, uint32_t size)
{
    RingBuffer *pRb;
    uint8_t *pBuf;
    
    ZF_ASSERT(ppRb != (RingBuffer **)0)
    
    pRb = (RingBuffer *)ZF_MALLOC(sizeof(RingBuffer));
    if (pRb == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc ringbuffer space error\r\n");
        return false;
    }
    
    pRb->Head = (uint32_t)0;
    pRb->Trail = (uint32_t)0;
    pRb->Count = 0;
    
    if (size > 0)
    {
        pBuf = (void *)ZF_MALLOC(size);
        if (pBuf == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc ringbuffer buffer space error\r\n");
            return false;
        }
        
        pRb->pBuf = pBuf;
        pRb->Size = size;
        pRb->IsExternBuffer = false;
    }
    else
    {
        pRb->pBuf = NULL;
        pRb->Size = 0;
        pRb->IsExternBuffer = true;
    }
    
    /* ��ʼ������ */
    pRb->IsFull = RB_isFull;
    pRb->IsEmpty = RB_isEmpty;
    pRb->SaveByte = RB_saveByte;
    pRb->GetByte = RB_getByte;
    pRb->GetCount = RB_getCount;
    pRb->ReadBytes = RB_readBytes;
    pRb->DropBytes = RB_dropBytes;
    pRb->Clear = RB_clear;
    pRb->Dispose = RB_dispose;
    
    /* ��� */
    *ppRb = pRb;
    
    return true;
}

/******************************************************************************
 * ����  ���������Ƿ�����
 * ����  ��(in)-pRb ���λ������ṹ��ָ��
 * ����  ��-true    ����
 *         -false   δ��
******************************************************************************/
bool RB_isFull(RingBuffer * const pRb)
{
    bool res = false;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    if (pRb->Size == pRb->Count)
    {
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ���������Ƿ��
 * ����  ��(in)-pRb ���λ������ṹ��ָ��
 * ����  ��-true    ��
 *         -false   δ��
******************************************************************************/
bool RB_isEmpty(RingBuffer * const pRb)
{
    bool res = false;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    if (pRb->Count == 0)
    {
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ��ѹ��һ���ֽ�
 * ����  ��(in)-pRb  ���λ������ṹ��ָ��
 *         (in)-byte Ҫѹ����ֽ�
 * ����  ��-true     �ɹ�
 *         -false    ʧ��
******************************************************************************/
bool RB_saveByte(RingBuffer * const pRb, uint8_t byte)
{
    bool res = false;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    ZF_ASSERT(pRb->pBuf != (uint8_t *)0)
    
    if (!RB_isFull(pRb))
    {
        pRb->pBuf[pRb->Trail++] = byte;
        pRb->Trail %= pRb->Size;
        pRb->Count++;
        
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ��ȡ��һ���ֽ�
 * ����  ��(in)-pRb    ���λ������ṹ��ָ��
 *         (out)-pByte ���ȡ���ֽڵĵ�ַ
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool RB_getByte(RingBuffer * const pRb, uint8_t *pByte)
{
    bool res = false;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    ZF_ASSERT(pRb->pBuf != (uint8_t *)0)
    ZF_ASSERT(pByte != (uint8_t *)0)
    
    if (!RB_isEmpty(pRb))
    {
        *pByte = pRb->pBuf[pRb->Head++];
        pRb->Head %= pRb->Size;
        pRb->Count--;
        
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ����ȡ��������ʹ���ֽڸ���
 * ����  ��(in)-pRb  ���λ������ṹ��ָ��
 * ����  ����ʹ���ֽڸ���
******************************************************************************/
uint32_t RB_getCount(RingBuffer * const pRb)
{
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    return pRb->Count;
}

/******************************************************************************
 * ����  ����ȡn���ֽ�(n�������������ʱȫ������)
 * ����  ��(in)-pRb     ���λ������ṹ��ָ��
 *         (out)-pArray ���ȡ���ֽڵĵ�ַ
 *         (in)-n       Ҫ��ȡ���ֽڸ���
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool RB_readBytes(RingBuffer * const pRb, uint8_t *pArray, uint32_t n)
{
    bool res = false;
    uint32_t len;
    uint32_t i, index;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    ZF_ASSERT(pRb->pBuf != (uint8_t *)0)
    ZF_ASSERT(pArray != (uint8_t *)0)
    
    if (!RB_isEmpty(pRb))
    {
        len = RB_getCount(pRb);
        if (len > n)
        {
            len = n;
        }
        
        for(i = 0; i < len; i++) 
        {
            index = (pRb->Head + i) % pRb->Size;
            *(pArray + i) = pRb->pBuf[index];
        }
        
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ������n���ֽ�(n�������������ʱȫ������)
 * ����  ��(in)-pRb     ���λ������ṹ��ָ��
 *         (in)-n       Ҫ�������ֽڸ���
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool RB_dropBytes(RingBuffer * const pRb, uint32_t n)
{
    bool res = false;
    uint32_t len;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    ZF_ASSERT(pRb->pBuf != (uint8_t *)0)
    
    if (!RB_isEmpty(pRb))
    {
        len = RB_getCount(pRb);
        if (len > n)
        {
            len = n;
        }
        
        pRb->Head += len;
        pRb->Count -= len;
        
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  ����ջ�����
 * ����  ��(in)-pRb     ���λ������ṹ��ָ��
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool RB_clear(RingBuffer * const pRb)
{
    bool res = false;
    
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    pRb->Head = (uint32_t)0;
    pRb->Trail = (uint32_t)0;
    pRb->Count = 0;
    
    res = true;
    
    return res;
}

/******************************************************************************
 * ����  ���ͷŻ�����(���ͷ��ⲿ�����Ļ�����)
 * ����  ��(in)-pRb     ���λ������ṹ��ָ��
 * ����  ��-true     �ɹ�
 *         -false    ʧ��
******************************************************************************/
bool RB_dispose(RingBuffer * const pRb)
{
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    /* �ⲿ�����Ļ��������ͷ� */
    if (!pRb->IsExternBuffer)
    {
        ZF_FREE(pRb->pBuf);
    }
    
    ZF_FREE(pRb);
    
    return true;
}

/******************************** END OF FILE ********************************/
