/*! \file hci_event_types.h
 *
 *  \brief Header file of type definitions for the HCI event handling. 
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __HCI_EVENT_TYPES_H__
#define __HCI_EVENT_TYPES_H__

#include "types.h"
#include "bluetooth.h"
#include "hci_types.h"

/*============================================================================*
Public Definitions
*============================================================================*/

/*! \brief  Event Packet Common Fields
 */
typedef struct
{
    hci_event_code_t         event_code;        
    uint8                    length;            
} HCI_EVENT_COMMON_T;


/*! \brief  ULP Connection Creation
 *
 *  HCI ULP Connection Complete Event (0x3E, subevent code: 0x01)
 */
typedef struct
{
    hci_return_t             status;
    hci_connection_handle_t  connection_handle;
    uint8                    role;
    uint8                    peer_address_type;
    BD_ADDR_T                peer_address;
    uint16                   conn_interval;
    uint16                   conn_latency;
    uint16                   supervision_timeout;
    uint8                    clock_accuracy;
} HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T;

/*! \brief  ULP Advertising
 *
 *  HCI ULP Advertising Report Event (0x3E, subevent code: 0x02)
 */
typedef struct
{
    uint8                    num_reports;
    uint8                    event_type;
    uint8                    address_type;
    BD_ADDR_T                address;
    uint8                    length_data;
} HCI_EV_DATA_ULP_ADVERTISING_REPORT_T;

/*! \brief  ULP Connection Update
 *
 *  HCI ULP Connection Update Complete Event (0x3E, subevent code: 0x03)
 */
typedef struct
{
    hci_return_t             status;
    hci_connection_handle_t  connection_handle;
    uint16                   conn_interval;
    uint16                   conn_latency;
    uint16                   supervision_timeout;
} HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T;

/*! \brief  ULP Read Remote Used Features
 *
 *  HCI ULP Read Remote Used Features Complete Event (0x3E,
 *                                                      subevent code: 0x04)
 */
typedef struct
{
    hci_return_t             status;
    hci_connection_handle_t  connection_handle;
    uint8                    features[8];
} HCI_EV_DATA_ULP_READ_REMOTE_USED_FEATURES_COMPLETE_T;

/*! \brief  ULP Encryption
 *
 *  HCI ULP Long Term Key Requested Event (0x3E, subevent code: 0x05)
 */
typedef struct
{
    hci_connection_handle_t  connection_handle;
    uint8                    random_number[8];
    uint16                   encryption_diversifier;
} HCI_EV_DATA_ULP_LONG_TERM_KEY_REQUESTED_T;

/*! \brief  Common header for ULP events
 */
typedef struct
{
    hci_event_code_t event_code;       
    uint8                    length;          
    uint8                    ulp_sub_opcode;
} HCI_ULP_EVENT_COMMON_T;


/*! \brief  Connection Complete Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    hci_connection_handle_t  handle;
    BD_ADDR_T                bd_addr;
    uint8                    link_type;
    uint8                    enc_mode;
} HCI_EV_DATA_CONN_COMPLETE_T;

/*! \brief  Disconnection Complete Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    hci_connection_handle_t  handle;
    hci_error_t              reason;             /* Reason for disconnection 0x08, 0x13 - 0x16 only */
} HCI_EV_DATA_DISCONNECT_COMPLETE_T;

/*! \brief  Encryption Change Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    hci_connection_handle_t  handle;
    hci_link_enc_t           enc_enable;
} HCI_EV_DATA_ENCRYPTION_CHANGE_T;

/*! \brief  Encryption Key Refresh Complete Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    hci_connection_handle_t  handle;
} HCI_EV_DATA_ENCRYPTION_KEY_REFRESH_COMPLETE_T;

/*!\brief   Read Remote Version Information Complete Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    hci_connection_handle_t  handle;
    uint8                    lmp_version;        /* defined in LMP */
    uint16                   manuf_name;         /* defined in LMP */
    uint16                   lmp_subversion;
} HCI_EV_DATA_READ_REMOTE_VER_INFO_COMPLETE_T;

/*! \brief  ULP Connection Creation
 *
 *  HCI ULP Connection Complete Event (0x3E, subevent code: 0x01)
 */
typedef struct
{
    HCI_ULP_EVENT_COMMON_T                  event;
    HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T   data;
}  HCI_EV_ULP_CONNECTION_COMPLETE_T;

/*! \brief  ULP Advertising
 *
 *  HCI ULP Advertising Report Event (0x3E, subevent code: 0x02)
 */
