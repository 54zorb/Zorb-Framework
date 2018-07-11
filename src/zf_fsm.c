/**
  *****************************************************************************
  * @file    zf_fsm.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   ����״̬����ʵ��
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:�����ļ�
  *
  *****************************************************************************
  */

#include "zf_fsm.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"

/******************************************************************************
 * ����  ������״̬��(�ڲ�����ռ�)
 * ����  ��(out)-ppFsm   ״̬��ָ���ָ��
 * ����  ��-true         �ɹ�
 *         -false        ʧ��
******************************************************************************/
bool Fsm_create(Fsm ** ppFsm)
{
    Fsm *pFsm;
    
    ZF_ASSERT(ppFsm != (Fsm **)0)
    
    /* ����ռ� */
    pFsm = ZF_MALLOC(sizeof(Fsm));
    if (pFsm == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc fsm space error\r\n");
        return false;
    }
    
    /* ��ʼ����Ա */
    pFsm->Level = 1;
    pFsm->ChildList = NULL;
    pFsm->Owner = NULL;
    pFsm->OwnerTriggerState = NULL;
    pFsm->CurrentState = NULL;
    pFsm->IsRunning = false;
    
    /* ��ʼ������ */
    pFsm->SetInitialState = Fsm_setInitialState;
    pFsm->Run = Fsm_run;
    pFsm->RunAll = Fsm_runAll;
    pFsm->Stop = Fsm_stop;
    pFsm->StopAll = Fsm_stopAll;
    pFsm->Dispose = Fsm_dispose;
    pFsm->DisposeAll = Fsm_disposeAll;
    pFsm->AddChild = Fsm_addChild;
    pFsm->RemoveChild = Fsm_removeChild;
    pFsm->Dispatch = Fsm_dispatch;
    pFsm->Transfer = Fsm_transfer;
    pFsm->TransferWithEvent = Fsm_transferWithEvent;
    
    /* ��� */
    *ppFsm = pFsm;
    
    return true;
}

/******************************************************************************
 * ����  ������״̬����ʼ״̬
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-initialState   ״̬����ʼ״̬
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
void Fsm_setInitialState(Fsm * const pFsm, IFsmState initialState)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(initialState != (IFsmState)0)
    
    pFsm->CurrentState = initialState;
}

/******************************************************************************
 * ����  �����е�ǰ״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_run(Fsm * const pFsm)
{
    /* ���ؽ�� */
    bool res = false;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    if (!pFsm->IsRunning)
    {
        pFsm->IsRunning = true;
        
        res = true;
    }
    
    return res;
}

