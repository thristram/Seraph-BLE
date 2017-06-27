/*! \file hci_types.h
 *
 *  \brief Basic type definitions for the HCI.  
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
*/

#ifndef __HCI_TYPES_H__
#define __HCI_TYPES_H__

#include "types.h"

/*============================================================================*
Public Definitions
*============================================================================*/

typedef uint8  hci_return_t;             /* return value (error if !success) */
typedef uint8  hci_error_t;
typedef uint16 hci_connection_handle_t;
typedef uint8  hci_event_code_t;         /* used for events    */
typedef uint8  hci_link_enc_t;           /* whether link encryption on or off */
typedef uint16 hci_op_code_t;            /* used for commands  */

#endif /* __HCI_TYPES_H__ */
