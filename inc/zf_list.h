/**
  *****************************************************************************
  * @file    zf_list.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���б��ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
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

/* �б�ڵ�ṹ */
typedef struct _ListNode
{
    bool IsExternData;          /* �Ƿ��ⲿ���ݣ���������ʱ���ͷ� */
    uint8_t *pData;             /* ָ�����ݵ�ָ�� */
    uint32_t Size;              /* ���ݵĴ�С */
    struct _ListNode *Next;     /* ָ����һ���ڵ� */
} ListNode;

/* �б�ṹ */
typedef struct _List
{
    ListNode *pRootNode;        /* ָ����ڵ����� */
    uint32_t Count;             /* �ڵ���� */
    
    /* ��β�����ӽڵ� */
    bool (*Add)(struct _List * const pList, ListNode *pNode);
    
    /* ɾ���ڵ�(�ͷſռ�) */
    bool (*Delete)(struct _List * const pList, ListNode *pNode);
    
    /* �Ƴ��ڵ�(���ͷſռ�) */
    bool (*Remove)(struct _List * const pList, ListNode *pNode);
    
    /* ����ָ���������Ľڵ��ָ�� */
    bool (*GetElementAt)(struct _List * const pList, uint32_t index,
        ListNode **ppNode);
    
    /* ����������ָ��data�Ľڵ��ָ�� */
    bool (*GetElementByData)(struct _List * const pList, void *pdata,
        ListNode **ppNode);
    
    /* ����ָ���������Ľڵ�����ݻ�������ָ�� */
    void *(*GetElementDataAt)(struct _List * const pList, uint32_t index);
    
    /* ���ؽڵ������ */
    bool (*GetElementIndex)(struct _List * const pList, ListNode *pNode,
        uint32_t *pIndex);
    
    /* ��ָ�����������ӽڵ� */
    bool (*AddElementAt)(struct _List * const pList, uint32_t index,
        ListNode *pNode);
    
    /* ��ָ��������ɾ���ڵ�(�ͷſռ�) */
    bool (*DeleteElementAt)(struct _List * const pList, uint32_t index);
    
    /* ��ָ���������Ƴ��ڵ�(���ͷſռ�) */
    bool (*RemoveElementAt)(struct _List * const pList, uint32_t index);
    
    /* ����б�(�ͷſռ�) */
    bool (*Clear)(struct _List * const pList);
    
    /* �ͷ��б�(�ͷſռ�) */
    bool (*Dispose)(struct _List * const pList);
} List;

/* �����б�(�ڲ�����ռ�) */
bool List_create(List **ppList);

/* ��β�����ӽڵ� */
bool List_add(List * const pList, ListNode *pNode);

/* ɾ���ڵ�(�ڲ��ͷſռ�) */
bool List_delete(List * const pList, ListNode *pNode);

/* �Ƴ��ڵ�(�ڲ����ͷſռ�) */
bool List_remove(List * const pList, ListNode *pNode);

/* ����ָ���������Ľڵ��ָ�� */
bool List_getElementAt(List * const pList, uint32_t index,
    ListNode **ppNode);

/* ����������ָ��data�Ľڵ��ָ�� */
bool List_getElementByData(List * const pList, void *pdata,
    ListNode **ppNode);

/* ����ָ���������Ľڵ�����ݻ�������ָ�� */
void *List_getElementDataAt(List * const pList, uint32_t index);

/* ���ؽڵ������ */
bool List_getElementIndex(List * const pList, ListNode *pNode,
    uint32_t *pIndex);

/* ��ָ�����������ӽڵ� */
bool List_addElementAt(List * const pList, uint32_t index,
    ListNode *pNode);

/* ��ָ��������ɾ���ڵ�(�ڲ��ͷſռ�) */
bool List_deleteElementAt(List * const pList, uint32_t index);

/* ��ָ���������Ƴ��ڵ�(���ͷſռ�) */
bool List_removeElementAt(List * const pList, uint32_t index);

/* ����б�(�ڲ��ͷſռ�) */
bool List_clear(List * const pList);

/* �ͷ��б�(�ڲ��ͷſռ�) */
bool List_dispose(List * const pList);

/* �����ڵ�(�ڲ�����ռ䣬size=0��ʾʹ���ⲿ����) */
bool List_mallocNode(ListNode **ppNode, void **ppData, uint32_t size);

/* �ͷŽڵ�(���ͷ��ⲿ����������) */
bool List_freeNode(ListNode *pNode);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_LIST_H__ */

/******************************** END OF FILE ********************************/
