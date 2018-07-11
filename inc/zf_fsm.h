/**
  *****************************************************************************
  * @file    zf_fsm.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ����״̬����ͷ�ļ�
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#ifndef __ZF_FSM_H__
#define __ZF_FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "zf_list.h"

/* ״̬���ź�0-31�������û��ź���32�Ժ��� */
enum {
    FSM_NULL_SIG = 0,
    FSM_ENTER_SIG,
    FSM_EXIT_SIG,
    FSM_USER_SIG_START = 32
    /* �û��ź������û��ļ����壬�������ڴ˶��� */
};

typedef struct _Fsm Fsm;

typedef uint32_t FsmSignal;

typedef void (*IFsmState)(Fsm * const, FsmSignal const);

/* ״̬���ṹ */
struct _Fsm
{
    uint8_t Level;                  /* Ƕ�ײ�������״̬������Ϊ1����״̬���������� */
                                    /* ע���Ͻ��ݹ�Ƕ�׺ͻ���Ƕ�� */
    List *ChildList;                /* ��״̬���б� */
    Fsm *Owner;                     /* ��״̬�� */
    IFsmState OwnerTriggerState;    /* ����״̬��Ϊ�趨״̬ʱ���Ŵ�����ǰ״̬�� */
                                    /* �����趨����ִ���길״̬��������������״̬�� */
    IFsmState CurrentState;         /* ��ǰ״̬ */
    bool IsRunning;                 /* �Ƿ���������(Ĭ�Ϲ�) */
    
    /* ���ó�ʼ״̬ */
    void (*SetInitialState)(Fsm * const pFsm, IFsmState initialState);
    
    /* ���е�ǰ״̬�� */
    bool (*Run)(Fsm * const pFsm);
    
    /* ���е�ǰ״̬������״̬�� */
    bool (*RunAll)(Fsm * const pFsm);
    
    /* ֹͣ��ǰ״̬�� */
    bool (*Stop)(Fsm * const pFsm);
    
    /* ֹͣ��ǰ״̬������״̬�� */
    bool (*StopAll)(Fsm * const pFsm);
    
    /* �ͷŵ�ǰ״̬�� */
    bool (*Dispose)(Fsm * const pFsm);
    
    /* �ͷŵ�ǰ״̬������״̬�� */
    bool (*DisposeAll)(Fsm * const pFsm);
    
    /* �����״̬�� */
    bool (*AddChild)(Fsm * const pFsm, Fsm * const pChildFsm);
    
    /* �Ƴ���״̬��(���ͷſռ�) */
    bool (*RemoveChild)(Fsm * const pFsm, Fsm * const pChildFsm);
    
    /* ����״̬�� */
    bool (*Dispatch)(Fsm * const pFsm, FsmSignal const signal);
    
    /* ״̬ת�� */
    void (*Transfer)(Fsm * const pFsm, IFsmState nextState);
    
    /* ״̬ת��(����ת����ת���¼�) */
    void (*TransferWithEvent)(Fsm * const pFsm, IFsmState nextState);
};

/* ����״̬��(�ڲ�����ռ�) */
bool Fsm_create(Fsm ** ppFsm);

/* ���ó�ʼ״̬ */
void Fsm_setInitialState(Fsm * const pFsm, IFsmState initialState);

/* ���е�ǰ״̬�� */
bool Fsm_run(Fsm * const pFsm);

/* ���е�ǰ״̬������״̬�� */
bool Fsm_runAll(Fsm * const pFsm);

/* ֹͣ��ǰ״̬�� */
bool Fsm_stop(Fsm * const pFsm);

/* ֹͣ��ǰ״̬������״̬�� */
bool Fsm_stopAll(Fsm * const pFsm);

/* �ͷŵ�ǰ״̬�� */
bool Fsm_dispose(Fsm * const pFsm);

/* �ͷŵ�ǰ״̬������״̬�� */
bool Fsm_disposeAll(Fsm * const pFsm);

/* �����״̬�� */
bool Fsm_addChild(Fsm * const pFsm, Fsm * const pChildFsm);

/* �Ƴ���״̬��(���ͷſռ�) */
bool Fsm_removeChild(Fsm * const pFsm, Fsm * const pChildFsm);

/* ����״̬�� */
bool Fsm_dispatch(Fsm * const pFsm, FsmSignal const signal);

/* ״̬ת�� */
void Fsm_transfer(Fsm * const pFsm, IFsmState nextState);

/* ״̬ת��(����ת����ת���¼�) */
void Fsm_transferWithEvent(Fsm * const pFsm, IFsmState nextState);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_FSM_H__ */

/******************************** END OF FILE ********************************/
