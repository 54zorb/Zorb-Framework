/**
  *****************************************************************************
  * @file    zf_list.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   简单列表的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_list.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"

/******************************************************************************
 * 描述  ：创建列表(内部分配空间)
 * 参数  ：(in)-ppList 列表指针的指针
 * 返回  ：无
******************************************************************************/
bool List_create(List **ppList)
{
    List *pList;
    
    ZF_ASSERT(ppList != (List **)0)
    
    /* 分配空间 */
    pList = ZF_MALLOC(sizeof(List));
    if (pList == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc list space error\r\n");
        return false;
    }
    
    /* 初始化成员 */
    pList->pRootNode = NULL;
    pList->Count = 0;
    
    /* 初始化方法 */
    pList->Add = List_add;
    pList->Delete = List_delete;
    pList->Remove = List_remove;
    pList->GetElementAt = List_getElementAt;
    pList->GetElementByData = List_getElementByData;
    pList->GetElementDataAt = List_getElementDataAt;
    pList->GetElementIndex = List_getElementIndex;
    pList->AddElementAt = List_addElementAt;
    pList->DeleteElementAt = List_deleteElementAt;
    pList->RemoveElementAt = List_removeElementAt;
    pList->Clear = List_clear;
    
    /* 输出 */
    *ppList = pList;
    
    return true;
}

