/**
  *****************************************************************************
  * @file    zf_list.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ���б��ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_list.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"

/******************************************************************************
 * ����  �������б�(�ڲ�����ռ�)
 * ����  ��(in)-ppList �б�ָ���ָ��
 * ����  ����
******************************************************************************/
bool List_create(List **ppList)
{
    List *pList;
    
    ZF_ASSERT(ppList != (List **)0)
    
    /* ����ռ� */
    pList = ZF_MALLOC(sizeof(List));
    if (pList == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc list space error\r\n");
        return false;
    }
    
    /* ��ʼ����Ա */
    pList->pRootNode = NULL;
    pList->Count = 0;
    
    /* ��ʼ������ */
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
    
    /* ��� */
    *ppList = pList;
    
    return true;
}

/******************************************************************************
 * ����  ����β�����ӽڵ�
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-pNode  Ҫ���ӵĽڵ�
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_add(List * const pList, ListNode *pNode)
{
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    return List_addElementAt(pList, pList->Count, pNode);
}

/******************************************************************************
 * ����  ��ɾ���ڵ�(�ڲ��ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-pNode  Ҫɾ���Ľڵ�
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_delete(List * const pList, ListNode *pNode)
{
    /* ���ؽ�� */
    bool res = false;
    
    /* Ҫɾ���ڵ��ǰһ���ڵ� */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* Ҫɾ�����Ǹ��ڵ� */
    if (pNode == pList->pRootNode)
    {
        pList->pRootNode = pNode->Next;
        pList->Count--;
        
        /* �ͷſռ� */
        List_freeNode(pNode);
        
        res = true;
    }
    /* Ҫɾ���Ĳ��Ǹ��ڵ� */
    else
    {
        pPreviousNode = pList->pRootNode;
        while (pPreviousNode->Next != pNode)
        {
            pPreviousNode = pPreviousNode->Next;
        }
        
        /* �ҵ�Ҫɾ���Ľڵ� */
        if(pPreviousNode != NULL || pPreviousNode->Next == pNode)
        {
            pPreviousNode->Next = pNode->Next;
            pList->Count--;
            
            /* �ͷſռ� */
            List_freeNode(pNode);
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * ����  ���Ƴ��ڵ�(�ڲ����ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-pNode  Ҫɾ���Ľڵ�
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_remove(List * const pList, ListNode *pNode)
{
    /* ���ؽ�� */
    bool res = false;
    
    /* Ҫ�Ƴ��ڵ��ǰһ���ڵ� */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* Ҫ�Ƴ����Ǹ��ڵ� */
    if (pNode == pList->pRootNode)
    {
        pList->pRootNode = pNode->Next;
        pList->Count--;
        
        res = true;
    }
    /* Ҫ�Ƴ��Ĳ��Ǹ��ڵ� */
    else
    {
        pPreviousNode = pList->pRootNode;
        while (pPreviousNode->Next != pNode)
        {
            pPreviousNode = pPreviousNode->Next;
        }
        
        /* �ҵ�Ҫ�Ƴ��Ľڵ� */
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
 * ����  ������ָ���������Ľڵ��ָ��
 * ����  ��(in)-pList   �б�ṹ��ָ��
 *         (in)-index   ����
 *         (out)-ppNode ָ���������Ľڵ�ָ���ָ��
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool List_getElementAt(List * const pList, uint32_t index,
    ListNode **ppNode)
{
    /* ���ؽ�� */
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
 * ����  ������������ָ��data�Ľڵ��ָ��
 * ����  ��(in)-pList   �б�ṹ��ָ��
 *         (in)-pdata   ���ݻ�����ָ��
 *         (out)-ppNode �ڵ�ָ��ĵ�ַ
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
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
            /* ��� */
            *ppNode = pNode;
            
            return true;
        }
        
        pNode = pNode->Next;
    }
    
    /* ��� */
    *ppNode = NULL;
    
    return false;
}

/******************************************************************************
 * ����  ������ָ���������Ľڵ�����ݻ�������ָ��
 * ����  ��(in)-pList   �б�ṹ��ָ��
 *         (in)-index   ����
 * ����  ��-�ɹ�        ���ݻ�������ָ��
 *         -ʧ��        NULL
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
 * ����  �����ؽڵ������
 * ����  ��(in)-pList   �б�ṹ��ָ��
 *         (in)-pNode   �ڵ�
 *         (out)-pIndex �ڵ��������ָ��
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool List_getElementIndex(List * const pList, ListNode *pNode,
    uint32_t *pIndex)
{
    /* ���ؽ�� */
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
    
    /* û�ҵ��ڵ� */
    if (pFindNode != pNode)
    {
        res = false;
        index = 0;
    }
    
    /* ������� */
    *pIndex = index;
    
    return res;
}

/******************************************************************************
 * ����  ����ָ�����������ӽڵ�(�����������ֵ������β������)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-index  ����
 *         (in)-pNode  Ҫ���ӵĽڵ�
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_addElementAt(List * const pList, uint32_t index,
    ListNode *pNode)
{
    /* ���ؽ�� */
    bool res = false;
    
    /* Ҫ����λ�õ�ǰһ���ڵ� */
    ListNode *pPreviousNode;
    
    ZF_ASSERT(pList != (List *)0)
    ZF_ASSERT(pNode != (ListNode *)0)
    
    if (index > pList->Count)
    {
        index = pList->Count;
    }
    
    /* ���ڵ� */
    if (index == 0)
    {
        /* ����ڵ� */
        pNode->Next = pList->pRootNode;
        pList->pRootNode = pNode;
        
        /* ������1 */
        pList->Count++;
        
        res = true;
    }
    else
    {
        /* ��Ҫ����λ�õ�ǰһ���ڵ� */
        if (List_getElementAt(pList, index - 1, &pPreviousNode))
        {
            /* ����ڵ� */
            pNode->Next = pPreviousNode->Next;
            pPreviousNode->Next = pNode;
            
            /* ������1 */
            pList->Count++;
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * ����  ����ָ��������ɾ���ڵ�(�ڲ��ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-index  ����
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_deleteElementAt(List * const pList, uint32_t index)
{
    /* Ҫɾ���Ľڵ� */
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
 * ����  ����ָ���������Ƴ��ڵ�(�ڲ����ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 *         (in)-index  ����
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_removeElementAt(List * const pList, uint32_t index)
{
    /* Ҫ�Ƴ��Ľڵ� */
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
 * ����  ������б�(�ڲ��ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_clear(List * const pList)
{
    /* ���ؽ�� */
    bool res = true;
    
    ZF_ASSERT(pList != (List *)0)
    
    while (pList->Count)
    {
        res &= List_deleteElementAt(pList, 0);
    }
    
    return res;
}

/******************************************************************************
 * ����  ���ͷ��б�(�ڲ��ͷſռ�)
 * ����  ��(in)-pList  �б�ṹ��ָ��
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_dispose(List * const pList)
{
    /* ���ؽ�� */
    bool res = true;
    
    ZF_ASSERT(pList != (List *)0)
    
    res = List_clear(pList);
    
    ZF_FREE(pList);
    
    return res;
}

/******************************************************************************
 * ����  �������ڵ�(�ڲ�����ռ䣬size=0��ʾʹ���ⲿ����)
 * ����  ��(out)-ppNode  �ڵ�ָ���ָ��
 *         (out)-ppData  ��ŵ����ݽṹָ���ָ��
 *         (in)-size    ��ŵ����ݽṹ��С(����0���ʾ���ڲ��������ݿռ�)
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
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
        
        /* ��� */
        *ppData = pData;
    }
    else
    {
        pNode->pData = NULL;
        pNode->Size = 0;
        pNode->IsExternData = true;
    }
    
    /* ��� */
    *ppNode = pNode;
    
    return true;
}

/******************************************************************************
 * ����  ���ͷŽڵ�(���ͷ��ⲿ����������)
 * ����  ��(in)-pNode  Ҫ�ͷŵĽڵ�
 * ����  ��-true       �ɹ�
 *         -false      ʧ��
******************************************************************************/
bool List_freeNode(ListNode *pNode)
{
    ZF_ASSERT(pNode != (ListNode *)0)
    
    /* �ⲿ���������ݲ��ͷ� */
    if (!pNode->IsExternData)
    {
        ZF_FREE(pNode->pData);
    }
    
    ZF_FREE(pNode);
    
    return true;
}

/******************************** END OF FILE ********************************/
