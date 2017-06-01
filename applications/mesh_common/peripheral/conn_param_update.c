/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      conn_param_update.c
 *
 *  DESCRIPTION
 *      Implements routines for the connection parameter update procedure
 *
 ******************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <mem.h>
#include <timer.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "conn_param_update.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* TGAP(conn_pause_peripheral) defined in Core Specification Addendum 3 Revision
 * 2. A Peripheral device should not perform a Connection Parameter Update proc-
 * -edure within TGAP(conn_pause_peripheral) after establishing a connection.
 */
#define TGAP_CPP_PERIOD                         (5 * SECOND)

/* TGAP(conn_pause_central) defined in Core Specification Addendum 3 Revision 2.
 * After the Peripheral device has no further pending actions to perform and the
 * Central device has not initiated any other actions within TGAP(conn_pause_ce-
 * -ntral), then the Peripheral device may perform a Connection Parameter Update
 * procedure.
 */
#define TGAP_CPC_PERIOD                         (1 * SECOND)

/* Slave device is not allowed to transmit another Connection Parameter
 * Update request till time TGAP(conn_param_timeout). Refer to section 9.3.9.2,
 * Vol 3, Part C of the Core 4.0 BT spec. The application should retry the
 * 'connection paramter update' procedure after time TGAP(conn_param_timeout)
 * which is 30 seconds.
 */
#define GAP_CONN_PARAM_TIMEOUT                  (30 * SECOND)

/* Maximum retry attempt */
#define MAX_RETRY                               (3)

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

typedef struct
{
    /* Connection parameter update Timer id */
    timer_id                        tid;

    /* Device Id */
    device_handle_id                device_id;

    /* New connection parameters set 1 */
    ble_con_params                  new_params_1;

    /* New connection parameters set 1 */
    ble_con_params                  new_params_2;

    /* Updated connection parameters */
    CONN_PARAMS_T                   conn_params;

    /* Connection Pause Central timer active flag */
    bool                            cpc_timer_active;

    /* Retries remaining for the current set of connection parameters */
    uint16                          retries_left;

    /* Two sets of connection parameters */
    bool                            two_sets;

    /* Current set of parameters */
    uint16                          cur_set;

    /* Number of reties to attempt for each connection parameter set */
    uint16                          num_retries;

    /* Callback to tell application whether connection params were applied */
    conn_param_update_complete      update_complete_callback;

} CONN_UPDATE_PARAM_DATA_T;

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function handles the expiry of TGAP(conn_pause_peripheral) timer */
static void handleGapCppTimerExpiry(timer_id tid);

/* Handles connection params update request */
static void requestConnParamUpdate(timer_id tid);

/* This function handles the signal CM_CONNECTION_UPDATE_SIGNALLING_IND */
static void handleConnUpdateSignallingInd(
        CM_CONNECTION_UPDATE_SIGNALLING_IND_T *cm_event_data);

/* This function handles the signal CM_CONNECTION_PARAM_UPDATE_CFM */
static void handleConnParamUpdateCfm(
        CM_CONNECTION_PARAM_UPDATE_CFM_T *cm_event_data);

/* This function handles the signal CM_CONNECTION_PARAM_UPDATE_IND */
static void handleConnParamUpdateInd(
        CM_CONNECTION_PARAM_UPDATE_IND_T *cm_event_data);

/* This function handles the connection updated event */
static void handleCmConnectionUpdated(CM_CONNECTION_UPDATED_T* cm_event_data);

/* This function handles the signal CM_SERVER_ACCESSED */
static void handleServerAccessed(CM_SERVER_ACCESSED_T *cm_event_data);

