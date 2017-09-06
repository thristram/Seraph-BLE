/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      action_model_handler.c
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <mem.h>
#if defined (CSR101x_A05)
#include <config_store.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "nvm_access.h"
#include "app_util.h"
#include "action_server.h"
#include "app_mesh_handler.h"
#include "action_model_handler.h"
#include "action_server.h"
#include "action_client.h"
#include "main_app.h"
#include "time_model_handler.h"
#include "buf_utils.h"

#ifdef ENABLE_ACTION_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/
#define MAX_MCP_PKT_LEN                        (11)
#define MAX_MCP_PAYLOAD                        (32)
#define PKT_ASSEMBLY_TIMER_VAL                 (5 * SECOND)
#define MINUTE_IN_SECONDS                      (60)
#define ACTION_ID_OFFSET                       (0x1F)
#define ACTION_ID_INVALID                      (0xFF)
#define ACTION_ID_MAX                          (0x20)
#define ACTION_INDEX_INVALID                   (0xFF)
#define ACTION_TYPE_MCP                        (0x00)
#define ACTION_TYPE_MCP_ACTION_GET_RSP         (0x80)

#define ACTION_REPEAT_FOREVER                  (0xFFFF)

#define ACTION_PAYLOAD_SIZE                    (8)
#define ACTION_MSG_MAX_PARTS                   (4)
#define ACTION_MSG_FULL_BITMASK                ((1 << ACTION_MSG_MAX_PARTS) - 1)

/* Macros for NVM access */
#define NVM_OFFSET_ACTION_ID                   (0)

#define NVM_OFFSET_ACTION_START_TIME           (1)
#define NVM_OFFSET_ACTION_REPEAT_TIME          (3)

#define NVM_OFFSET_ACTION_NUM_REPEAT           (5)
#define NVM_OFFSET_ACTION_MCP_TARGET_ID        (6)
#define NVM_OFFSET_ACTION_TIME_TYPE            (7)
#define NVM_OFFSET_ACTION_START_TIME_RECVD     (8)
#define NVM_OFFSET_ACTION_MCP_PKT              (10)

typedef struct
{
    uint8               action_id;
    uint8               mcp_pkt_len;
    uint32              start_time;
    uint32              repeat_time;
    uint16              curr_repeat_cnt;
    uint16              num_repeats;
    uint16              mcp_target_id;
    uint16              time_type;
    uint32              start_time_recvd;
    uint8               mcp_pkt[MAX_MCP_PKT_LEN];
}ACTION_INFO_T;

typedef struct
{
    ACTION_INFO_T       actions[MAX_ACTIONS_SUPPORTED];
    uint8               pkt_assembly[MAX_MCP_PAYLOAD];
    timer_id            pkt_assembly_tid;
    uint8               assembly_action_id;
    uint8               recvd_msg_bitmask;
    bool                last_part_recvd;
    uint8               full_msg_bitmask;
    timer_id            next_action_tid;
    uint8               next_action_index;
    uint32              next_action_exp_time;
}ACTION_HANDLER_DATA_T;

typedef union
{
    CSRMESH_ACTION_ACTION_STATUS_T          action_status;
    CSRMESH_ACTION_DELETE_ACK_T             action_del_ack;
    CSRMESH_ACTION_SET_ACTION_ACK_T         set_action_ack;
    CSRMESH_ACTION_SET_ACTION_INFO_EXT_T    set_action_ext;
} ACTION_MODEL_RSP_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/
 
/* Action handler private data */
static ACTION_HANDLER_DATA_T action_hdlr_priv_data;

/* Model Response Common Data */
static ACTION_MODEL_RSP_DATA_T action_model_rsp_data;

/*============================================================================*
 *  Private Function Prototype
 *============================================================================*/
static void setTimerForNextAction(uint8 current_utc_time[]);

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------
 *
 *  NAME
 *      getDiffTimeInSeconds
 *
 *  DESCRIPTION
 *      This function subtracts time1 with time2 in milliseconds. It then 
 *      converts the difference in seconds.
 *      NOTE: The function always expects the time1 array to be greater than 
 *      the time2 array.
 *
 *  RETURNS/MODIFIES
 *      returns the difference time in seconds
 *
 *----------------------------------------------------------------------------*/