/******************************************************************************
 * 描述  ：在尾端增加节点
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-pNode  要增加的节点
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_add(List * const pList, ListNode *pNode)
{
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    return List_addElementAt(pList, pList->Count, pNode);
}

/******************************************************************************
 * 描述  ：删除节点(内部释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-pNode  要删除的节点
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_delete(List * const pList, ListNode *pNode)
{
    /* 返回结果 */
    bool res = false;
    
    /* 要删除节点的前一个节点 */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* 要删除的是根节点 */
    if (pNode == pList->pRootNode)
    {
        pList->pRootNode = pNode->Next;
        pList->Count--;
        
        /* 释放空间 */
        List_freeNode(pNode);
        
        res = true;
    }
    /* 要删除的不是根节点 */
    else
    {
        pPreviousNode = pList->pRootNode;
        while (pPreviousNode->Next != pNode)
        {
            pPreviousNode = pPreviousNode->Next;
        }
        
        /* 找到要删除的节点 */
        if(pPreviousNode != NULL || pPreviousNode->Next == pNode)
        {
            pPreviousNode->Next = pNode->Next;
            pList->Count--;
            
            /* 释放空间 */
            List_freeNode(pNode);
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * 描述  ：移除节点(内部不释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-pNode  要删除的节点
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_remove(List * const pList, ListNode *pNode)
{
    /* 返回结果 */
    bool res = false;
    
    /* 要移除节点的前一个节点 */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* 要移除的是根节点 */
    if (pNode == pList->pRootNode)
    {
        pList->pRootNode = pNode->Next;
        pList->Count--;
        
        res = true;
    }
    /* 要移除的不是根节点 */
    else
    {
        pPreviousNode = pList->pRootNode;
        while (pPreviousNode->Next != pNode)
        {
            pPreviousNode = pPreviousNode->Next;
        }
        
        /* 找到要移除的节点 */
        if(pPreviousNode != NULL || pPreviousNode->Next == pNode)
        {
            pPreviousNode->Next = pNode->Next;
            pList->Count--;
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * 描述  ：返回指定索引处的节点的指针
 * 参数  ：(in)-pList   列表结构体指针
 *         (in)-index   索引
 *         (out)-ppNode 指定索引处的节点指针的指针
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool List_getElementAt(List * const pList, uint32_t index,
    ListNode **ppNode)
{
    /* 返回结果 */
    bool res = true;
    
    ListNode *pNode;
    uint32_t i;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(index < pList->Count)
    
    pNode = pList->pRootNode;
    for (i = 0; i < index; i++)
    {
        if (pNode->Next == NULL)
        {
            res = false;
            break;
        }
        pNode = pNode->Next;
    }
    
    if(res == true)
    {
        *ppNode = pNode;
    }
    else
    {
        *ppNode = NULL;
    }
    
    return res;
}

/******************************************************************************
 * 描述  ：返回数据区指向data的节点的指针
 * 参数  ：(in)-pList   列表结构体指针
 *         (in)-pdata   数据缓冲区指针
 *         (out)-ppNode 节点指针的地址
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool List_getElementByData(List * const pList, void *pdata, ListNode **ppNode)
{
    ListNode *pNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pdata != (void *)0)
    
    pNode = pList->pRootNode;
    while (pNode != NULL)
    {
        if (pNode->pData == pdata)
        {
            /* 输出 */
            *ppNode = pNode;
            
            return true;
        }
        
        pNode = pNode->Next;
    }
    
    /* 输出 */
    *ppNode = NULL;
    
    return false;
}

/******************************************************************************
 * 描述  ：返回指定索引处的节点的数据缓冲区的指针
 * 参数  ：(in)-pList   列表结构体指针
 *         (in)-index   索引
 * 返回  ：-成功        数据缓冲区的指针
 *         -失败        NULL
******************************************************************************/
void *List_getElementDataAt(List * const pList, uint32_t index)
{
    ListNode *pNode;
    
    List_getElementAt(pList, index, &pNode);
    
    if (pNode != NULL)
    {
        return (void*)pNode->pData;
    }
    
    return NULL;
}

/******************************************************************************
 * 描述  ：返回节点的索引
 * 参数  ：(in)-pList   列表结构体指针
 *         (in)-pNode   节点
 *         (out)-pIndex 节点的索引的指针
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool List_getElementIndex(List * const pList, ListNode *pNode,
    uint32_t *pIndex)
{
    /* 返回结果 */
    bool res = true;
    
    uint32_t index = 0;
    
    ListNode *pFindNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    pFindNode = pList->pRootNode;
    while (pFindNode != pNode && pFindNode != NULL)
    {
        pFindNode = pFindNode->Next;
        index++;
    }
    
    /* 没找到节点 */
    if (pFindNode != pNode)
    {
        res = false;
        index = 0;
    }
    
    /* 输出索引 */
    *pIndex = index;
    
    return res;
}

/******************************************************************************
 * 描述  ：在指定索引处增加节点(索引超过最大值，则在尾端增加)
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-index  索引
 *         (in)-pNode  要增加的节点
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_addElementAt(List * const pList, uint32_t index,
    ListNode *pNode)
{
    /* 返回结果 */
    bool res = false;
    
    /* 要增加位置的前一个节点 */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    if (index > pList->Count)
    {
        index = pList->Count;
    }
    
    /* 根节点 */
    if (index == 0)
    {
        /* 插入节点 */
        pNode->Next = pList->pRootNode;
        pList->pRootNode = pNode;
        
        /* 数量加1 */
        pList->Count++;
        
        res = true;
    }
    else
    {
        /* 找要增加位置的前一个节点 */
        if (List_getElementAt(pList, index - 1, &pPreviousNode))
        {
            /* 插入节点 */
            pNode->Next = pPreviousNode->Next;
            pPreviousNode->Next = pNode;
            
            /* 数量加1 */
            pList->Count++;
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * 描述  ：在指定索引处删除节点(内部释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-index  索引
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_deleteElementAt(List * const pList, uint32_t index)
{
    /* 要删除的节点 */
    ListNode *pDeleteNode = NULL;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(index < pList->Count)
    
    if (!List_getElementAt(pList, index, &pDeleteNode))
    {
        return false;
    }
    
    return List_delete(pList, pDeleteNode);
}

/******************************************************************************
 * 描述  ：在指定索引处移除节点(内部不释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 *         (in)-index  索引
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_removeElementAt(List * const pList, uint32_t index)
{
    /* 要移除的节点 */
    ListNode *pDeleteNode = NULL;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(index < pList->Count)
    
    if (!List_getElementAt(pList, index, &pDeleteNode))
    {
        return false;
    }
    
    return List_remove(pList, pDeleteNode);
}

/******************************************************************************
 * 描述  ：清空列表(内部释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_clear(List * const pList)
{
    /* 返回结果 */
    bool res = true;
    
    ZF_ASSERT(pList != (List *)0)
    
    while (pList->Count)
    {
        res &= List_deleteElementAt(pList, 0);
    }
    
    return res;
}

/******************************************************************************
 * 描述  ：释放列表(内部释放空间)
 * 参数  ：(in)-pList  列表结构体指针
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_dispose(List * const pList)
{
    /* 返回结果 */
    bool res = true;
    
    ZF_ASSERT(pList != (List *)0)
    
    res = List_clear(pList);
    
    ZF_FREE(pList);
    
    return res;
}

/******************************************************************************
 * 描述  ：创建节点(内部分配空间，size=0表示使用外部数据)
 * 参数  ：(out)-ppNode  节点指针的指针
 *         (out)-ppData  存放的数据结构指针的指针
 *         (in)-size    存放的数据结构大小(输入0则表示不内部分配数据空间)
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool List_mallocNode(ListNode **ppNode, void **ppData,
    uint32_t size)
{
    ListNode *pNode;
    void *pData;
    
    ZF_ASSERT(ppNode != (ListNode **)0)
    
    pNode = (ListNode *)ZF_MALLOC(sizeof(ListNode));
    if (pNode == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc list node space error\r\n");
        return false;
    }
    
    pNode->Next = NULL;
    
    if (size > 0)
    {
        ZF_ASSERT(ppData != (void **)0)
        
        pData = (void *)ZF_MALLOC(size);
        if (pData == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc list node data space error\r\n");
            return false;
        }
        
        pNode->pData = pData;
        pNode->Size = size;
        pNode->IsExternData = false;
        
        /* 输出 */
        *ppData = pData;
    }
    else
    {
        pNode->pData = NULL;
        pNode->Size = 0;
        pNode->IsExternData = true;
    }
    
    /* 输出 */
    *ppNode = pNode;
    
    return true;
}

/******************************************************************************
 * 描述  ：释放节点(不释放外部创建的数据)
 * 参数  ：(in)-pNode  要释放的节点
 * 返回  ：-true       成功
 *         -false      失败
******************************************************************************/
bool List_freeNode(ListNode *pNode)
{
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* 外部创建的数据不释放 */
    if (!pNode->IsExternData)
    {
        ZF_FREE(pNode->pData);
    }
    
    ZF_FREE(pNode);
    
    return true;
}

/******************************** END OF FILE ********************************/
