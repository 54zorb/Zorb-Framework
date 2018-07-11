/**
  *****************************************************************************
  * @file    zf_event.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   �¼����¼���������ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_event.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"
#include "zf_critical.h"

/******************************************************************************
 * ����  �������¼�
 * ����  ��(out)-ppEvent  �¼�ָ���ָ��
 * ����  ��-true          �ɹ�
 *         -false         ʧ��
******************************************************************************/
bool Event_create(Event **ppEvent)
{
    Event *pEvent;
    
    ZF_ASSERT(ppEvent != (Event **)0)
    
    /* ����ռ� */
    pEvent = ZF_MALLOC(sizeof(Event));
    if (pEvent == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc event space error\r\n");
        return false;
    }
    
    /* ��ʼ����Ա */
    pEvent->Priority = EVENT_LOWEST_PRIORITY;
    pEvent->EventProcess = NULL;
    pEvent->pArgList = NULL;
    
    /* ��ʼ������ */
    pEvent->AddArg = Event_addArg;
    pEvent->Dispose = Event_Dispose;
    
    /* ��� */
    *ppEvent = pEvent;
    
    return true;
}

/******************************************************************************
 * ����  �������¼�����(��������Ⱥ�˳�������)
 * ����  ��(in)-pEvent  �¼�ָ��
 *         (in)-pArg    Ҫ���ӵĲ���
 *         (in)-size    Ҫ���ӵĴ�С
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool Event_addArg(Event * const pEvent, void *pArg, uint32_t size)
{
    ListNode *pNode;
    void *pData;
    
    ZF_ASSERT(pEvent != (Event *)0)
    ZF_ASSERT(pArg != (void *)0)
    ZF_ASSERT(size > 0)
    
    /* ���������б� */
    if (pEvent->pArgList == NULL)
    {
        List_create(&pEvent->pArgList);
        
        if (pEvent->pArgList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc event arg list space error\r\n");
            
            return false;
        }
    }
    
    /* ���������ڵ� */
    if (!List_mallocNode(&pNode, &pData, size))
    {
        ZF_DEBUG(LOG_E, "malloc event arg node space error\r\n");
        
        return false;
    }
    
    /* ����������� */
    ZF_MEMCPY(pData, pArg, size);
    
    /* ��ӵ������б� */
    if (!pEvent->pArgList->Add(pEvent->pArgList, pNode))
    {
        ZF_DEBUG(LOG_E, "add event arg node into event arg list error\r\n");
        
        return false;
    }
    
    return true;
}

/******************************************************************************
 * ����  �������¼�(�������Ҳͬ������)
 * ����  ��(in)-pEvent  �¼�ָ��
 * ����  ��-true        �ɹ�
 *         -false       ʧ��
******************************************************************************/
bool Event_Dispose(Event * const pEvent)
{
    ZF_ASSERT(pEvent != (Event *)0)
    
    /* ���ٲ����б� */
    if (pEvent->pArgList != NULL)
    {
        pEvent->pArgList->Dispose(pEvent->pArgList);
    }
    
    ZF_FREE(pEvent);
    
    return true;
}

/******************************************************************************
 * ����  �������¼�������
 * ����  ��(out)-ppEventHandler  �¼�������ָ���ָ��
 * ����  ��-true                 �ɹ�
 *         -false                ʧ��
******************************************************************************/
bool EventHandler_create(EventHandler **ppEventHandler)
{
    EventHandler *pEventHandler;
    
    ZF_ASSERT(ppEventHandler != (EventHandler **)0)
    
    /* ����ռ� */
    pEventHandler = ZF_MALLOC(sizeof(EventHandler));
    if (pEventHandler == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc event handler space error\r\n");
        return false;
    }
    
    /* ��ʼ����Ա */
    pEventHandler->pEventList = NULL;
    pEventHandler->IsRunning = true;
    
    /* ��ʼ������ */
    pEventHandler->GetEventCount = EventHandler_getEventCount;
    pEventHandler->Add = EventHandler_add;
    pEventHandler->Delete = EventHandler_delete;
    pEventHandler->Clear = EventHandler_clear;
    pEventHandler->Dispose = EventHandler_dispose;
    pEventHandler->Execute = EventHandler_execute;
    
    /* ��� */
    *ppEventHandler = pEventHandler;
    
    return true;
}

/******************************************************************************
 * ����  ����ȡ�¼���
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 * ����  ���¼���
******************************************************************************/
uint32_t EventHandler_getEventCount(EventHandler * const pEventHandler)
{
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    if (pEventHandler->pEventList == NULL)
    {
        return 0;
    }
    else
    {
        return pEventHandler->pEventList->Count;
    }
}

