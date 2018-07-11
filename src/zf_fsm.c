/**
  *****************************************************************************
  * @file    zf_fsm.c
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   有限状态机的实现
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
  *
  *****************************************************************************
  */

#include "zf_fsm.h"
#include "zf_assert.h"
#include "zf_debug.h"
#include "zf_malloc.h"

/******************************************************************************
 * 描述  ：创建状态机(内部分配空间)
 * 参数  ：(out)-ppFsm   状态机指针的指针
 * 返回  ：-true         成功
 *         -false        失败
******************************************************************************/
bool Fsm_create(Fsm ** ppFsm)
{
    Fsm *pFsm;
    
    ZF_ASSERT(ppFsm != (Fsm **)0)
    
    /* 分配空间 */
    pFsm = ZF_MALLOC(sizeof(Fsm));
    if (pFsm == NULL)
    {
        ZF_DEBUG(LOG_E, "malloc fsm space error\r\n");
        return false;
    }
    
    /* 初始化成员 */
    pFsm->Level = 1;
    pFsm->ChildList = NULL;
    pFsm->Owner = NULL;
    pFsm->OwnerTriggerState = NULL;
    pFsm->CurrentState = NULL;
    pFsm->IsRunning = false;
    
    /* 初始化方法 */
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
    
    /* 输出 */
    *ppFsm = pFsm;
    
    return true;
}

/******************************************************************************
 * 描述  ：设置状态机初始状态
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-initialState   状态机初始状态
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
void Fsm_setInitialState(Fsm * const pFsm, IFsmState initialState)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(initialState != (IFsmState)0)
    
    pFsm->CurrentState = initialState;
}

/******************************************************************************
 * 描述  ：运行当前状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool Fsm_run(Fsm * const pFsm)
{
    /* 返回结果 */
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
 * 描述  ：运行当前状态机和子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
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
 * 描述  ：停止当前状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool Fsm_stop(Fsm * const pFsm)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    pFsm->IsRunning = false;
    
    return true;
}

/******************************************************************************
 * 描述  ：停止当前状态机和子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
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
 * 描述  ：释放当前状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
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
 * 描述  ：释放当前状态机和子状态机
 * 参数  ：(in)-pFsm           状态机指针
 * 返回  ：-true               成功
 *         -false              失败
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
 * 描述  ：添加子状态机
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-pChildFsm      子状态机指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool Fsm_addChild(Fsm * const pFsm, Fsm * const pChildFsm)
{
    /* 返回结果 */
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
 * 描述  ：移除子状态机(不释放空间)
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-pChildFsm      子状态机指针
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool Fsm_removeChild(Fsm * const pFsm, Fsm * const pChildFsm)
{
    /* 返回结果 */
    bool res = false;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(pChildFsm != (Fsm *)0)
    
    if (pFsm->ChildList != NULL && pFsm->ChildList->Count > 0)
    {
        ListNode *pNode;
        
        /* 移除状态机 */
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
 * 描述  ：调度状态机
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-signal         调度信号
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
bool Fsm_dispatch(Fsm * const pFsm, FsmSignal const signal)
{
    /* 返回结果 */
    bool res = false;
    
    ZF_ASSERT(pFsm != (Fsm *)0)
    
    if (pFsm->IsRunning)
    {
        if (pFsm->CurrentState != NULL)
        {
            /* 1:根状态机时调度
               2:没设置触发状态时调度
               3:正在触发状态时调度
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
 * 描述  ：状态转移
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-nextState      转移后状态
 * 返回  ：-true               成功
 *         -false              失败
******************************************************************************/
void Fsm_transfer(Fsm * const pFsm, IFsmState nextState)
{
    ZF_ASSERT(pFsm != (Fsm *)0)
    ZF_ASSERT(nextState != (IFsmState)0)
    
    pFsm->CurrentState = (IFsmState)nextState;
}

/******************************************************************************
 * 描述  ：状态转移(触发转出和转入事件)
 * 参数  ：(in)-pFsm           状态机指针
 *         (in)-nextState      转移后状态
 * 返回  ：-true               成功
 *         -false              失败
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
