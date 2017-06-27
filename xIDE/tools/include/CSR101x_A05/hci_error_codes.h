/*! \file hci_error_codes.h 
 *
 *  \brief Defines for HCI error codes
 *
 * Copyright (c) 2010 - 2011 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __HCI_ERROR_CODES_H__
#define __HCI_ERROR_CODES_H__

#include "hci_types.h"

/******************************************************************************
    HCI Successful return value 
 *****************************************************************************/
#define HCI_SUCCESS                         ((hci_return_t)0x00)
#define HCI_COMMAND_CURRENTLY_PENDING       ((hci_return_t)0x00)

/******************************************************************************
    HCI Error codes 
 *****************************************************************************/
#define HCI_ERROR_ILLEGAL_COMMAND           ((hci_error_t)0x01)
#define HCI_ERROR_NO_CONNECTION             ((hci_error_t)0x02)
#define HCI_ERROR_HARDWARE_FAIL             ((hci_error_t)0x03)
#define HCI_ERROR_PAGE_TIMEOUT              ((hci_error_t)0x04)
#define HCI_ERROR_AUTH_FAIL                 ((hci_error_t)0x05)
#define HCI_ERROR_KEY_MISSING               ((hci_error_t)0x06)
#define HCI_ERROR_MEMORY_FULL               ((hci_error_t)0x07)
#define HCI_ERROR_CONN_TIMEOUT              ((hci_error_t)0x08)
#define HCI_ERROR_MAX_NR_OF_CONNS           ((hci_error_t)0x09)
#define HCI_ERROR_MAX_NR_OF_SCO             ((hci_error_t)0x0A)
#define HCI_ERROR_MAX_NR_OF_ACL             ((hci_error_t)0x0B)
#define HCI_ERROR_COMMAND_DISALLOWED        ((hci_error_t)0x0C)
#define HCI_ERROR_REJ_BY_REMOTE_NO_RES      ((hci_error_t)0x0D)  /* no resource */
#define HCI_ERROR_REJ_BY_REMOTE_SEC         ((hci_error_t)0x0E)  /* security violation */
#define HCI_ERROR_REJ_BY_REMOTE_PERS        ((hci_error_t)0x0F)  /* personal device */
#define HCI_ERROR_HOST_TIMEOUT              ((hci_error_t)0x10) 
#define HCI_ERROR_UNSUPPORTED_FEATURE       ((hci_error_t)0x11)  /* or incorrect param value */
#define HCI_ERROR_ILLEGAL_FORMAT            ((hci_error_t)0x12) 
#define HCI_ERROR_OETC_USER                 ((hci_error_t)0x13)  /* other end terminated */
#define HCI_ERROR_OETC_LOW_RESOURCE         ((hci_error_t)0x14)  /* other end terminated */
#define HCI_ERROR_OETC_POWERING_OFF         ((hci_error_t)0x15)  /* other end terminated */
#define HCI_ERROR_CONN_TERM_LOCAL_HOST      ((hci_error_t)0x16)  /* local host terminated */
#define HCI_ERROR_AUTH_REPEATED             ((hci_error_t)0x17)  
#define HCI_ERROR_PAIRING_NOT_ALLOWED       ((hci_error_t)0x18) 
#define HCI_ERROR_UNKNOWN_LMP_PDU           ((hci_error_t)0x19) 
#define HCI_ERROR_UNSUPPORTED_REM_FEATURE   ((hci_error_t)0x1A) /* HCI_ERROR_UNSUPPORTED_LMP_FEATURE */
#define HCI_ERROR_SCO_OFFSET_REJECTED       ((hci_error_t)0x1B) 
#define HCI_ERROR_SCO_INTERVAL_REJECTED     ((hci_error_t)0x1C) 
#define HCI_ERROR_SCO_AIR_MODE_REJECTED     ((hci_error_t)0x1D) 
#define HCI_ERROR_INVALID_LMP_PARAMETERS    ((hci_error_t)0x1E) 
#define HCI_ERROR_UNSPECIFIED               ((hci_error_t)0x1F) 
#define HCI_ERROR_UNSUPP_LMP_PARAM          ((hci_error_t)0x20) 
#define HCI_ERROR_ROLE_CHANGE_NOT_ALLOWED   ((hci_error_t)0x21) 
#define HCI_ERROR_LMP_RESPONSE_TIMEOUT      ((hci_error_t)0x22)
#define HCI_ERROR_LMP_TRANSACTION_COLLISION ((hci_error_t)0x23)
#define HCI_ERROR_LMP_PDU_NOT_ALLOWED       ((hci_error_t)0x24) 
#define HCI_ERROR_ENC_MODE_NOT_ACCEPTABLE   ((hci_error_t)0x25) 
#define HCI_ERROR_UNIT_KEY_USED             ((hci_error_t)0x26) 
#define HCI_ERROR_QOS_NOT_SUPPORTED         ((hci_error_t)0x27) 
#define HCI_ERROR_INSTANT_PASSED            ((hci_error_t)0x28) 
#define HCI_ERROR_PAIR_UNIT_KEY_NO_SUPPORT  ((hci_error_t)0x29)
#define HCI_ERROR_DIFFERENT_TRANSACTION_COLLISION  ((hci_error_t)0x2A)
#define HCI_ERROR_SCM_INSUFFICIENT_RESOURCES ((hci_error_t)0x2B)
#define HCI_ERROR_QOS_UNACCEPTABLE_PARAMETER ((hci_error_t)0x2C)
#define HCI_ERROR_QOS_REJECTED              ((hci_error_t)0x2D)
#define HCI_ERROR_CHANNEL_CLASS_NO_SUPPORT  ((hci_error_t)0x2E)
#define HCI_ERROR_INSUFFICIENT_SECURITY     ((hci_error_t)0x2F)
#define HCI_ERROR_PARAM_OUT_OF_MAND_RANGE   ((hci_error_t)0x30)
#define HCI_ERROR_SCM_NO_LONGER_REQD        ((hci_error_t)0x31)
#define HCI_ERROR_ROLE_SWITCH_PENDING       ((hci_error_t)0x32)
#define HCI_ERROR_SCM_PARAM_CHANGE_PENDING  ((hci_error_t)0x33)
#define HCI_ERROR_RESVD_SLOT_VIOLATION      ((hci_error_t)0x34)
#define HCI_ERROR_ROLE_SWITCH_FAILED        ((hci_error_t)0x35)
#define HCI_ERROR_INQUIRY_RESPONSE_DATA_TOO_LARGE ((hci_error_t)0x36)
#define HCI_ERROR_SP_NOT_SUPPORTED_BY_HOST  ((hci_error_t)0x37)
#define HCI_ERROR_HOST_BUSY_PAIRING         ((hci_error_t)0x38)
#define HCI_ERROR_CONN_REJ_NO_SUITABLE_CHANNEL_FOUND   ((hci_error_t)0x39)
#define HCI_ERROR_CONTROLLER_BUSY                      ((hci_error_t)0x3A)
#define HCI_ERROR_UNACCEPTABLE_CONN_INTERVAL           ((hci_error_t)0x3B)
#define HCI_ERROR_DIRECTED_ADVERTISING_TIMEOUT         ((hci_error_t)0x3C)
#define HCI_ERROR_CONN_TERMINATED_DUE_TO_MIC_FAILURE   ((hci_error_t)0x3D)
#define HCI_ERROR_CONNECTION_FAILED_TO_BE_ESTABLISHED  ((hci_error_t)0x3E)

#endif /*__HCI_ERROR_CODES_H__*/
