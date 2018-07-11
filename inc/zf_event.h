/**
  *****************************************************************************
  * @file    zf_event.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   事件和事件处理器的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#ifndef __ZF_EVENT_H__
#define __ZF_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"
#include "zf_list.h"

#define EVENT_HIGHEST_PRIORITY 0  /* 事件最高优先级(用户不可用) */
#define EVENT_LOWEST_PRIORITY 32  /* 事件最低优先级(用户不可用) */

/* 推送事件 */
#define EVENT_POST(handler_, event_) handler_->Add(handler_, event_)

/* 事件 */
typedef struct _Event Event;
/* 事件处理器 */
typedef struct _EventHandler EventHandler;
/* 事件处理程序类型 */
typedef void (*IEventProcess)(List *pArgList);

/* 事件结构 */
struct _Event
{
    uint8_t Priority;               /* 优先级 */
    IEventProcess EventProcess;     /* 事件程序 */
    List *pArgList;                 /* 事件程序的参数指针 */
    
    /* 增加程序参数(深拷贝，按先后顺序入队列) */
    bool (*AddArg)(Event * const pEvent, void *pArg, uint32_t size);
    
    /* 销毁事件 */
    bool (*Dispose)(Event * const pEvent);
};

/* 事件处理器结构 */
struct _EventHandler
{
    List *pEventList;   /* 事件列表 */
    bool IsRunning;     /* 是否正在运行:默认开 */
    
    /* 获取事件数 */
    uint32_t (*GetEventCount)(EventHandler * const pEventHandler);
    
    /* 增加事件(按优先级排序) */
    bool (*Add)(EventHandler * const pEventHandler, Event *pEvent);
    
    /* 删除事件(释放空间) */
    bool (*Delete)(EventHandler * const pEventHandler, Event *pEvent);
    
    /* 清空事件列表(释放空间) */
    bool (*Clear)(EventHandler * const pEventHandler);
    
    /* 销毁事件处理器(释放空间) */
    bool (*Dispose)(EventHandler * const pEventHandler);
    
    /* 执行事件(按列表位置) */
    void (*Execute)(struct _EventHandler * const pEventHandler);
};

/* 创建事件 */
bool Event_create(Event **ppEvent);

/* 增加事件参数(深拷贝，按先后顺序入队列) */
bool Event_addArg(Event * const pEvent, void *pArg, uint32_t size);

/* 销毁事件 */
bool Event_Dispose(Event * const pEvent);

/* 创建事件处理器 */
bool EventHandler_create(EventHandler **ppEventHandler);

/* 获取事件数 */
uint32_t EventHandler_getEventCount(EventHandler * const pEventHandler);

/* 增加事件(按优先级排序) */
bool EventHandler_add(EventHandler * const pEventHandler, Event *pEvent);

/* 删除事件(释放空间) */
bool EventHandler_delete(EventHandler * const pEventHandler, Event *pEvent);

/* 清空事件列表(释放空间) */
bool EventHandler_clear(EventHandler * const pEventHandler);

/* 销毁事件处理器(释放空间) */
bool EventHandler_dispose(EventHandler * const pEventHandler);

/* 执行事件(按列表位置) */
void EventHandler_execute(EventHandler * const pEventHandler);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_EVENT_H__ */

/******************************** END OF FILE ********************************/
