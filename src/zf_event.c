/**
  *****************************************************************************
  * @file    zf_event.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   事件和事件处理器的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_event.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"
#include "zf_critical.h"

/******************************************************************************
 * 描述  ：创建事件
 * 参数  ：(out)-ppEvent  事件指针的指针
 * 返回  ：-true          成功
 *         -false         失败
******************************************************************************/
bool Event_create(Event **ppEvent)
{
    Event *pEvent;
    
    ZF_ASSERT(ppEvent != (Event **)0)
    
    /* 分配空间 */
    pEvent = ZF_MALLOC(sizeof(Event));
    if (pEvent == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc event space error\r\n");
        return false;
    }
    
    /* 初始化成员 */
    pEvent->Priority = EVENT_LOWEST_PRIORITY;
    pEvent->EventProcess = NULL;
    pEvent->pArgList = NULL;
    
    /* 初始化方法 */
    pEvent->AddArg = Event_addArg;
    pEvent->Dispose = Event_Dispose;
    
    /* 输出 */
    *ppEvent = pEvent;
    
    return true;
}

/******************************************************************************
 * 描述  ：增加事件参数(深拷贝，按先后顺序入队列)
 * 参数  ：(in)-pEvent  事件指针
 *         (in)-pArg    要增加的参数
 *         (in)-size    要增加的大小
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool Event_addArg(Event * const pEvent, void *pArg, uint32_t size)
{
    ListNode *pNode;
    void *pData;
    
    ZF_ASSERT(pEvent != (Event *)0)
    ZF_ASSERT(pArg != (void *)0)
    ZF_ASSERT(size > 0)
    
    /* 创建参数列表 */
    if (pEvent->pArgList == NULL)
    {
        List_create(&pEvent->pArgList);
        
        if (pEvent->pArgList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc event arg list space error\r\n");
            
            return false;
        }
    }
    
    /* 创建参数节点 */
    if (!List_mallocNode(&pNode, &pData, size))
    {
        ZF_DEBUG(LOG_E, "malloc event arg node space error\r\n");
        
        return false;
    }
    
    /* 深拷贝参数数据 */
    ZF_MEMCPY(pData, pArg, size);
    
    /* 添加到参数列表 */
    if (!pEvent->pArgList->Add(pEvent->pArgList, pNode))
    {
        ZF_DEBUG(LOG_E, "add event arg node into event arg list error\r\n");
        
        return false;
    }
    
    return true;
}

/******************************************************************************
 * 描述  ：销毁事件(程序参数也同步销毁)
 * 参数  ：(in)-pEvent  事件指针
 * 返回  ：-true        成功
 *         -false       失败
******************************************************************************/
bool Event_Dispose(Event * const pEvent)
{
    ZF_ASSERT(pEvent != (Event *)0)
    
    /* 销毁参数列表 */
    if (pEvent->pArgList != NULL)
    {
        pEvent->pArgList->Dispose(pEvent->pArgList);
    }
    
    ZF_FREE(pEvent);
    
    return true;
}

/******************************************************************************
 * 描述  ：创建事件处理器
 * 参数  ：(out)-ppEventHandler  事件处理器指针的指针
 * 返回  ：-true                 成功
 *         -false                失败
******************************************************************************/
bool EventHandler_create(EventHandler **ppEventHandler)
{
    EventHandler *pEventHandler;
    
    ZF_ASSERT(ppEventHandler != (EventHandler **)0)
    
    /* 分配空间 */
    pEventHandler = ZF_MALLOC(sizeof(EventHandler));
    if (pEventHandler == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc event handler space error\r\n");
        return false;
    }
    
    /* 初始化成员 */
    pEventHandler->pEventList = NULL;
    pEventHandler->IsRunning = true;
    
    /* 初始化方法 */
    pEventHandler->GetEventCount = EventHandler_getEventCount;
    pEventHandler->Add = EventHandler_add;
    pEventHandler->Delete = EventHandler_delete;
    pEventHandler->Clear = EventHandler_clear;
    pEventHandler->Dispose = EventHandler_dispose;
    pEventHandler->Execute = EventHandler_execute;
    
    /* 输出 */
    *ppEventHandler = pEventHandler;
    
    return true;
}

