/**
  *****************************************************************************
  * @file    zf_event.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   �¼����¼���������ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
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

#define EVENT_HIGHEST_PRIORITY 0  /* �¼�������ȼ�(�û�������) */
#define EVENT_LOWEST_PRIORITY 32  /* �¼�������ȼ�(�û�������) */

/* �����¼� */
#define EVENT_POST(handler_, event_) handler_->Add(handler_, event_)

/* �¼� */
typedef struct _Event Event;
/* �¼������� */
typedef struct _EventHandler EventHandler;
/* �¼������������ */
typedef void (*IEventProcess)(List *pArgList);

/* �¼��ṹ */
struct _Event
{
    uint8_t Priority;               /* ���ȼ� */
    IEventProcess EventProcess;     /* �¼����� */
    List *pArgList;                 /* �¼�����Ĳ���ָ�� */
    
    /* ���ӳ������(��������Ⱥ�˳�������) */
    bool (*AddArg)(Event * const pEvent, void *pArg, uint32_t size);
    
    /* �����¼� */
    bool (*Dispose)(Event * const pEvent);
};

/* �¼��������ṹ */
struct _EventHandler
{
    List *pEventList;   /* �¼��б� */
    bool IsRunning;     /* �Ƿ���������:Ĭ�Ͽ� */
    
    /* ��ȡ�¼��� */
    uint32_t (*GetEventCount)(EventHandler * const pEventHandler);
    
    /* �����¼�(�����ȼ�����) */
    bool (*Add)(EventHandler * const pEventHandler, Event *pEvent);
    
    /* ɾ���¼�(�ͷſռ�) */
    bool (*Delete)(EventHandler * const pEventHandler, Event *pEvent);
    
    /* ����¼��б�(�ͷſռ�) */
    bool (*Clear)(EventHandler * const pEventHandler);
    
    /* �����¼�������(�ͷſռ�) */
    bool (*Dispose)(EventHandler * const pEventHandler);
    
    /* ִ���¼�(���б�λ��) */
    void (*Execute)(struct _EventHandler * const pEventHandler);
};

/* �����¼� */
bool Event_create(Event **ppEvent);

/* �����¼�����(��������Ⱥ�˳�������) */
bool Event_addArg(Event * const pEvent, void *pArg, uint32_t size);

/* �����¼� */
bool Event_Dispose(Event * const pEvent);

/* �����¼������� */
bool EventHandler_create(EventHandler **ppEventHandler);

/* ��ȡ�¼��� */
uint32_t EventHandler_getEventCount(EventHandler * const pEventHandler);

/* �����¼�(�����ȼ�����) */
bool EventHandler_add(EventHandler * const pEventHandler, Event *pEvent);

/* ɾ���¼�(�ͷſռ�) */
bool EventHandler_delete(EventHandler * const pEventHandler, Event *pEvent);

/* ����¼��б�(�ͷſռ�) */
bool EventHandler_clear(EventHandler * const pEventHandler);

/* �����¼�������(�ͷſռ�) */
bool EventHandler_dispose(EventHandler * const pEventHandler);

/* ִ���¼�(���б�λ��) */
void EventHandler_execute(EventHandler * const pEventHandler);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_EVENT_H__ */

/******************************** END OF FILE ********************************/
