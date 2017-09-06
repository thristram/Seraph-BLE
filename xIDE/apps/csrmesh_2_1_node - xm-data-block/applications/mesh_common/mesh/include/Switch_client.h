/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file Switch_client.h
 * 
 *  \brief This files provides the prototypes of the client functions defined
 *         in the CSRmesh Switch model 
 */
/******************************************************************************/

#ifndef __SWITCH_CLIENT_H__
#define __SWITCH_CLIENT_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/

/*! \addtogroup Switch_Client
 * @{
 */
#include "switch_model.h"

/*============================================================================*
Public Function Prototypes
*============================================================================*/

/*----------------------------------------------------------------------------*
 * SwitchModelClientInit
 */
/*! \brief 
 *      Initialises Switch Model Client functionality.
 *  \param app_callback Pointer to the application callback function that will
 *                      be called when the model client receives a message.
 *  \returns CSRmeshResult. Refer to \ref CSRmeshResult.
 */
/*----------------------------------------------------------------------------*/
extern CSRmeshResult SwitchModelClientInit(CSRMESH_MODEL_CALLBACK_T app_callback);


/*!@} */
#endif /* __SWITCH_CLIENT_H__ */

