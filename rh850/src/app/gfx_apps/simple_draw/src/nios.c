#include "nios.h"

uint8_t gu8undo_num = 0;
queue_task_def gtqueue_manager;
task_recorder_def gttask_recorder[_TASK_NUM];

//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Assigner
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_AssignedPriority(task_recorder_def* precorder,uint8_t u8undo_num)
{
    uint8_t u8row = 0;
    uint8_t u8counter = 0;
    task_recorder_def trecorder_temp;
    
    if(u8undo_num != 0)
    {
        for(u8row = 0; u8row < u8undo_num; u8row++)
        {
            for(u8counter = (u8row + 1); u8counter < gu8undo_num; u8counter++)
            {
                if((precorder[u8row].ttask[precorder[u8row].u8current_point].u8level + precorder[u8row].u8discard_time) < 
                   (precorder[u8counter].ttask[precorder[u8counter].u8current_point].u8level + precorder[u8counter].u8discard_time))
                {
                    trecorder_temp = precorder[u8row]; 
                    precorder[u8row] = precorder[u8counter]; 
                    precorder[u8counter] = trecorder_temp; 
                }
            }
        }
    }
    for(u8counter = 0; u8counter < _TASK_NUM; u8counter++)
    {
        if(precorder[u8counter].u8task_num != 0)
            precorder[u8counter].u8discard_time++;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
uint8_t W_Task_ExcludeDone(task_recorder_def* precorder)
{
    uint8_t u8exclue_num = 0;
    uint8_t u8row = 0;
    uint8_t u8counter = 0;
    task_recorder_def trecorder_temp;

    for(u8row = 0; u8row < _TASK_NUM - 1; u8row++)
    {
        for(u8counter = (u8row + 1); u8counter < _TASK_NUM; u8counter++)
        {
            if(precorder[u8row].u8task_num == 0)
            {
                if(precorder[u8counter].u8task_num != 0)
                {
                    trecorder_temp = precorder[u8row]; 
                    precorder[u8row] = precorder[u8counter]; 
                    precorder[u8counter] = trecorder_temp; 
                }
            }
        }
    }
    for(u8row = 0; u8row < _TASK_NUM; u8row++)
    {
        if(precorder[u8row].u8task_num == 0)
            u8exclue_num++;
    }
    return u8exclue_num;
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_AssignedTask(task_recorder_def* precorder)
{
    uint8_t u8exclue_num = 0;
    
    u8exclue_num = W_Task_ExcludeDone(precorder);
    gu8undo_num = _TASK_NUM - u8exclue_num;
    W_Task_AssignedPriority(precorder,gu8undo_num);
}
//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Recorder
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_QueueManagerInit(queue_task_def* pqueue_manager)
{
    uint8_t u8counter = 0;
    pqueue_manager->u8current_point = 0;
    pqueue_manager->u8end_point = 0;
    
    for(u8counter = 0; u8counter < _TASK_QUEUE_SIZE; u8counter++)
    {
        W_Task_ClearTaskQueue(&pqueue_manager->ttask[u8counter]);
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_RecorderInit(task_recorder_def* precorder)
{
    uint8_t u8task_counter = 0;
    uint8_t u8queue_counter = 0;
    
    for(u8task_counter = 0; u8task_counter < _TASK_NUM; u8task_counter++)
    {
        precorder[u8task_counter].u8discard_time = 0;
        precorder[u8task_counter].u8current_point = 0;
        precorder[u8task_counter].u8end_point = 0;
        precorder[u8task_counter].u8task_num = 0;
        
        for(u8queue_counter = 0; u8queue_counter < _RECORDER_QUEUE_SIZE; u8queue_counter++)
        {
            precorder[u8task_counter].ttask[u8queue_counter].u8type = _TYPE_NONE;
            precorder[u8task_counter].ttask[u8queue_counter].u8level = _LEVEL_NONE;
            precorder[u8task_counter].ttask[u8queue_counter].u8event = _EVENT_NONE;
            precorder[u8task_counter].ttask[u8queue_counter].u8state = _STATE_NONE;
            precorder[u8task_counter].ttask[u8queue_counter].ttask_handler = NULL;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_TaskInRecorder(void (*call_fun)(void),uint8_t u8type, uint8_t u8task_counter)
{
    gttask_recorder[u8task_counter].u8end_point =  (gttask_recorder[u8task_counter].u8end_point + 1) % _RECORDER_QUEUE_SIZE;
    gttask_recorder[u8task_counter].u8task_num++;
    gttask_recorder[u8task_counter].ttask[0].ttask_handler = call_fun;
    gttask_recorder[u8task_counter].ttask[0].u8type = u8type;
    gttask_recorder[u8task_counter].ttask[0].u8state = _STATE_INIT;
    gttask_recorder[u8task_counter].ttask[0].u8event = _EVENT_FIRST;
    gttask_recorder[u8task_counter].ttask[0].u8level = _LEVEL5;
    gttask_recorder[u8task_counter].u8discard_time = 0;
    gttask_recorder[u8task_counter].u8pre_type = u8type;
}
//-------------------------------------------------------------------------------------------------------------------------
uint8_t W_Task_ChangeEvent(uint8_t u8type, uint8_t u8level, uint8_t u8event)
{
    uint8_t u8counter = 0;
    for(u8counter = 0; u8counter < _TASK_NUM; u8counter++)
    {
        if(gttask_recorder[u8counter].u8pre_type == u8type)
        {
            if(gttask_recorder[u8counter].u8task_num < _RECORDER_QUEUE_SIZE)
            {
                gttask_recorder[u8counter].u8discard_time = 0;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8type = u8type;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].ttask_handler = gttask_recorder[u8counter].tpre_task_handler;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8state = gttask_recorder[u8counter].u8pre_state;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8level = u8level;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8event = u8event;
                gttask_recorder[u8counter].u8end_point = (gttask_recorder[u8counter].u8end_point + 1) % _RECORDER_QUEUE_SIZE;
                gttask_recorder[u8counter].u8task_num++;
                gttask_recorder[u8counter].u8queue_full = _FALSE;
                return _TRUE;
            }
            gttask_recorder[u8counter].u8queue_full = _TRUE;
        }
    }
    return _FALSE;
} 
//-------------------------------------------------------------------------------------------------------------------------
uint8_t W_Task_ChangeState(uint8_t u8type,uint8_t u8level, uint8_t u8state, void (*call_fun)(void))
{
    uint8_t u8counter = 0;

    for(u8counter = 0; u8counter < _TASK_NUM; u8counter++)
    {
        if(gttask_recorder[u8counter].u8pre_type == u8type)
        {
            if(gttask_recorder[u8counter].u8task_num < _RECORDER_QUEUE_SIZE)
            {
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8type = u8type;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].ttask_handler = call_fun;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8state = u8state;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8level = _LEVEL5;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8event = _EVENT_FIRST;
                gttask_recorder[u8counter].u8discard_time = 0;
                gttask_recorder[u8counter].u8end_point = (gttask_recorder[u8counter].u8end_point + 1) % _RECORDER_QUEUE_SIZE;
                gttask_recorder[u8counter].u8task_num++;
                gttask_recorder[u8counter].u8queue_full = _FALSE;
                return _TRUE;
            }
            gttask_recorder[u8counter].u8queue_full = _TRUE;
        }
    }
    return _FALSE;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Excuter
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_PushTaskInQueue(queue_task_def* pqueue_manager, task_recorder_def* precorder, uint8_t u8undo_num)
{
    uint8_t u8counter;
    uint8_t u8size = 0;
    
    u8size = (_TASK_QUEUE_SIZE > u8undo_num) ? u8undo_num : _TASK_QUEUE_SIZE;
    
    gtqueue_manager.u8current_point = 0;
    gtqueue_manager.u8end_point = 0;
    
    for(u8counter = 0; u8counter < u8size; u8counter++)
    {        
        pqueue_manager->ttask[pqueue_manager->u8end_point] = precorder[u8counter].ttask[precorder[u8counter].u8current_point];
        pqueue_manager->u8end_point += 1;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_ClearTaskQueue(nios_task_def *pttask)
{
    pttask->u8type = _TYPE_NONE;
    pttask->u8state = _STATE_NONE;
    pttask->u8event = _EVENT_NONE;
    pttask->u8level = _LEVEL_NONE;
    pttask->ttask_handler = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_ClearRecorderQueue(nios_task_def *pttask)
{
    pttask->u8state = _STATE_NONE;
    pttask->u8event = _EVENT_NONE;
    pttask->u8level = _LEVEL_NONE;
    pttask->ttask_handler = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_Done(uint8_t u8index)
{
    gttask_recorder[u8index].u8pre_type = gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point].u8type;
    gttask_recorder[u8index].u8pre_state = gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point].u8state;
    gttask_recorder[u8index].u8pre_event = gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point].u8event;
    gttask_recorder[u8index].u8pre_level = gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point].u8level;
    gttask_recorder[u8index].tpre_task_handler = gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point].ttask_handler;
    W_Task_ClearRecorderQueue(&gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point]);
    gttask_recorder[u8index].u8current_point = (gttask_recorder[u8index].u8current_point + 1) % _RECORDER_QUEUE_SIZE;
    
    if(gttask_recorder[u8index].u8task_num > 0)
        gttask_recorder[u8index].u8task_num--;
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_PullTask(void)
{
    W_Task_ClearTaskQueue(&gtqueue_manager.ttask[gtqueue_manager.u8current_point]);
    gtqueue_manager.u8current_point++;
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_ExcuteTask(void)
{
    gtqueue_manager.ttask[gtqueue_manager.u8current_point].ttask_handler();
    W_Task_PullTask();
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Task_ProcessTask(void)
{
    if(gtqueue_manager.u8current_point < gtqueue_manager.u8end_point)
    {
        W_Task_ExcuteTask();
    }
    else
    {
        // The queue is empty and need to push new task into queue.
        W_Task_AssignedTask(gttask_recorder);
        W_Task_PushTaskInQueue(&gtqueue_manager,gttask_recorder,gu8undo_num);
    }
}

