/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      battery_model_handler.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "battery_hw.h"
#include "battery_model_handler.h"
#include "battery_server.h"
#include "user_config.h"

#ifdef ENABLE_BATTERY_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/*============================================================================*
 *  Private Data
 *============================================================================*/
/* Pointer to model handler Data */
static BATTERY_HANDLER_DATA_T* p_batt_model_hdlr_data;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      batteryModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Battery Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult batteryModelEventHandler(CSRMESH_MODEL_EVENT_T event_code,
                                              CSRMESH_EVENT_DATA_T* data,
                                              CsrUint16 length,
                                              void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_BATTERY_GET_STATE:
        {
            CSRMESH_BATTERY_GET_STATE_T *p_get_state = 
                                  (CSRMESH_BATTERY_GET_STATE_T *)data->data;

            /* Read Battery Level */
            p_batt_model_hdlr_data->battery_model.batterylevel 
                                                        = ReadBatteryLevel();
            p_batt_model_hdlr_data->battery_model.batterystate
                                                        = GetBatteryState();
            p_batt_model_hdlr_data->battery_model.tid   = p_get_state->tid;

            /* Pass Battery state data to model */
            if (state_data != NULL)
            {
                *state_data = (void *)&p_batt_model_hdlr_data->battery_model;
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
 *      BatteryModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Battery model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BatteryModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Battery Model */
    BatteryModelInit(nw_id, 
                     model_groups,
                     num_groups,
                     batteryModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BatteryModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Battery Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void BatteryModelDataInit(BATTERY_HANDLER_DATA_T* p_handler_data)
{
    if(p_handler_data != NULL)
    {
        p_batt_model_hdlr_data = p_handler_data;
    }
}

#endif /* ENABLE_BATTERY_MODEL */