/******************************************************************************
 * 描述  ：获取事件数
 * 参数  ：(in)-pEventHandler  事件处理器指针
 * 返回  ：事件数
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
 * 描述  ：增加事件(按优先级排序)
 * 参数  ：(in)-pEventHandler  事件处理器指针
 *         (in)-pEvent         事件指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool EventHandler_add(EventHandler * const pEventHandler, Event *pEvent)
{
    ListNode *pNode;
    uint32_t i;
    uint32_t index; /* 插入事件位置索引 */
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    ZF_ASSERT(pEvent != (Event *)0)
    
    /* 创建事件列表 */
    if (pEventHandler->pEventList == NULL)
    {
        List_create(&pEventHandler->pEventList);
        
        if (pEventHandler->pEventList == NULL)
        {
            ZF_DEBUG(LOG_E, "malloc event list space error\r\n");
            
            return false;
        }
    }
    
    /* 创建事件节点 */
    if (!List_mallocNode(&pNode, NULL, 0))
    {
        ZF_DEBUG(LOG_E, "malloc event node space error\r\n");
        
        return false;
    }
    
    pNode->pData = (void *)pEvent;
    pNode->Size = sizeof(Event);
    
    /* 按优先级排序 */
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
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 添加到事件列表 */
    if (!pEventHandler->pEventList
        ->AddElementAt(pEventHandler->pEventList, index, pNode))
    {
        ZF_DEBUG(LOG_E, "add event node into event list error\r\n");
        
        /* 退出临界区 */
        ZF_CRITICAL_EXIT();
        
        return false;
    }
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    return true;
}

/******************************************************************************
 * 描述  ：删除事件(释放空间)
 * 参数  ：(in)-pEventHandler  事件处理器指针
 *         (in)-pEvent         事件指针
 * 返回  ：-true               成功
 *         -false              失败
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
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    /* 移除事件 */
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
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    ZF_FREE(pEvent);
    
    return true;
}

/******************************************************************************
 * 描述  ：清空事件列表(释放空间)
 * 参数  ：(in)-pEventHandler  事件处理器指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool EventHandler_clear(EventHandler * const pEventHandler)
{
    /* 返回结果 */
    bool res = true;
    
    uint32_t i;
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    if (pEventHandler->pEventList == NULL ||
        pEventHandler->pEventList->Count == 0)
    {
        return true;
    }
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    for (i = 0; i < pEventHandler->pEventList->Count; i++)
    {
        /* 释放所有事件 */
        ZF_FREE(pEventHandler->pEventList
            ->GetElementDataAt(pEventHandler->pEventList, i));
    }
    
    res = pEventHandler->pEventList->Clear(pEventHandler->pEventList);
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    return res;
}

/******************************************************************************
 * 描述  ：销毁事件处理器(释放空间)
 * 参数  ：(in)-pEventHandler  事件处理器指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool EventHandler_dispose(EventHandler * const pEventHandler)
{
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    EventHandler_clear(pEventHandler);
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    pEventHandler->pEventList->Dispose(pEventHandler->pEventList);
    
    pEventHandler->pEventList = NULL;
    
    /* 退出临界区 */
    ZF_CRITICAL_EXIT();
    
    ZF_FREE(pEventHandler);
    
    return true;
}

/******************************************************************************
 * 描述  ：执行事件(按列表位置)
 * 参数  ：(in)-pEventHandler  事件处理器指针
 * 返回  ：无
******************************************************************************/
void EventHandler_execute(EventHandler * const pEventHandler)
{
    Event *pEvent;
    
    ZF_ASSERT(pEventHandler != (EventHandler *)0)
    
    /* 没有运行则退出 */
    if (!pEventHandler->IsRunning)
    {
        return;
    }
    
    /* 没有事件则退出 */
    if (pEventHandler->pEventList == NULL 
        || pEventHandler->pEventList->Count == 0)
    {
        return;
    }
    
    /* 进入临界区 */
    ZF_CRITICAL_ENTER();
    
    pEvent = (Event *)pEventHandler->pEventList
        ->GetElementDataAt(pEventHandler->pEventList, 0);
    
    /* 退出临界区 */
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