/******************************************************************************
 * ����  �����е�ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_runAll(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    Fsm_run(pFsm);
    
    if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
    {
        uint32_t i;
        Fsm * pChildFsm;
        
        for (i = 0; i < pFsm->ChildList->Count; i++)
        {
            pChildFsm = (Fsm *)pFsm->ChildList
                ->GetElementDataAt(pFsm->ChildList, i);
            
            if (pChildFsm != NULL)
            {
                Fsm_runAll(pChildFsm);
            }
        }
    }
    
    return true;
}

/******************************************************************************
 * ����  ��ֹͣ��ǰ״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_stop(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    pFsm->IsRunning = false;
    
    return true;
}

/******************************************************************************
 * ����  ��ֹͣ��ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_stopAll(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    Fsm_stop(pFsm);
    
    if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
    {
        uint32_t i;
        Fsm * pChildFsm;
        
        for (i = 0; i < pFsm->ChildList->Count; i++)
        {
            pChildFsm = (Fsm *)pFsm->ChildList
                ->GetElementDataAt(pFsm->ChildList, i);
            
            if (pChildFsm != NULL)
            {
                Fsm_stopAll(pChildFsm);
            }
        }
    }
    
    return true;
}

/******************************************************************************
 * ����  ���ͷŵ�ǰ״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_dispose(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    if (pFsm->ChildList != NULL)
    {
        pFsm->ChildList->Dispose(pFsm->ChildList);
    }
    
    ZF_FREE(pFsm);
    
    return true;
}

/******************************************************************************
 * ����  ���ͷŵ�ǰ״̬������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_disposeAll(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
    {
        uint32_t i;
        Fsm * pChildFsm;
        
        for (i = 0; i < pFsm->ChildList->Count; i++)
        {
            pChildFsm = (Fsm *)pFsm->ChildList
                ->GetElementDataAt(pFsm->ChildList, i);
            
            if (pChildFsm != NULL)
            {
                Fsm_disposeAll(pChildFsm);
            }
        }
    }
    
    Fsm_dispose(pFsm);
    
    return true;
}

/******************************************************************************
 * ����  �������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-pChildFsm      ��״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_addChild(Fsm * const pFsm, Fsm * const pChildFsm)
{
    /* ���ؽ�� */
    bool res = false;
    
    List *pList;
    ListNode *pNode;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(pChildFsm != (Fsm *)0)
    
    if (pFsm->ChildList == NULL)
    {
        List_create(&pList);
        pFsm->ChildList = pList;
    }
    
    pList = pFsm->ChildList;
    
    if (pList != NULL)
    {
        List_mallocNode(&pNode, NULL, 0);
        if (pNode != NULL)
        {
            pNode->pData = (void *)pChildFsm;
            pNode->Size = sizeof(Fsm);
            
            pList->Add(pList, pNode);
            
            pChildFsm->Owner = pFsm;
            pChildFsm->Level = pFsm->Level + 1;
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * ����  ���Ƴ���״̬��(���ͷſռ�)
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-pChildFsm      ��״̬��ָ��
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_removeChild(Fsm * const pFsm, Fsm * const pChildFsm)
{
    /* ���ؽ�� */
    bool res = false;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(pChildFsm != (Fsm *)0)
    
    if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
    {
        ListNode *pNode;
        
        /* �Ƴ�״̬�� */
        while(pFsm->ChildList
            ->GetElementByData(pFsm->ChildList, pChildFsm, &pNode))
        {
            if (pNode == NULL)
            {
                break;
            }
            
            if (!pFsm->ChildList->Delete(pFsm->ChildList, pNode))
            {
                ZF_DEBUG(LOG_E, "delete fsm node from list error\r\n");
                
                break;
            }
            
            res = true;
        }
    }
    
    return res;
}

/******************************************************************************
 * ����  ������״̬��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-signal         �����ź�
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
bool Fsm_dispatch(Fsm * const pFsm, FsmSignal const signal)
{
    /* ���ؽ�� */
    bool res = false;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    if (pFsm->IsRunning)
    {
        if (pFsm->CurrentState != NULL)
        {
            /* 1:��״̬��ʱ����
               2:û���ô���״̬ʱ����
               3:���ڴ���״̬ʱ����
             */
            if (pFsm->Owner == NULL || pFsm->OwnerTriggerState == NULL
                || pFsm->OwnerTriggerState == pFsm->Owner->CurrentState)
            {
                pFsm->CurrentState(pFsm, signal);
                
                res = true;
            }
        }
        
        if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
        {
            uint32_t i;
            Fsm * pChildFsm;
            
            for (i = 0; i < pFsm->ChildList->Count; i++)
            {
                pChildFsm = (Fsm *)pFsm->ChildList
                    ->GetElementDataAt(pFsm->ChildList, i);
                
                if (pChildFsm != NULL)
                {
                    Fsm_dispatch(pChildFsm, signal);
                }
            }
        }
    }
    
    return res;
}

/******************************************************************************
 * ����  ��״̬ת��
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-nextState      ת�ƺ�״̬
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
void Fsm_transfer(Fsm * const pFsm, IFsmState nextState)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(nextState != (IFsmState)0)
    
    pFsm->CurrentState = (IFsmState)nextState;
}

/******************************************************************************
 * ����  ��״̬ת��(����ת����ת���¼�)
 * ����  ��(in)-pFsm           ״̬��ָ��
 *         (in)-nextState      ת�ƺ�״̬
 * ����  ��-true               �ɹ�
 *         -false              ʧ��
******************************************************************************/
void Fsm_transferWithEvent(Fsm * const pFsm, IFsmState nextState)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(nextState != (IFsmState)0)
    
    Fsm_dispatch(pFsm, FSM_EXIT_SIG);
    
    Fsm_transfer(pFsm, nextState);
    
    Fsm_dispatch(pFsm, FSM_ENTER_SIG);
}

/******************************** END OF FILE ********************************/
