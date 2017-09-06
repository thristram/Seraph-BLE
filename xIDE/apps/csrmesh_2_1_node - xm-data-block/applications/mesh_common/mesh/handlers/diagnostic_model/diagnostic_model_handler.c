/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      diagnostic_model_handler.c
 *
 *  DESCRIPTION
 *      Implements handler function for diagnostic model messages
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <time.h>
#include <timer.h>
#include <mem.h>
#ifdef CSR101x
#include <ls_app_if.h>
#else
#include <ls_api.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "iot_hw.h"
#include "csr_mesh.h"
#include "diagnostic_model_handler.h"
#include "app_mesh_handler.h"
#include "core_mesh_handler.h"
#include "main_app.h"
#include "nvm_access.h"
#include "battery_hw.h"

#ifdef ENABLE_DIAGNOSTIC_MODEL
/*============================================================================*
 *  Private Definitions
 *============================================================================*/


/*============================================================================*
 *  Private Data
 *============================================================================*/
static DIAGNOSTICS_HANDLER_DATA_T*                   p_diagnostic_hdlr_data;

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
 *      diagnosticModelEventHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Diagnostic Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult diagnosticModelEventHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                             CSRMESH_EVENT_DATA_T* data,
                                             CsrUint16 length,
                                             void **state_data)
{
    switch(event_code)
    {
        case CSRMESH_DIAGNOSTIC_STATE:
        {
        }
        break;

        case CSRMESH_DIAGNOSTIC_GET_STATS:
        {
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
 *      DiagnosticModelHandlerInit
 *
 *  DESCRIPTION
 *      The Application function Initilises the Diagnostic model handler.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void DiagnosticModelHandlerInit(CsrUint8 nw_id,
                                  uint16 model_groups[],
                                  CsrUint16 num_groups)
{
    /* Initialize Diagnostic Model */
    DiagnosticModelInit(nw_id, 
                    model_groups,
                    num_groups,
                    diagnosticModelEventHandler);

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      DiagnosticModelDataInit
 *
 *  DESCRIPTION
 *      This function initialises the Diagnostic Model data on the global structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void DiagnosticModelDataInit(DIAGNOSTICS_HANDLER_DATA_T* diagnostic_handler_data)
{

    if(diagnostic_handler_data != NULL)
    {
        p_diagnostic_hdlr_data = diagnostic_handler_data;
    }
}

#endif /* ENABLE_DIAGNOSTIC_MODEL */

