/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_util.c
 *
 *
 ******************************************************************************/
 /*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <panic.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "app_util.h"
#include "core_mesh_handler.h"
#include "app_mesh_handler.h"
#include "debug.h"
/*============================================================================*
 *  Private Data
 *============================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/*============================================================================*
 *  Private Function Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Function Definitions
 *============================================================================*/

#ifdef DEBUG_ENABLE
/*----------------------------------------------------------------------------*
 *  NAME
 *      printInDecimal
 *
 *  DESCRIPTION
 *      This function prints an UNSIGNED integer in decimal.
 *
 *  RETURNS
 *      Nothing.
 *
 
*----------------------------------------------------------------------------*/
extern void PrintInDecimal(uint32 val)
{
    if(val >= 10)
    {
        PrintInDecimal(val/10);
    }
    DebugWriteChar(('0' + (val%10)));
}
#endif /* DEBUG_ENABLE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      ReportPanic
 *
 *  DESCRIPTION
 *      This function calls firmware panic routine and gives a single point
 *      of debugging any application level panics
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void ReportPanic(app_panic_code panic_code)
{
    /* Raise panic */
    Panic(panic_code);
}