/* Stores new connection parameters and works out which set to use */
static bool processNewParams(device_handle_id device_id,
                             ble_con_params *new_params_1,
                             ble_con_params *new_params_2);

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Connection parameter update procedure data */
static CONN_UPDATE_PARAM_DATA_T local_data =
{
    .update_complete_callback = NULL
};

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleGapCppTimerExpiry
 *
 *  DESCRIPTION
 *      This function handles the expiry of TGAP(conn_pause_peripheral) timer
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void handleGapCppTimerExpiry(timer_id tid)
{
    local_data.tid = TimerCreate(TGAP_CPC_PERIOD, TRUE,
                                   requestConnParamUpdate);
    local_data.cpc_timer_active = TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      requestConnParamUpdate
 *
 *  DESCRIPTION
 *      Handles connection parameter update request
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void requestConnParamUpdate(timer_id tid)
{
    local_data.tid = TIMER_INVALID;
    local_data.cpc_timer_active = FALSE;

    if(local_data.cur_set == 1)
    {
        CMConnParamUpdateReq(local_data.device_id,
                             &local_data.new_params_1);
    }
    else
    {
        CMConnParamUpdateReq(local_data.device_id,
                             &local_data.new_params_2);
    }

    if(local_data.retries_left > 0)
    {
        local_data.retries_left--;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnUpdateSignallingInd
 *
 *  DESCRIPTION
 *      This function handles the signal CM_CONNECTION_UPDATE_SIGNALLING_IND
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleConnUpdateSignallingInd(
                        CM_CONNECTION_UPDATE_SIGNALLING_IND_T *cm_event_data)
{
    CMConnectionUpdateSignalingRsp(cm_event_data->device_id,
                                   cm_event_data->sig_identifier,
                                   TRUE);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnParamUpdateCfm
 *
 *  DESCRIPTION
 *      This function handles the signal CM_CONNECTION_PARAM_UPDATE_CFM
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleConnParamUpdateCfm(
        CM_CONNECTION_PARAM_UPDATE_CFM_T *cm_event_data)
{
    if(cm_event_data->status != sys_status_success)
    {
        if(local_data.retries_left > 0)
        {
            /* Retry connection parameter update */
            local_data.tid =
                    TimerCreate(GAP_CONN_PARAM_TIMEOUT, TRUE,
                                handleGapCppTimerExpiry);
        }
        else if(local_data.two_sets)
        {
            /* Reset the flag it is the last set */
            local_data.two_sets     = FALSE;

            /* Initialise for the second set of parameters */
            local_data.cur_set      = 2;
            local_data.retries_left = local_data.num_retries;

            /* Retry connection parameter update */
            local_data.tid =
                    TimerCreate(GAP_CONN_PARAM_TIMEOUT, TRUE,
                                handleGapCppTimerExpiry);
        }
        else
        {
            local_data.retries_left = MAX_RETRY;
            
            /* Callback application */
            if (local_data.update_complete_callback != NULL)
            {
                local_data.update_complete_callback(FALSE);
            }
        }
    }
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      handleConnParamUpdateInd
 *
 *  DESCRIPTION
 *      This function handles the signal CM_CONNECTION_PARAM_UPDATE_IND
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void handleConnParamUpdateInd(
        CM_CONNECTION_PARAM_UPDATE_IND_T *cm_event_data)
{
    bool request_again = FALSE;

    if(CMGetPeerDeviceRole(cm_event_data->device_id) != con_role_central)
        return;

    if(local_data.tid == TIMER_INVALID)
    {
        if(local_data.cur_set == 1)
        {
            if(cm_event_data->conn_interval > local_data.
                                            new_params_1.con_max_interval)
            {
                request_again = TRUE;
            }
        }
        else
        {
            if(cm_event_data->conn_interval > local_data.
                                            new_params_2.con_max_interval)
            {
                request_again = TRUE;
            }
        }

        if(request_again)
        {
            requestConnParamUpdate(TIMER_INVALID);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleCmConnectionUpdated
 *
 *  DESCRIPTION
 *      This function handles the connection updated event
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleCmConnectionUpdated(CM_CONNECTION_UPDATED_T* cm_event_data)
{
    if (cm_event_data->conn_interval > 0)
    {
        /* Store the new connection parameters. */
        local_data.conn_params.conn_interval = cm_event_data->conn_interval;
        local_data.conn_params.conn_latency  = cm_event_data->conn_latency;
        local_data.conn_params.conn_timeout  = cm_event_data->supervision_timeout;
        
        /* Callback application */
        if (local_data.update_complete_callback != NULL)
        {
            local_data.update_complete_callback(TRUE);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleServerAccessed
 *
 *  DESCRIPTION
 *      Handles server read/write access
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleServerAccessed(CM_SERVER_ACCESSED_T *cm_event_data)
{
    /* CM_SERVER_ACCESSED indicates that the central device is still disco-
     * -vering services. So, restart the connection parameter update
     * timer
     */
     if(local_data.cpc_timer_active == TRUE
            && local_data.tid != TIMER_INVALID)
     {
        TimerDelete(local_data.tid);
        local_data.tid = TimerCreate(TGAP_CPC_PERIOD,
                                         TRUE, requestConnParamUpdate);
     }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      processNewParams
 *
 *  DESCRIPTION
 *      Stores new connection parameters and works out which set to use.
 *
 *  RETURNS
 *      TRUE if successful, false otherwise.
 *
 *---------------------------------------------------------------------------*/
static bool processNewParams(device_handle_id device_id,
                             ble_con_params *new_params_1,
                             ble_con_params *new_params_2)
{
    if(new_params_1 == NULL && new_params_2 == NULL)
        return FALSE;

    local_data.device_id        = device_id;
    if(new_params_1 != NULL)
    {
        local_data.new_params_1     = *new_params_1;
        local_data.cur_set      = 1;
    }
    local_data.two_sets         = FALSE;
    if(new_params_2 != NULL)
    {
        local_data.new_params_2 = *new_params_2;
        if(new_params_1 != NULL)
        {
            local_data.two_sets     = TRUE;
        }
        else
        {
            local_data.cur_set      = 2;
        }
    }

    MemSet(&local_data.conn_params, 0, sizeof(local_data.conn_params));

    return TRUE;
}

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      RequestConnParamsUpdate
 *
 *  DESCRIPTION
 *      Starts the Connection parameter update procedure.
 *      This function will wait for the appropriate time to request a connection
 *      parameter update and then send it. It will retry sending the first set
 *      of parameters a number of times before falling back to the second set.
 *      It may be called at any time during a connection, but may take some time
 *      to complete.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void RequestConnParamsUpdate(device_handle_id device_id,
                                    ble_con_params *new_params_1,
                                    ble_con_params *new_params_2)
{
    
    if (local_data.tid != TIMER_INVALID)
    {
        TimerDelete(local_data.tid);
    }
    
    if(!processNewParams(device_id, new_params_1, new_params_2))
        return;

    local_data.num_retries      = MAX_RETRY;
    local_data.retries_left     = local_data.num_retries;
    local_data.cpc_timer_active = FALSE;

    /* The application first starts a timer of
     * TGAP_CPP_PERIOD. During this time, the application
     * waits for the peer device to do the database
     * discovery procedure. After expiry of this timer, the
     * application starts one more timer of period
     * TGAP_CPC_PERIOD. If the application receives any
     * GATT_ACCESS_IND during this time, it assumes that
     * the peer device is still doing device database
     * discovery procedure or some other configuration and
     * it should not update the parameters, so it restarts
     * the TGAP_CPC_PERIOD timer. If this timer expires, the
     * application assumes that database discovery procedure
     * is complete and it initiates the connection parameter
     * update procedure.
     * Please note that this procedure requires all the
     * reads/writes to be made IRQ. If application wants
     * firmware to reply for some of the request, it shall
     * reply with "gatt_status_irq_proceed.
     */
    local_data.tid = TimerCreate(TGAP_CPP_PERIOD, TRUE,
                                             handleGapCppTimerExpiry);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      RequestConnParamsUpdateOnce
 *
 *  DESCRIPTION
 *      This function requests a connection parameter update only once for each
 *      set of connection parameters given. It will send the request as soon as
 *      possible, so should not be called until well after a connection has
 *      been established. It can be used when a change in connection parameters
 *      is required midway through a connection, for example, to temporarily
 *      reduce the connection interval before some data transfer. It will still
 *      wait for any GATT activity to finish before sending the request, but
 *      aims to complete as quickly as possible (hence only attempting each set
 *      once).
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void RequestConnParamsUpdateOnce(device_handle_id device_id,
                                        ble_con_params *new_params_1,
                                        ble_con_params *new_params_2)
{
    if (local_data.tid != TIMER_INVALID)
    {
        TimerDelete(local_data.tid);
    }
    
    if(!processNewParams(device_id, new_params_1, new_params_2))
        return;

    local_data.num_retries      = 1;
    local_data.retries_left     = local_data.num_retries;
    local_data.cpc_timer_active = TRUE;

    /* The application first starts a timer of
     * TGAP_CPC_PERIOD. If the application receives any
     * GATT_ACCESS_IND during this time, it assumes that
     * the peer device is still doing device database
     * discovery procedure or some other configuration and
     * it should not update the parameters, so it restarts
     * the TGAP_CPC_PERIOD timer. If this timer expires, the
     * application assumes that database discovery procedure
     * is complete and it initiates the connection parameter
     * update procedure.
     * Please note that this procedure requires all the
     * reads/writes to be made IRQ. If application wants
     * firmware to reply for some of the request, it shall
     * reply with "gatt_status_irq_proceed.
     */
    local_data.tid = TimerCreate(TGAP_CPC_PERIOD, TRUE, requestConnParamUpdate);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      StopConnParamsUpdate
 *
 *  DESCRIPTION
 *      Stops connection parameter update procedure
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void StopConnParamsUpdate(void)
{
    /* Kill the button press timer id. */
    TimerDelete(local_data.tid);
    local_data.tid  = TIMER_INVALID;
    local_data.device_id        = CM_INVALID_DEVICE_ID;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetUpdatedConnParams
 *
 *  DESCRIPTION
 *      Gets the updated connection parameters
 *
 *  RETURNS
 *      TRUE if the parameter got updated, FALSE otherwise
 *
 *---------------------------------------------------------------------------*/
extern bool GetUpdatedConnParams(CONN_PARAMS_T *conn_params)
{
    bool result = FALSE;

    if(local_data.conn_params.conn_interval > 0)
    {
        *conn_params = local_data.conn_params;
        result = TRUE;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnectionParamUpdateEvent
 *
 *  DESCRIPTION
 *      This function handles the events related to connection parameter update
 *      procedure
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/

extern void ConnectionParamUpdateEvent(cm_event event_type,
                                       CM_EVENT_T *cm_event_data)
{
    switch(event_type)
    {
        case CM_CONNECTION_UPDATE_SIGNALLING_IND:
        {
            handleConnUpdateSignallingInd(
                    (CM_CONNECTION_UPDATE_SIGNALLING_IND_T*)cm_event_data);
        }
        break;

        case CM_CONNECTION_PARAM_UPDATE_CFM:
        {
            handleConnParamUpdateCfm(
                    (CM_CONNECTION_PARAM_UPDATE_CFM_T*)cm_event_data);
        }
        break;

        case CM_CONNECTION_PARAM_UPDATE_IND:
        {
            handleConnParamUpdateInd(
                    (CM_CONNECTION_PARAM_UPDATE_IND_T*)cm_event_data);
        }
        break;

        case CM_CONNECTION_UPDATED:
        {
            handleCmConnectionUpdated(
                    (CM_CONNECTION_UPDATED_T*)cm_event_data);
        }
        break;

        case CM_SERVER_ACCESSED:
        {
            handleServerAccessed((CM_SERVER_ACCESSED_T*)cm_event_data);
        }
        break;

        default:
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnectionParamUpdateSetCallback
 *
 *  DESCRIPTION
 *      This function allows an application callback function to be set, which
 *      will be called when a connection parameter update request completes.
 *      This could either be because the host accepted the new parameters, or
 *      because the maximum number of rejections was reached (and so the new
 *      parameters have not been applied)
 *
 *  RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void ConnectionParamUpdateSetCallback(conn_param_update_complete function)
{
    local_data.update_complete_callback = function;
}
