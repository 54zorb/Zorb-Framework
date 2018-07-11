/**
  *****************************************************************************
  * @file    zf_buffer.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   环形缓冲器的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_buffer.h"
#include "zf_assert.h"
#include "zf_malloc.h"
#include "zf_debug.h"

/******************************************************************************
 * 描述  ：创建环形缓冲器(内部分配空间，size=0表示使用外部数据)
 * 参数  ：(out)-ppRb  环形缓冲器结构体指针的指针
 *         (in)-size   缓冲器大小
 *         (out)-ppBuf 缓冲器空间指针的指针
 * 返回  ：无
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
    
    /* 初始化方法 */
    pRb->IsFull = RB_isFull;
    pRb->IsEmpty = RB_isEmpty;
    pRb->SaveByte = RB_saveByte;
    pRb->GetByte = RB_getByte;
    pRb->GetCount = RB_getCount;
    pRb->ReadBytes = RB_readBytes;
    pRb->DropBytes = RB_dropBytes;
    pRb->Clear = RB_clear;
    pRb->Dispose = RB_dispose;
    
    /* 输出 */
    *ppRb = pRb;
    
    return true;
}

/******************************************************************************
 * 描述  ：缓冲器是否已满
 * 参数  ：(in)-pRb 环形缓冲器结构体指针
 * 返回  ：-true    已满
 *         -false   未满
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
 * 描述  ：缓冲器是否空
 * 参数  ：(in)-pRb 环形缓冲器结构体指针
 * 返回  ：-true    空
 *         -false   未空
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
 * 描述  ：压入一个字节
 * 参数  ：(in)-pRb  环形缓冲器结构体指针
 *         (in)-byte 要压入的字节
 * 返回  ：-true     成功
 *         -false    失败
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
 * 描述  ：取出一个字节
 * 参数  ：(in)-pRb    环形缓冲器结构体指针
 *         (out)-pByte 存放取出字节的地址
 * 返回  ：-true       成功
 *         -false      失败
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
 * 描述  ：读取缓冲器已使用字节个数
 * 参数  ：(in)-pRb  环形缓冲器结构体指针
 * 返回  ：已使用字节个数
******************************************************************************/
uint32_t RB_getCount(RingBuffer * const pRb)
{
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    return pRb->Count;
}

/******************************************************************************
 * 描述  ：读取n个字节(n超过最大数据数时全部读出)
 * 参数  ：(in)-pRb     环形缓冲器结构体指针
 *         (out)-pArray 存放取出字节的地址
 *         (in)-n       要读取的字节个数
 * 返回  ：-true        成功
 *         -false       失败
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
 * 描述  ：丢弃n个字节(n超过最大数据数时全部丢弃)
 * 参数  ：(in)-pRb     环形缓冲器结构体指针
 *         (in)-n       要丢弃的字节个数
 * 返回  ：-true        成功
 *         -false       失败
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
 * 描述  ：清空缓冲器
 * 参数  ：(in)-pRb     环形缓冲器结构体指针
 * 返回  ：-true        成功
 *         -false       失败
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
 * 描述  ：释放缓冲器(不释放外部创建的缓冲区)
 * 参数  ：(in)-pRb     环形缓冲器结构体指针
 * 返回  ：-true     成功
 *         -false    失败
******************************************************************************/
bool RB_dispose(RingBuffer * const pRb)
{
    ZF_ASSERT(pRb != (RingBuffer *)0)
    
    /* 外部创建的缓冲区不释放 */
    if (!pRb->IsExternBuffer)
    {
        ZF_FREE(pRb->pBuf);
    }
    
    ZF_FREE(pRb);
    
    return true;
}

/******************************** END OF FILE ********************************/
