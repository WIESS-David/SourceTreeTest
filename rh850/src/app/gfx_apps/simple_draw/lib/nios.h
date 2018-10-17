#include <stdint.h>
#include <stddef.h>

#define _FALSE                      (uint8_t)0
#define _TRUE                       (uint8_t)1

// Task Define
#define _TASK_NUM                   (uint8_t)0x03
#define _TASK_QUEUE_SIZE            (uint8_t)0x03
#define _RECORDER_QUEUE_SIZE        (uint8_t)0x03
//----------------------------------------------------------------------------------------------------------------------------
// Type Define
#define _TYPE_NONE                  (uint8_t)0xFF
//----------------------------------------------------------------------------------------------------------------------------
// State Define
#define _STATE_NONE                 (uint8_t)0xFF
#define _STATE_INIT                 (uint8_t)0x00
//----------------------------------------------------------------------------------------------------------------------------
// Event Define
#define _EVENT_NONE                 (uint8_t)0xFF
#define _EVENT_FIRST                (uint8_t)0x01
#define _EVENT_DATA                 (uint8_t)0x02
#define _EVENT_TIMER1               (uint8_t)0x03
#define _EVENT_TIMER2               (uint8_t)0x04
#define _EVENT_TIMER3               (uint8_t)0x05
#define _EVENT_TIMER4               (uint8_t)0x06
#define _EVENT_LOOP                 (uint8_t)0x07
#define _EVENT_ERROR                (uint8_t)0xFE
//----------------------------------------------------------------------------------------------------------------------------
// Task Level Define
#define _LEVEL_NONE                 (uint8_t)0xFF
#define _LEVEL1                     (uint8_t)0x01                       // The task is not impotant and periodic.
#define _LEVEL2                     (uint8_t)0x02                       // The task is a middle level and periodic.
#define _LEVEL3                     (uint8_t)0x03                       // The task is very important and periodic.
#define _LEVEL4                     (uint8_t)0x04                       // The task is very important and nonperiodic.
#define _LEVEL5                     (uint8_t)0x05                       // The task is excuted first time.
//----------------------------------------------------------------------------------------------------------------------------
// Handler methods
typedef void (*task_handler_def)(void);
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    uint8_t u8type;
    uint8_t u8state;
    uint8_t u8event;
    uint8_t u8level;
    task_handler_def ttask_handler;
}nios_task_def;
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    nios_task_def ttask[_RECORDER_QUEUE_SIZE];
    uint8_t u8current_point;
    uint8_t u8end_point;
    uint8_t u8pre_type;
    uint8_t u8pre_state;
    uint8_t u8pre_event;
    uint8_t u8pre_level;
    task_handler_def tpre_task_handler;
    uint8_t u8task_num;
    uint8_t u8queue_full;
    uint8_t u8discard_time;
}task_recorder_def;
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    nios_task_def ttask[_TASK_QUEUE_SIZE];
    uint8_t u8current_point;
    uint8_t u8end_point;
}queue_task_def;
//----------------------------------------------------------------------------------------------------------------------------
uint8_t W_Task_ExcludeDone(task_recorder_def* precorder);
void W_Task_AssignedPriority(task_recorder_def* precorder,uint8_t u8undo_num);
void W_Task_AssignedTask(task_recorder_def* precorder);
void W_Task_QueueManagerInit(queue_task_def* pqueue_manager);
void W_Task_RecorderInit(task_recorder_def* precorder);
void W_Task_TaskInRecorder(void (*call_fun)(void),uint8_t u8type, uint8_t u8counter);
uint8_t W_Task_ChangeEvent(uint8_t u8type,uint8_t u8level, uint8_t u8event);
uint8_t W_Task_ChangeState(uint8_t u8type,uint8_t u8level, uint8_t u8state, void (*call_fun)(void));
void W_Task_TaskDone(uint8_t u8index);
void W_Task_TaskInit(void);
void W_Task_ClearTaskQueue(nios_task_def *pttask);
void W_Task_ClearRecorderQueue(nios_task_def *pttask);
void W_Task_PullTask(void);
void W_Task_PushTaskInQueue(queue_task_def* pqueue_manager, task_recorder_def* precorder, uint8_t u8undo_num);
void W_Task_ExcuteTask(void);
void W_Task_PushState(void);
void W_Task_ProcessTask(void);
void W_Task_TimerCore(void);
void Task_Done(uint8_t u8index);