/******************************************************************************
 * ����  �������¼�(�����ȼ�����)
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 *         (in)-pEvent         �¼�ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool EventHandler_add(EventHandler * const pEventHandler, Event *pEvent)
{
    ListNode *pNode;
    uint32_t i;
    uint32_t index; /* �����¼�λ������ */
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    ZF_ASSERT(pEvent != (Event *)0)
    
    /* �����¼��б� */
    if (pEventHandler->pEventList == NULL)
    {
        List_create(&pEventHandler->pEventList);
        
        if (pEventHandler->pEventList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc event list space error\r\n");
            
            return false;
        }
    }
    
    /* �����¼��ڵ� */
    if (!List_mallocNode(&pNode, NULL, 0))
    {
        ZF_DEBUG(LOG_E, "malloc event node space error\r\n");
        
        return false;
    }
    
    pNode->pData = (void *)pEvent;
    pNode->Size = sizeof(Event);
    
    /* �����ȼ����� */
    index = pEventHandler->pEventList->Count;
    
    for (i = 0; i < pEventHandler->pEventList->Count; i++)
    {
        if (pEvent->Priority < ((Event *)pEventHandler->pEventList
            ->GetElementDataAt(pEventHandler->pEventList, i))->Priority)
        {
            index = i;
            break;
        }
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* ��ӵ��¼��б� */
    if (!pEventHandler->pEventList
        ->AddElementAt(pEventHandler->pEventList, index, pNode))
    {
        ZF_DEBUG(LOG_E, "add event node into event list error\r\n");
        
        /* �˳��ٽ��� */
        ZF_CRITICAL_EXIT();
        
        return false;
    }
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    return true;
}

/******************************************************************************
 * ����  ��ɾ���¼�(�ͷſռ�)
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 *         (in)-pEvent         �¼�ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool EventHandler_delete(EventHandler * const pEventHandler, Event *pEvent)
{
    ListNode *pNode;
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    ZF_ASSERT(pEvent != (Event *)0)
    
    if (pEventHandler->pEventList == NULL ||
        pEventHandler->pEventList->Count == 0)
    {
        return false;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    /* �Ƴ��¼� */
    while(pEventHandler->pEventList
        ->GetElementByData(pEventHandler->pEventList, pEvent, &pNode))
    {
        if (pNode == NULL)
        {
            break;
        }
        
        if (!pEventHandler->pEventList->Delete(pEventHandler->pEventList, pNode))
        {
            ZF_DEBUG(LOG_E, "delete event node from list error\r\n");
            
            break;
        }
    }
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    ZF_FREE(pEvent);
    
    return true;
}

/******************************************************************************
 * ����  ������¼��б�(�ͷſռ�)
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool EventHandler_clear(EventHandler * const pEventHandler)
{
    /* ���ؽ�� */
    bool res = true;
    
    uint32_t i;
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    if (pEventHandler->pEventList == NULL ||
        pEventHandler->pEventList->Count == 0)
    {
        return true;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    for (i = 0; i < pEventHandler->pEventList->Count; i++)
    {
        /* �ͷ������¼� */
        ZF_FREE(pEventHandler->pEventList
            ->GetElementDataAt(pEventHandler->pEventList, i));
    }
    
    res = pEventHandler->pEventList->Clear(pEventHandler->pEventList);
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    return res;
}

/******************************************************************************
 * ����  �������¼�������(�ͷſռ�)
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool EventHandler_dispose(EventHandler * const pEventHandler)
{
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    EventHandler_clear(pEventHandler);
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    pEventHandler->pEventList->Dispose(pEventHandler->pEventList);
    
    pEventHandler->pEventList = NULL;
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    ZF_FREE(pEventHandler);
    
    return true;
}

/******************************************************************************
 * ����  ��ִ���¼�(���б�λ��)
 * ����  ��(in)-pEventHandler  �¼�������ָ��
 * ����  ����
******************************************************************************/
void EventHandler_execute(EventHandler * const pEventHandler)
{
    Event *pEvent;
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    /* û���������˳� */
    if (!pEventHandler->IsRunning)
    {
        return;
    }
    
    /* û���¼����˳� */
    if (pEventHandler->pEventList == NULL 
        || pEventHandler->pEventList->Count == 0)
    {
        return;
    }
    
    /* �����ٽ��� */
    ZF_CRITICAL_ENTER();
    
    pEvent = (Event *)pEventHandler->pEventList
        ->GetElementDataAt(pEventHandler->pEventList, 0);
    
    /* �˳��ٽ��� */
    ZF_CRITICAL_EXIT();
    
    if (pEvent == NULL || pEvent->EventProcess == NULL)
    {
        ZF_DEBUG(LOG_E, "event handler execute error:event is null\r\n");
        while(1);
    }
    
    pEvent->EventProcess(pEvent->pArgList);
    
    EventHandler_delete(pEventHandler, pEvent);
}

/******************************** END OF FILE ********************************/
