/**
  *****************************************************************************
  * @file    zf_list.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   简单列表的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_LIST_H__
#define __ZF_LIST_H__

#ifdef __cplusplus 
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

/* 列表节点结构 */
typedef struct _ListNode
{
    bool IsExternData;          /* 是否外部数据，是则销毁时不释放 */
    uint8_t *pData;             /* 指向数据的指针 */
    uint32_t Size;              /* 数据的大小 */
    struct _ListNode *Next;     /* 指向下一个节点 */
} ListNode;

/* 列表结构 */
typedef struct _List
{
    ListNode *pRootNode;        /* 指向根节点数据 */
    uint32_t Count;             /* 节点个数 */
    
    /* 在尾端增加节点 */
    bool (*Add)(struct _List * const pList, ListNode *pNode);
    
    /* 删除节点(释放空间) */
    bool (*Delete)(struct _List * const pList, ListNode *pNode);
    
    /* 移除节点(不释放空间) */
    bool (*Remove)(struct _List * const pList, ListNode *pNode);
    
    /* 返回指定索引处的节点的指针 */
    bool (*GetElementAt)(struct _List * const pList, uint32_t index,
        ListNode **ppNode);
    
    /* 返回数据区指向data的节点的指针 */
    bool (*GetElementByData)(struct _List * const pList, void *pdata,
        ListNode **ppNode);
    
    /* 返回指定索引处的节点的数据缓冲区的指针 */
    void *(*GetElementDataAt)(struct _List * const pList, uint32_t index);
    
    /* 返回节点的索引 */
    bool (*GetElementIndex)(struct _List * const pList, ListNode *pNode,
        uint32_t *pIndex);
    
    /* 在指定索引处增加节点 */
    bool (*AddElementAt)(struct _List * const pList, uint32_t index,
        ListNode *pNode);
    
    /* 在指定索引处删除节点(释放空间) */
    bool (*DeleteElementAt)(struct _List * const pList, uint32_t index);
    
    /* 在指定索引处移除节点(不释放空间) */
    bool (*RemoveElementAt)(struct _List * const pList, uint32_t index);
    
    /* 清空列表(释放空间) */
    bool (*Clear)(struct _List * const pList);
    
    /* 释放列表(释放空间) */
    bool (*Dispose)(struct _List * const pList);
} List;

/* 创建列表(内部分配空间) */
bool List_create(List **ppList);

/* 在尾端增加节点 */
bool List_add(List * const pList, ListNode *pNode);

/* 删除节点(内部释放空间) */
bool List_delete(List * const pList, ListNode *pNode);

/* 移除节点(内部不释放空间) */
bool List_remove(List * const pList, ListNode *pNode);

/* 返回指定索引处的节点的指针 */
bool List_getElementAt(List * const pList, uint32_t index,
    ListNode **ppNode);

/* 返回数据区指向data的节点的指针 */
bool List_getElementByData(List * const pList, void *pdata,
    ListNode **ppNode);

/* 返回指定索引处的节点的数据缓冲区的指针 */
void *List_getElementDataAt(List * const pList, uint32_t index);

/* 返回节点的索引 */
bool List_getElementIndex(List * const pList, ListNode *pNode,
    uint32_t *pIndex);

/* 在指定索引处增加节点 */
bool List_addElementAt(List * const pList, uint32_t index,
    ListNode *pNode);

/* 在指定索引处删除节点(内部释放空间) */
bool List_deleteElementAt(List * const pList, uint32_t index);

/* 在指定索引处移除节点(不释放空间) */
bool List_removeElementAt(List * const pList, uint32_t index);

/* 清空列表(内部释放空间) */
bool List_clear(List * const pList);

/* 释放列表(内部释放空间) */
bool List_dispose(List * const pList);

/* 创建节点(内部分配空间，size=0表示使用外部数据) */
bool List_mallocNode(ListNode **ppNode, void **ppData, uint32_t size);

/* 释放节点(不释放外部创建的数据) */
bool List_freeNode(ListNode *pNode);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_LIST_H__ */

/******************************** END OF FILE ********************************/