static uint32 getDiffTimeInSeconds(uint8 time1[], uint8 time2[])
{
    uint8  index, temp1=0, time3[6];
    uint16 temp_sum=0, divisor=1000;
    bool carry = FALSE;
    uint32 quotient=0, dividend=0;

    /* The below loop subtracts the time1 from time2 and puts the value onto
     * the time3 array.
     */
    for(index = 0; index < 6; index ++)
    {
        /* Get the first byte of time1 onto temp */
        temp1 = time1[index];

        /* If a carry is present and the byte value is greater than zero, then
         * subtract the value and assign the carry to FALSE.
         */
        if(carry == TRUE && temp1 > 0)
        {
            temp1--;
            carry = FALSE;
        }
        /* If the byte in array1 is more than or equal to the byte in array2 
         * then just subtract the value otherwise add the max byte value + 1 
         * and subtract the array2 value and set the carry bit.
         */
        if(temp1 >= time2[index])
        {
            time3[index] = temp1 - time2[index];
        }
        else
        {
            temp_sum = 0x0100 + temp1;
            time3[index] = temp_sum - time2[index];
            carry = TRUE;
        }
    }

    /* The below loop divides the time3 value by divisor and gives the quotient
     * value in uint32 format.
     */
    for(index = 6; index >0; index=index-2)
    {
        /* Left shift the val in the loop once the quotient value is assigned */
        if(quotient > 0)
        {
            quotient <<=16;
        }

        /* Get the two byte values from the array and assign it as the new 
         * dividend. Make sure you append the same as there could be a remainder
         * from the previous part division.
         */
        temp1 = index-1;
        dividend |= time3[temp1--];
        dividend <<= 8;
        dividend |= time3[temp1];

        /* If the dividend is greater than the divisor then divide the same and 
         * assign the remainder back to dividend. Left shift the dividend by one
         * byte for the next byte.
         */
        if(dividend > divisor)
        {
            quotient |= dividend/divisor;
            dividend %= divisor;
        }
        dividend <<= 8;
    }
    return quotient;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      getSupportedActions
 *
 *  DESCRIPTION
 *      This function returns a bitmask containing the current supported 
 *      actions.
 *
 *  RETURNS/MODIFIES
 *      Supported actions in a bitmask format.
 *
 *----------------------------------------------------------------------------*/
static uint32 getSupportedActions(void)
{
    uint8 index;
    uint32 supp_actions_bitmask = 0;

    for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_hdlr_priv_data.actions[index].action_id < ACTION_ID_MAX)
        {
            supp_actions_bitmask |= 1 << 
                            action_hdlr_priv_data.actions[index].action_id;
        }
    }
    return supp_actions_bitmask;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      deleteActions
 *
 *  DESCRIPTION
 *      This function deletes all the actions that are received in the action
 *      bitmask.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void deleteActions(uint32 actions_bitmask)
{
    uint8 index;
    uint32 action_id_bitmask;
    uint8 current_time[6];
    int8   timezone;

    for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_hdlr_priv_data.actions[index].action_id < ACTION_ID_MAX)
        {
            action_id_bitmask = 1 << 
                            action_hdlr_priv_data.actions[index].action_id;
            if((action_id_bitmask & actions_bitmask) == action_id_bitmask)
            {
                action_hdlr_priv_data.actions[index].action_id = 
                                                            ACTION_ID_INVALID;

                /* Action id needs to be made invalid in NVM */
                Nvm_Write(
                    (uint16*) (&action_hdlr_priv_data.actions[index].action_id), 
                     sizeof(uint16),
                     GET_ACTION_NVM_OFFSET(index) + NVM_OFFSET_ACTION_ID);
            }
        }
    }

    if(TimeModelGetUTC(current_time, &timezone) == TRUE)
    {
        setTimerForNextAction(current_time);
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      getNextExpiringAction
 *
 *  DESCRIPTION
 *      This function returns the time of the next expiring action 
 *
 *  RETURNS/MODIFIES
 *      Returns the time of the next expiring action.
 *
 *----------------------------------------------------------------------------*/
static uint32 getNextExpiringAction(uint32 cur_ref_time, uint8 *action_index)
{
    uint8 index;
    uint32 next_exp_time = 0xFFFFFFFF;

    for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_hdlr_priv_data.actions[index].action_id < ACTION_ID_MAX)
        {
            /* The start time of the action has not been elapsed yet.*/
            if(action_hdlr_priv_data.actions[index].start_time > cur_ref_time)
            {
                /* If the start time is less than the next expected time assign
                 * next expiring time to the start time.
                 */
                if(action_hdlr_priv_data.actions[index].start_time < 
                                                                next_exp_time)
                {
                    next_exp_time = 
                        action_hdlr_priv_data.actions[index].start_time;
                    *action_index = index;
                }
            }
            else
            {
                uint32 diff_time=0, diff_slots=0, next_repeat_time=0;

                /* Caleculate the next repeat time and compare with the expiring
                 * time.
                 */
                if(action_hdlr_priv_data.actions[index].repeat_time != 0)
                {
                    diff_time = cur_ref_time - 
                            action_hdlr_priv_data.actions[index].start_time;

                    diff_slots = diff_time / 
                            action_hdlr_priv_data.actions[index].repeat_time;

                    diff_slots++;

                    next_repeat_time = 
                            action_hdlr_priv_data.actions[index].start_time +
                  (diff_slots)*action_hdlr_priv_data.actions[index].repeat_time;

                    /* If the repeat time is less than the next exp repeat time, 
                     * then store this value as the next exp time.
                     */
                    if(next_repeat_time < next_exp_time)
                    {
                        next_exp_time= next_repeat_time;
                        *action_index = index;
                    }
                }
            }
        }
    }
    return next_exp_time;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      getActionIdIndex
 *
 *  DESCRIPTION
 *      This function returns the index at which the action must be stored in 
 *      the action array.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static uint8 getActionIdIndex(uint8 action_id)
{
    uint8 index;

    /* check whether the action is already present if present then send the 
     * same index.
     */
    for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_id == action_hdlr_priv_data.actions[index].action_id)
        {
            return index;
        }
    }

    /* Check for an empty slot and send the lowest available action info which
     * is free.
     */
    for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_hdlr_priv_data.actions[index].action_id > ACTION_ID_MAX)
        {
            return index;
        }
    }

    return ACTION_INDEX_INVALID;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      getCurrentRefTime
 *
 *  DESCRIPTION
 *      This function returns the current reference time from 1st jan 2015 in
 *      seconds.
 *
 *  RETURNS/MODIFIES
 *      returns the current reference time.
 *
 *----------------------------------------------------------------------------*/