typedef struct
{
    HCI_ULP_EVENT_COMMON_T                event;
    HCI_EV_DATA_ULP_ADVERTISING_REPORT_T  data;
    uint8                                 adv_data[31];
    uint8                                 rssi;
}  HCI_EV_ULP_ADVERTISING_REPORT_T;

/*! \brief  ULP Connection Update
 *      
 *  HCI ULP Connection Update Complete Event (0x3E, subevent code: 0x03)
 */
typedef struct
{
    HCI_ULP_EVENT_COMMON_T                          event;
    HCI_EV_DATA_ULP_CONNECTION_UPDATE_COMPLETE_T    data;
}  HCI_EV_ULP_CONNECTION_UPDATE_COMPLETE_T;

/*! \brief  ULP Read Remote Used Features
 *
 *  HCI ULP Read Remote Used Features Complete Event (0x3E, 
 *                                                      subevent code: 0x04)
 */
typedef struct
{
    HCI_ULP_EVENT_COMMON_T                                  event;
    HCI_EV_DATA_ULP_READ_REMOTE_USED_FEATURES_COMPLETE_T    data;
}  HCI_EV_ULP_READ_REMOTE_USED_FEATURES_COMPLETE_T;

/*! \brief  ULP Encryption
 *
 *  HCI ULP Long Term Key Requested Event (0x3E, subevent code: 0x05)
 */
typedef struct
{
    HCI_ULP_EVENT_COMMON_T                          event;
    HCI_EV_DATA_ULP_LONG_TERM_KEY_REQUESTED_T       data;
}  HCI_EV_ULP_LONG_TERM_KEY_REQUESTED_T;


/*! \brief  Hardware Error Event
 */
typedef struct
{
    HCI_EVENT_COMMON_T       event;
    uint8                    hw_error;           
} HCI_EV_HARDWARE_ERROR_T;

typedef struct
{
    uint8 hw_error;
} HCI_EV_DATA_HARDWARE_ERROR_T;


/*! \brief  Data Buffer Overflow Event
 */
typedef struct
{
    HCI_EVENT_COMMON_T       event;
    uint8                    link_type;
} HCI_EV_DATA_BUFFER_OVERFLOW_T;


typedef struct
{
    uint8                    link_type;
} HCI_EV_DATA_DATA_BUFFER_OVERFLOW_T;


/*! \brief  Variable argument field
 */
typedef struct
{
    hci_connection_handle_t  handle;
    uint16                   num_completed;
} HANDLE_COMPLETE_T;


/*! \brief  Command Complete Event
 */
typedef struct
{
    uint8                    num_hci_command_pkts;
    hci_op_code_t            op_code;            /* op code of command causing this event */
    hci_return_t             status;
    void                     *argument_ptr;      /* HCI_COMMAND_COMPLETE_ARGS_T */
} HCI_EV_DATA_COMMAND_COMPLETE_T;


/*! \brief  Command Status Event
 */
typedef struct
{
    hci_return_t             status;             /* cast to error if error */
    uint8                    num_hci_command_pkts;
    hci_op_code_t            op_code;            /* op code of command that
                                                    caused this event */
} HCI_EV_DATA_COMMAND_STATUS_T;


/*! \brief  Debug Event
 */
typedef struct
{
    uint8 debug_event;
    uint16 num0;
    uint16 num1;
    uint16 num2;
    uint16 num3;
    uint16 num4;
    uint16 num5;
    uint16 num6;
    uint16 num7;
    uint16 num8;
} HCI_EV_DEBUG_T;


/*! \brief  Allow manufacturer's extension events to go over HCI.  
 *
 *  These are used for:
 *
 *  1. To tunnel additional protocols over HCI (see CSR specification
 *          bc01-s-023c).  These are carried in BlueCore-Friendly format.
 *  2. For responses to debug commands.
 *
 *  The opaque type for bccmds is shorter than the maximum BCCMD pdu.
 *  However we never use bccmds anything like that long in the CSR1000
 *  HCI, so we use a shorter value here to avoid wasting memory.
 *----------------------------------------------------------------------------*/
typedef union
{
    uint8                    *bcf;               /* carries tunnelled primitives */
    uint16                   bccmd[16];          /* (opaque) carries bccmds      */
    HCI_EV_DEBUG_T           debug_event;        /* carries debug events         */
} EV_MNFR_EXTN_PAYLOAD_T;


/*! \brief  Manufacturer-specific Event
 */
typedef struct
{
    uint8                    payload_descriptor;
    uint8                    payload_word_len;
    EV_MNFR_EXTN_PAYLOAD_T   payload;
} HCI_EV_DATA_MNFR_EXTENSION_T;


#endif /* __HCI_EVENT_TYPES_H__ */
