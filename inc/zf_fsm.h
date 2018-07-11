/**
  *****************************************************************************
  * @file    zf_fsm.h
  * @author  Zorb
  * @version V1.0.0
  * @date    2018-06-28
  * @brief   有限状态机的头文件
  *****************************************************************************
  * @history
  *
  * 1. Date:2018-06-28
  *    Author:Zorb
  *    Modification:建立文件
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

/* 状态机信号0-31保留，用户信号在32以后定义 */
enum {
    FSM_NULL_SIG = 0,
    FSM_ENTER_SIG,
    FSM_EXIT_SIG,
    FSM_USER_SIG_START = 32
    /* 用户信号请在用户文件定义，不允许在此定义 */
};

typedef struct _Fsm Fsm;

typedef uint32_t FsmSignal;

typedef void (*IFsmState)(Fsm * const, FsmSignal const);

/* 状态机结构 */
struct _Fsm
{
    uint8_t Level;                  /* 嵌套层数，根状态机层数为1，子状态机层数自增 */
                                    /* 注：严禁递归嵌套和环形嵌套 */
    List *ChildList;                /* 子状态机列表 */
    Fsm *Owner;                     /* 父状态机 */
    IFsmState OwnerTriggerState;    /* 当父状态机为设定状态时，才触发当前状态机 */
                                    /* 若不设定，则当执行完父状态机，立即运行子状态机 */
    IFsmState CurrentState;         /* 当前状态 */
    bool IsRunning;                 /* 是否正在运行(默认关) */
    
    /* 设置初始状态 */
    void (*SetInitialState)(Fsm * const pFsm, IFsmState initialState);
    
    /* 运行当前状态机 */
    bool (*Run)(Fsm * const pFsm);
    
    /* 运行当前状态机和子状态机 */
    bool (*RunAll)(Fsm * const pFsm);
    
    /* 停止当前状态机 */
    bool (*Stop)(Fsm * const pFsm);
    
    /* 停止当前状态机和子状态机 */
    bool (*StopAll)(Fsm * const pFsm);
    
    /* 释放当前状态机 */
    bool (*Dispose)(Fsm * const pFsm);
    
    /* 释放当前状态机和子状态机 */
    bool (*DisposeAll)(Fsm * const pFsm);
    
    /* 添加子状态机 */
    bool (*AddChild)(Fsm * const pFsm, Fsm * const pChildFsm);
    
    /* 移除子状态机(不释放空间) */
    bool (*RemoveChild)(Fsm * const pFsm, Fsm * const pChildFsm);
    
    /* 调度状态机 */
    bool (*Dispatch)(Fsm * const pFsm, FsmSignal const signal);
    
    /* 状态转移 */
    void (*Transfer)(Fsm * const pFsm, IFsmState nextState);
    
    /* 状态转移(触发转出和转入事件) */
    void (*TransferWithEvent)(Fsm * const pFsm, IFsmState nextState);
};

/* 创建状态机(内部分配空间) */
bool Fsm_create(Fsm ** ppFsm);

/* 设置初始状态 */
void Fsm_setInitialState(Fsm * const pFsm, IFsmState initialState);

/* 运行当前状态机 */
bool Fsm_run(Fsm * const pFsm);

/* 运行当前状态机和子状态机 */
bool Fsm_runAll(Fsm * const pFsm);

/* 停止当前状态机 */
bool Fsm_stop(Fsm * const pFsm);

/* 停止当前状态机和子状态机 */
bool Fsm_stopAll(Fsm * const pFsm);

/* 释放当前状态机 */
bool Fsm_dispose(Fsm * const pFsm);

/* 释放当前状态机和子状态机 */
bool Fsm_disposeAll(Fsm * const pFsm);

/* 添加子状态机 */
bool Fsm_addChild(Fsm * const pFsm, Fsm * const pChildFsm);

/* 移除子状态机(不释放空间) */
bool Fsm_removeChild(Fsm * const pFsm, Fsm * const pChildFsm);

/* 调度状态机 */
bool Fsm_dispatch(Fsm * const pFsm, FsmSignal const signal);

/* 状态转移 */
void Fsm_transfer(Fsm * const pFsm, IFsmState nextState);

/* 状态转移(触发转出和转入事件) */
void Fsm_transferWithEvent(Fsm * const pFsm, IFsmState nextState);

#ifdef __cplusplus
}
#endif

#endif /* __ZF_FSM_H__ */

/******************************** END OF FILE ********************************/