static uint32 getCurrentRefTime(uint8 current_utc_time[])
{
    /* Reference time 1st jan 2015 in UTC */
    uint8 ref_time[] = {0x40, 0x90, 0x9C, 0xA1, 0x4A, 0x01};

    /* When the current utc team is updated then the new curr reference time is
     * caleculated.
     */
    return getDiffTimeInSeconds(current_utc_time, ref_time);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      nextActionTimerHandler
 *
 *  DESCRIPTION
 *      The function handles the next action timer expiry.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void nextActionTimerHandler(timer_id tid)
{
    uint8 current_time[6];
    int8  timezone;
    uint8 index = action_hdlr_priv_data.next_action_index;
    bool is_time_valid = FALSE;
    uint32 curr_ref_time = 0;

    if (action_hdlr_priv_data.next_action_tid == tid)
    {
        action_hdlr_priv_data.next_action_tid = TIMER_INVALID;
        is_time_valid = TimeModelGetUTC(current_time, &timezone);
        if(is_time_valid)
        {
            curr_ref_time = getCurrentRefTime(current_time);
        }

        if((index != 0xFF) && (is_time_valid) &&
          (curr_ref_time >= action_hdlr_priv_data.next_action_exp_time))
        {
            /* Time has expired send the action here */
            ActionSendMessage(DEFAULT_NW_ID,
                action_hdlr_priv_data.actions[index].mcp_target_id, 
                AppGetCurrentTTL(),
                &action_hdlr_priv_data.actions[index].mcp_pkt[0],
                action_hdlr_priv_data.actions[index].mcp_pkt_len);

            /* If an action has limited number of repeats, then increment
             * the repeat count every time a message is sent.
             */
            if ((ACTION_REPEAT_FOREVER != action_hdlr_priv_data.actions[index].num_repeats) &&
                (0UL != action_hdlr_priv_data.actions[index].repeat_time))
            {
                action_hdlr_priv_data.actions[index].curr_repeat_cnt =
                    (curr_ref_time - action_hdlr_priv_data.actions[index].start_time)/(action_hdlr_priv_data.actions[index].repeat_time);
            }

            /* If the action is only one type then after the completion of the
             * action delete the same. If the action shouldn't be repeated 
             * forever, remove the action when it is repeated for required 
             * number of times.
             */
            if ((action_hdlr_priv_data.actions[index].repeat_time == 0) ||
                ((ACTION_REPEAT_FOREVER != action_hdlr_priv_data.actions[index].num_repeats) &&
                 (action_hdlr_priv_data.actions[index].curr_repeat_cnt > action_hdlr_priv_data.actions[index].num_repeats)))
            {
                action_hdlr_priv_data.actions[index].action_id = ACTION_ID_INVALID;

                /* Action id needs to be made invalid in NVM */
                Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].action_id),
                          sizeof(uint16), GET_ACTION_NVM_OFFSET(index) + NVM_OFFSET_ACTION_ID);
            }
        }

        if(is_time_valid)
        {
            setTimerForNextAction(current_time);
        }
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      setTimerForNextAction
 *
 *  DESCRIPTION
 *      This function should be called on a new action addition or deletion or
 *      when the system time has been updated. This function would recaleculate
 *      the timer for the next action set and set relavant timers.
 *
 *  RETURNS/MODIFIES
 *      returns the current reference time.
 *
 *----------------------------------------------------------------------------*/
