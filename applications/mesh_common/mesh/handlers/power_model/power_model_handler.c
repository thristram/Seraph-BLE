/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      power_model_handler.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "iot_hw.h"
#include "power_model_handler.h"
#include "app_mesh_handler.h"
#include "power_server.h"
#ifdef ENABLE_LIGHT_MODEL
#include "light_model_handler.h"
#endif

#ifdef ENABLE_POWER_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/*============================================================================*
 *  Private Data
 *============================================================================*/
static POWER_HANDLER_DATA_T* p_power_hdlr_data;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      togglePowerState
 *
 *  DESCRIPTION
 *      This function toggles the power state.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void togglePowerState(void)
{
    csr_mesh_power_state_t curr_state = p_power_hdlr_data->power_model.state;

    switch (curr_state)
    {
        case csr_mesh_power_state_on:
            p_power_hdlr_data->power_model.state = csr_mesh_power_state_off;
        break;

        case csr_mesh_power_state_off:
            p_power_hdlr_data->power_model.state = csr_mesh_power_state_on;
        break;

        case csr_mesh_power_state_onfromstandby:
            p_power_hdlr_data->power_model.state = csr_mesh_power_state_standby;
        break;

        case csr_mesh_power_state_standby:
            p_power_hdlr_data->power_model.state 
                                        = csr_mesh_power_state_onfromstandby;
        break;

        default:
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      powerModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Power Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult powerModelEventHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                            CSRMESH_EVENT_DATA_T* data,
                                            CsrUint16 length,
                                            void **state_data)
{
    bool state_changed = FALSE;

    switch(event_code)
    {
        case CSRMESH_POWER_SET_STATE_NO_ACK:
        case CSRMESH_POWER_SET_STATE:
        {
            CSRMESH_POWER_SET_STATE_T *p_event = 
                 (CSRMESH_POWER_SET_STATE_T *)
                                    (((CSRMESH_EVENT_DATA_T *)data)->data);
            if(p_power_hdlr_data->power_model.state != p_event->state)
            {
                p_power_hdlr_data->power_model.state = p_event->state;
                state_changed = TRUE;
            }
        }
        break;

        case CSRMESH_POWER_TOGGLE_STATE_NO_ACK:
        case CSRMESH_POWER_TOGGLE_STATE:
        {
            togglePowerState();
            state_changed = TRUE;
        }
        break;

        default:
        break;
    }

    if (TRUE == state_changed)
    {
#ifdef ENABLE_LIGHT_MODEL
        /* Update the power state in the light model as well */
        LightUpdatePowerState(p_power_hdlr_data->power_model.state);
#endif
        /* Set the light state accordingly */
        if (p_power_hdlr_data->power_model.state == 
                                            csr_mesh_power_state_off ||
            p_power_hdlr_data->power_model.state == 
                                            csr_mesh_power_state_standby)
        {
            IOTLightControlDevicePower(FALSE);
        }
        else if(p_power_hdlr_data->power_model.state == 
                                            csr_mesh_power_state_on||
                p_power_hdlr_data->power_model.state == 
                                        csr_mesh_power_state_onfromstandby)
        {
            /* Turn on with stored colour */
            RestoreLightState();
        }
    }

    /* Return updated power state to the model */
    if (state_data != NULL)
    {
        *state_data = (void *)&p_power_hdlr_data->power_model;
    }

    return CSR_MESH_RESULT_SUCCESS;
}

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      PowerModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Power model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void PowerModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Power Model */
    PowerModelInit(nw_id, 
                   model_groups,
                   num_groups,
                   powerModelEventHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      PowerModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Power Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void PowerModelDataInit(POWER_HANDLER_DATA_T* power_handler_data)
{
    if(power_handler_data != NULL)
    {
        p_power_hdlr_data = power_handler_data;
    }
    PowerUpdatePowerState(csr_mesh_power_state_off);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      PowerUpdatePowerState
 *
 *  DESCRIPTION
 *      This function can be called to update the Power models power state.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void PowerUpdatePowerState(csr_mesh_power_state_t pwr_state)
{
    p_power_hdlr_data->power_model.state = pwr_state;
}
#endif /* ENABLE_POWER_MODEL */