static void setTimerForNextAction(uint8 current_utc_time[])
{
    uint32 timer_time, curr_ref_time;
    uint8 action_index=0;

    /* Get the current reference time based on UTC */
    curr_ref_time = getCurrentRefTime(current_utc_time);

    /* As the UTC time has changed recaleculate the timer for the next action
     * to be sent across.
     */
    action_hdlr_priv_data.next_action_exp_time = 
        getNextExpiringAction(curr_ref_time, &action_index);

    /* Delete the timers as there are no more valid actions */
    TimerDelete(action_hdlr_priv_data.next_action_tid);
    action_hdlr_priv_data.next_action_tid = TIMER_INVALID;

    if(action_hdlr_priv_data.next_action_exp_time != 0xFFFFFFFF)
    {
        uint32 diff_time = 
            action_hdlr_priv_data.next_action_exp_time - curr_ref_time;

        /* As the time is more than 30 minutes we start a timer for 30 minutes
         * and take the next action.
         */
        if(diff_time > (30 * MINUTE_IN_SECONDS))
        {
            timer_time = 30 * MINUTE;
            action_hdlr_priv_data.next_action_index = 0xFF;
        }
        else
        {
            timer_time = diff_time * SECOND;
            action_hdlr_priv_data.next_action_index = action_index;
        }
        action_hdlr_priv_data.next_action_tid = 
                        TimerCreate(timer_time, TRUE, nextActionTimerHandler);
    }
    else
    {
        action_hdlr_priv_data.next_action_index = 0xFF;
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      resetPktAssemblyInfo
 *
 *  DESCRIPTION
 *      This function resets the strctures used for packet assembly and deletes
 *      the packet assembly timer.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void resetPktAssemblyInfo(void)
{
    MemSet(&action_hdlr_priv_data.pkt_assembly, 0,
           sizeof(action_hdlr_priv_data.pkt_assembly));
    TimerDelete(action_hdlr_priv_data.pkt_assembly_tid);
    action_hdlr_priv_data.pkt_assembly_tid = TIMER_INVALID;
    action_hdlr_priv_data.assembly_action_id = ACTION_ID_INVALID;
    action_hdlr_priv_data.recvd_msg_bitmask = 0;
    action_hdlr_priv_data.last_part_recvd = FALSE;
    action_hdlr_priv_data.full_msg_bitmask = 0;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      pktAssemblyTimeoutHandler
 *
 *  DESCRIPTION
 *      This function is called when the pkt Assembly timer expires resulting in
 *      the packet not being assembled completely.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void pktAssemblyTimeoutHandler(timer_id tid)
{
    if (action_hdlr_priv_data.pkt_assembly_tid == tid)
    {
        resetPktAssemblyInfo();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      writeActionDataOnIndex
 *
 *  DESCRIPTION
 *      This function writes action data structure onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void writeActionDataOnIndex(uint8 index)
{
    uint16 index1, temp;
    uint16 nvm_act_index = GET_ACTION_NVM_OFFSET(index);

    /* Pack the mcp packet len and action id onto word to reduce space.*/
    temp = 
        action_hdlr_priv_data.actions[index].mcp_pkt_len << 8 |
        action_hdlr_priv_data.actions[index].action_id;

    Nvm_Write((uint16*) (&temp), 
              sizeof(uint16),
              nvm_act_index + NVM_OFFSET_ACTION_ID);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].start_time),
              sizeof(uint32),
              nvm_act_index + NVM_OFFSET_ACTION_START_TIME);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].repeat_time),
              sizeof(uint32),
              nvm_act_index + NVM_OFFSET_ACTION_REPEAT_TIME);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].num_repeats),
              sizeof(uint16),
              nvm_act_index + NVM_OFFSET_ACTION_NUM_REPEAT);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].mcp_target_id),
              sizeof(uint16),
              nvm_act_index + NVM_OFFSET_ACTION_MCP_TARGET_ID);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].time_type),
              sizeof(uint16),
              nvm_act_index + NVM_OFFSET_ACTION_TIME_TYPE);

    Nvm_Write((uint16*) (&action_hdlr_priv_data.actions[index].start_time_recvd),
              sizeof(uint32),
              nvm_act_index + NVM_OFFSET_ACTION_START_TIME_RECVD);

    for(index1 = 0; 
        index1 <= action_hdlr_priv_data.actions[index].mcp_pkt_len;
        index1= index1 + 2)
    {
        temp = action_hdlr_priv_data.actions[index].mcp_pkt[index1 + 1] << 8 |
               action_hdlr_priv_data.actions[index].mcp_pkt[index1];

        Nvm_Write((uint16*) (&temp),
                  sizeof(uint16),
                  nvm_act_index + NVM_OFFSET_ACTION_MCP_PKT + (index1/2));
    }
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      decodeActionPktRecvd
 *
 *  DESCRIPTION
 *      This function decodes the action packet received and stores it into the
 *      data structure.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void decodeActionPktRecvd(uint8 action_id)
{
    uint8 index, action_type, time_type;
    uint8* pkt_assembly_ptr;
    uint32 cur_ref_time=0;
    uint8 current_time[6];
    int8   timezone;

    pkt_assembly_ptr = &action_hdlr_priv_data.pkt_assembly[0];
    /* Get the index where the action info recvd needs to be stored */
    index = getActionIdIndex(action_id);
    if(index != ACTION_INDEX_INVALID)
    {
        action_type = BufReadUint8(&pkt_assembly_ptr);

        /* Decode the packet and store it onto the action structure if the 
         * action type is mcp.
         */
        if(action_type == ACTION_TYPE_MCP)
        {
            action_hdlr_priv_data.actions[index].mcp_pkt_len = 
                                            BufReadUint8(&pkt_assembly_ptr);

            action_hdlr_priv_data.actions[index].mcp_target_id = 
                                            BufReadUint16(&pkt_assembly_ptr);

            if(action_hdlr_priv_data.actions[index].mcp_pkt_len > MAX_MCP_PKT_LEN)
                action_hdlr_priv_data.actions[index].mcp_pkt_len = MAX_MCP_PKT_LEN;

            MemCopy(&action_hdlr_priv_data.actions[index].mcp_pkt[0],
                    pkt_assembly_ptr,
                    action_hdlr_priv_data.actions[index].mcp_pkt_len);

            pkt_assembly_ptr +=
                        action_hdlr_priv_data.actions[index].mcp_pkt_len;

            time_type = BufReadUint8(&pkt_assembly_ptr);
            action_hdlr_priv_data.actions[index].time_type = time_type;

            /* we need to add the current time to the start time */
            action_hdlr_priv_data.actions[index].start_time = 
                                            BufReadUint32(&pkt_assembly_ptr);

            action_hdlr_priv_data.actions[index].start_time_recvd = 
                                action_hdlr_priv_data.actions[index].start_time;
            /* In the case of time type 4 or 5, add the current time onto the 
             * start time and store the same.
             */
            if(time_type == 0x04 || time_type == 0x05)
            {
                if(TimeModelGetUTC(current_time, &timezone) == TRUE)
                {
                    cur_ref_time = getCurrentRefTime(current_time);
                    action_hdlr_priv_data.actions[index].start_time += cur_ref_time;
                }
                else
                {
                    /* If the current time is not present in the device the this
                     * relative action is not valid.
                     */
                    action_hdlr_priv_data.assembly_action_id = ACTION_ID_INVALID;
                }
            }

            if(action_hdlr_priv_data.assembly_action_id != ACTION_ID_INVALID)
            {
                action_hdlr_priv_data.actions[index].curr_repeat_cnt = 0;

                /* The repeat time is present only in the case of type 3 and 5 */
                if(time_type == 0x03 || time_type == 0x05)
                {
                    action_hdlr_priv_data.actions[index].repeat_time = BufReadUint16(&pkt_assembly_ptr);
                    action_hdlr_priv_data.actions[index].repeat_time |= (((uint32)BufReadUint8(&pkt_assembly_ptr)) << 16);
                    action_hdlr_priv_data.actions[index].num_repeats = BufReadUint16(&pkt_assembly_ptr);
                }
                else
                {
                    /* Make sure you reset this to 0 if the new action is being over
                     * written.
                     */
                    action_hdlr_priv_data.actions[index].repeat_time = 0;
                    action_hdlr_priv_data.actions[index].num_repeats = 0;
                }
                action_hdlr_priv_data.actions[index].action_id = 
                                        action_hdlr_priv_data.assembly_action_id;

                /* Copy the action information onto NVM */
                writeActionDataOnIndex(index);

                /* Set timer for next action as a new action msg is added */
                if(TimeModelGetUTC(current_time, &timezone) == TRUE)
                {
                    setTimerForNextAction(current_time);
                }
            }
        }
    }

    /* Delete the timer for action msg assembly and reset the data strctures */
    resetPktAssemblyInfo();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      actionModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Time Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult actionModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                             CSRMESH_EVENT_DATA_T* data,
                                             CsrUint16 length,
                                             void **state_data)
{
    MemSet(&action_model_rsp_data, 0x0000, sizeof(action_model_rsp_data));

    switch(event_code)
    {
        case CSRMESH_ACTION_SET_ACTION:
        {
            CSRMESH_ACTION_SET_ACTION_T *p_event = 
                                    (CSRMESH_ACTION_SET_ACTION_T *)data->data;
            uint8 action_id, part_num, last_part;

            action_model_rsp_data.set_action_ack.actionid = p_event->actionid;
            action_id = p_event->actionid & ACTION_ID_OFFSET;
            p_event->actionid >>= 5;
            last_part = p_event->actionid & 0x01;
            p_event->actionid >>= 1;
            part_num = p_event->actionid & 0x03;

            /* Start assembling only if there is space to store the actions 
             * otherwise ignore assembling until an action is deleted and rhe
             * space is available for the device.
             */
            if(getActionIdIndex(action_id) != ACTION_INDEX_INVALID)
            {
                /* check whether an action assembly is in progress or not if not 
                 * start a new assembly.
                 */
                if(action_hdlr_priv_data.pkt_assembly_tid == TIMER_INVALID)
                {
                    /* store the action id and start assembly timeout timer.*/
                    action_hdlr_priv_data.assembly_action_id = action_id;

                    action_hdlr_priv_data.pkt_assembly_tid  = 
                        TimerCreate(PKT_ASSEMBLY_TIMER_VAL, TRUE, 
                                    pktAssemblyTimeoutHandler);
                }

                if(action_id == action_hdlr_priv_data.assembly_action_id)
                {
                    if(p_event->payload_len > ACTION_PAYLOAD_SIZE)
                        p_event->payload_len = ACTION_PAYLOAD_SIZE;

                    /* copy the part and start the action pkt decode */
                    MemCopy(&action_hdlr_priv_data.pkt_assembly[part_num * 8],
                            p_event->payload,
                            p_event->payload_len);

                    /* Set the received part bitmask */
                    action_hdlr_priv_data.recvd_msg_bitmask |= (1 << part_num);

                    /* Set the last part received flag if its the last part */
                    if(last_part == 0x01)
                    {
                        action_hdlr_priv_data.last_part_recvd = TRUE;
                        part_num++;
                        action_hdlr_priv_data.full_msg_bitmask = 
                                                        ((1 << part_num) - 1);
                    }

                    /* If the received msg bitmask is same as the total msg 
                     * bitmask, then full packet is received to decode 
                     */
                    if(action_hdlr_priv_data.last_part_recvd == TRUE &&
                       ((action_hdlr_priv_data.recvd_msg_bitmask & 
                        action_hdlr_priv_data.full_msg_bitmask) == 
                                        action_hdlr_priv_data.full_msg_bitmask))
                    {
                        /* start decoding as complete packet is received */
                        decodeActionPktRecvd(action_id);
                    }
                }
            }

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&action_model_rsp_data.set_action_ack;
            }
        }
        break;

        case CSRMESH_ACTION_GET_ACTION_STATUS:
        {
            /* Updates the bitmask for the current supported actions */
            action_model_rsp_data.action_status.actionids = getSupportedActions();
            action_model_rsp_data.action_status.maxactionssupported = MAX_ACTIONS_SUPPORTED;

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&action_model_rsp_data.action_status;
            }
        }
        break;

        case CSRMESH_ACTION_DELETE:
        {
            CSRMESH_ACTION_DELETE_T *p_event = 
                            (CSRMESH_ACTION_DELETE_T *)data->data;

            /* Deletes the respective actions defined in the bitmask */
            deleteActions(p_event->actionids);

            action_model_rsp_data.action_del_ack.actionids 
                                                    = p_event->actionids;

            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&action_model_rsp_data.action_del_ack;
            }
        }
        break;

        case CSRMESH_ACTION_GET:
        {
            CSRMESH_ACTION_GET_T *p_event = 
                            (CSRMESH_ACTION_GET_T *)data->data;
            uint8 index;
            bool action_found = FALSE;

            /* check whether the action is valid and not expired */
            for(index = 0; index < MAX_ACTIONS_SUPPORTED; index++)
            {
                if(p_event->actionid == 
                                action_hdlr_priv_data.actions[index].action_id)
                {
                    action_found = TRUE;
                    break;
                }
            }

            /* Form and send the action with the action information if the 
             * action is valid and not expired.
             */
            if(action_found == TRUE)
            {
                action_model_rsp_data.set_action_ext.set_action.actionid = 
                    action_hdlr_priv_data.actions[index].action_id;

                /* MCP type says its a response for Action Get */
                action_model_rsp_data.set_action_ext.set_action.mcp_type= 
                    ACTION_TYPE_MCP_ACTION_GET_RSP;

                action_model_rsp_data.set_action_ext.set_action.actiondestid= 
                    action_hdlr_priv_data.actions[index].mcp_target_id;

                action_model_rsp_data.set_action_ext.set_action.timetype= 
                    action_hdlr_priv_data.actions[index].time_type;

                action_model_rsp_data.set_action_ext.set_action.starttime= 
                    action_hdlr_priv_data.actions[index].start_time_recvd;

                action_model_rsp_data.set_action_ext.set_action.repeattime= 
                    action_hdlr_priv_data.actions[index].repeat_time;

                action_model_rsp_data.set_action_ext.set_action.noofrepeat= 
                    action_hdlr_priv_data.actions[index].num_repeats;

                action_model_rsp_data.set_action_ext.buf_size= 
                    action_hdlr_priv_data.actions[index].mcp_pkt_len;

                MemCopy(&action_model_rsp_data.set_action_ext.buf[0],
                        &action_hdlr_priv_data.actions[index].mcp_pkt[0],
                        action_hdlr_priv_data.actions[index].mcp_pkt_len);
            }
            /* If the action is not present then do not send action response. */
            else
            {
                state_data = NULL;
            }


            /* Send response data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&action_model_rsp_data.set_action_ext;
            }
        }
        break;



        default:
        break;
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      ActionModelSyncCurrentTime
 *
 *  DESCRIPTION
 *      This function is called to sync the current time in the action model
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void ActionModelSyncCurrentTime(uint8 current_time[])
{
    uint8 index;
    uint32 curr_ref_time;

    /* Get the current reference time based on UTC */
    curr_ref_time = getCurrentRefTime(current_time);

    /* Delete all the actions which have been elapsed */
    for(index=0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        if(action_hdlr_priv_data.actions[index].action_id != ACTION_ID_INVALID
           && action_hdlr_priv_data.actions[index].repeat_time == 0 
           && curr_ref_time > action_hdlr_priv_data.actions[index].start_time)
        {
            action_hdlr_priv_data.actions[index].action_id = 
                                                        ACTION_ID_INVALID;

            /* Action id needs to be made invalid in NVM */
            Nvm_Write(
                (uint16*) (&action_hdlr_priv_data.actions[index].action_id), 
                sizeof(uint16),
                GET_ACTION_NVM_OFFSET(index) + NVM_OFFSET_ACTION_ID);
        }
    }

    setTimerForNextAction(current_time);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReadActionModelDataFromNVM
 *
 *  DESCRIPTION
 *      This function reads action model data from NVM into state variable.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void ReadActionModelDataFromNVM(uint16 offset)
{
    uint8 index, index1;
    uint16 temp;
    for(index=0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        uint16 nvm_act_index = GET_ACTION_NVM_OFFSET(index);

        /* Read action id and mcp packet len from NVM */
        Nvm_Read((uint16 *)&temp, sizeof(uint16), 
                 nvm_act_index + NVM_OFFSET_ACTION_ID);

        action_hdlr_priv_data.actions[index].action_id = temp & 0xFF;
        temp >>= 8;

        if(action_hdlr_priv_data.actions[index].action_id < ACTION_ID_MAX)
        {
            action_hdlr_priv_data.actions[index].mcp_pkt_len = temp & 0xFF;
            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].start_time,
                      sizeof(uint32),
                      nvm_act_index + NVM_OFFSET_ACTION_START_TIME);

            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].repeat_time,
                      sizeof(uint32),
                      nvm_act_index + NVM_OFFSET_ACTION_REPEAT_TIME);

            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].num_repeats,
                      sizeof(uint16),
                      nvm_act_index + NVM_OFFSET_ACTION_NUM_REPEAT);

            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].mcp_target_id,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_ACTION_MCP_TARGET_ID);

            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].time_type,
                     sizeof(uint16), 
                     nvm_act_index + NVM_OFFSET_ACTION_TIME_TYPE);

            Nvm_Read(
                (uint16 *)&action_hdlr_priv_data.actions[index].start_time_recvd,
                      sizeof(uint32),
                      nvm_act_index + NVM_OFFSET_ACTION_START_TIME_RECVD);


            for(index1=0; 
                index1 < action_hdlr_priv_data.actions[index].mcp_pkt_len;
                index1 = index1+2)
            {
                /* Read action id and mcp packet len from NVM */
                Nvm_Read((uint16 *)&temp,
                        sizeof(uint16), 
                        nvm_act_index + NVM_OFFSET_ACTION_MCP_PKT + (index1/2));
                action_hdlr_priv_data.actions[index].mcp_pkt[index1] = 
                                                                temp & 0xFF;
                temp >>=8;
                action_hdlr_priv_data.actions[index].mcp_pkt[index1+1] = 
                                                                temp & 0xFF;
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      WriteActionModelDataOntoNVM
 *
 *  DESCRIPTION
 *      This function writes action model data onto NVM.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void WriteActionModelDataOntoNVM(uint16 offset)
{
    uint8 index;
    for(index=0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        writeActionDataOnIndex(index);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ActionModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Action model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ActionModelHandlerInit(uint8 nw_id,
                                   uint16 model_groups[],
                                   CsrUint16 num_groups)
{
    /* Initialize Action Model */
    ActionModelInit(nw_id, 
                    model_groups,
                    num_groups,
                    actionModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ActionModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Action Model data on the global 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ActionModelDataInit(void)
{
    uint8 index;

    MemSet(&action_hdlr_priv_data, 0, 
            sizeof(action_hdlr_priv_data));

    TimerDelete(action_hdlr_priv_data.pkt_assembly_tid);
    action_hdlr_priv_data.pkt_assembly_tid = TIMER_INVALID;

    TimerDelete(action_hdlr_priv_data.next_action_tid);
    action_hdlr_priv_data.next_action_tid = TIMER_INVALID;

    for(index=0; index < MAX_ACTIONS_SUPPORTED; index++)
    {
        action_hdlr_priv_data.actions[index].action_id = ACTION_ID_INVALID;
    }
}
#endif /* ENABLE_ACTION_MODEL */

