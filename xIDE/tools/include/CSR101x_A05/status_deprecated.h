/*! \file
 *  \brief  Legacy status code definitions that have since been replaced with
 *          the global status codes defined in status.h
 *
 *  Status code definitions in this file are retained for backward compatibility
 *  with exiting applications but should not be used in new developments.
 *  Individual definitions in this file are deliberately not documented to
 *  discourage their use.
 *
 * Copyright (c) 2011 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __STATUS_DEPRECATED_H__
#define __STATUS_DEPRECATED_H__

/* The old names for these various status codes are mapped via pre-processor
 * definitions to the new names.
 */
#define L2CAP_SUCCESS                       sys_status_success

/* Below set of return codes are mapped to HCI return codes. */
#define L2CAP_ERR_ILLEGAL_CMD               HCI_ERROR_ILLEGAL_COMMAND
#define L2CAP_ERR_NO_CONNECTION             HCI_ERROR_NO_CONNECTION
#define L2CAP_ERR_HARDWARE_FAIL             HCI_ERROR_HARDWARE_FAIL
#define L2CAP_ERR_PAGE_TIMEOUT              HCI_ERROR_PAGE_TIMEOUT
#define L2CAP_ERR_AUTH_FAIL                 HCI_ERROR_AUTH_FAIL
#define L2CAP_ERR_KEY_MISSING               HCI_ERROR_KEY_MISSING
#define L2CAP_ERR_MEMORY_FULL               HCI_ERROR_MEMORY_FULL
#define L2CAP_ERR_CONN_TIMEOUT              HCI_ERROR_CONN_TIMEOUT
#define L2CAP_ERR_MAX_NR_OF_CONNS           HCI_ERROR_MAX_NR_OF_CONNS
#define L2CAP_ERR_MAX_NR_OF_SCO             HCI_ERROR_MAX_NR_OF_SCO
#define L2CAP_ERR_MAX_NR_OF_ACL             HCI_ERROR_MAX_NR_OF_ACL
#define L2CAP_ERR_CMD_DISALLOWED            HCI_ERROR_COMMAND_DISALLOWED
#define L2CAP_ERR_REJ_BY_REMOTE_NO_RES      HCI_ERROR_REJ_BY_REMOTE_NO_RES
#define L2CAP_ERR_REJ_BY_REMOTE_SEC         HCI_ERROR_REJ_BY_REMOTE_SEC
#define L2CAP_ERR_REJ_BY_REMOTE_PERS        HCI_ERROR_REJ_BY_REMOTE_PERS
#define L2CAP_ERR_HOST_TIMEOUT              HCI_ERROR_HOST_TIMEOUT
#define L2CAP_ERR_UNSUPPORTED_FEATURE       HCI_ERROR_UNSUPPORTED_FEATURE
#define L2CAP_ERR_ILLEGAL_FORMAT            HCI_ERROR_ILLEGAL_FORMAT
#define L2CAP_ERR_OETC_USER                 HCI_ERROR_OETC_USER
#define L2CAP_ERR_OETC_LOW_RESOURCE         HCI_ERROR_OETC_LOW_RESOURCE
#define L2CAP_ERR_OETC_POWERING_OFF         HCI_ERROR_OETC_POWERING_OFF
#define L2CAP_ERR_CONN_TERM_LOCAL_HOST      HCI_ERROR_CONN_TERM_LOCAL_HOST
#define L2CAP_ERR_AUTH_REPEATED             HCI_ERROR_AUTH_REPEATED
#define L2CAP_ERR_PAIRING_NOT_ALLOWED       HCI_ERROR_PAIRING_NOT_ALLOWED
#define L2CAP_ERR_UNKNOWN_LMP_PDU           HCI_ERROR_UNKNOWN_LMP_PDU
#define L2CAP_ERR_UNSUPPORTED_REM_FEATURE   HCI_ERROR_UNSUPPORTED_REM_FEATURE
#define L2CAP_ERR_SCO_OFFSET_REJECTED       HCI_ERROR_SCO_OFFSET_REJECTED
#define L2CAP_ERR_SCO_INTERVAL_REJECTED     HCI_ERROR_SCO_INTERVAL_REJECTED
#define L2CAP_ERR_SCO_AIR_MODE_REJECTED     HCI_ERROR_SCO_AIR_MODE_REJECTED
#define L2CAP_ERR_INVALID_LMP_PARAMETERS    HCI_ERROR_INVALID_LMP_PARAMETERS
#define L2CAP_ERR_UNSPECIFIED               HCI_ERROR_UNSPECIFIED
#define L2CAP_ERR_UNSUPP_LMP_PARAM          HCI_ERROR_UNSUPP_LMP_PARAM
#define L2CAP_ERR_ROLE_CHANGE_NOT_ALLOWED   HCI_ERROR_ROLE_CHANGE_NOT_ALLOWED
#define L2CAP_ERR_LMP_RESPONSE_TIMEOUT      HCI_ERROR_LMP_RESPONSE_TIMEOUT
#define L2CAP_ERR_LMP_TRANSACTION_COLLISION HCI_ERROR_LMP_TRANSACTION_COLLISION
#define L2CAP_ERR_LMP_PDU_NOT_ALLOWED       HCI_ERROR_LMP_PDU_NOT_ALLOWED
#define L2CAP_ERR_ENC_MODE_NOT_ACCEPTABLE   HCI_ERROR_ENC_MODE_NOT_ACCEPTABLE
#define L2CAP_ERR_UNIT_KEY_USED             HCI_ERROR_UNIT_KEY_USED
#define L2CAP_ERR_QOS_NOT_SUPPORTED         HCI_ERROR_QOS_NOT_SUPPORTED
#define L2CAP_ERR_INSTANT_PASSED            HCI_ERROR_INSTANT_PASSED
#define L2CAP_ERR_PAIR_UNIT_KEY_NO_SUPPORT  HCI_ERROR_PAIR_UNIT_KEY_NO_SUPPORT
#define L2CAP_ERR_DIFFERENT_TRANSACTION_COLLISION HCI_ERROR_DIFFERENT_TRANSACTION_COLLISION
#define L2CAP_ERR_SCM_INSUFFICIENT_RESOURCES HCI_ERROR_SCM_INSUFFICIENT_RESOURCES
#define L2CAP_ERR_QOS_UNACCEPTABLE_PARAMETER HCI_ERROR_QOS_UNACCEPTABLE_PARAMETER
#define L2CAP_ERR_QOS_REJECTED              HCI_ERROR_QOS_REJECTED
#define L2CAP_ERR_CHANNEL_CLASS_NO_SUPPORT  HCI_ERROR_CHANNEL_CLASS_NO_SUPPORT
#define L2CAP_ERR_INSUFFICIENT_SECURITY     HCI_ERROR_INSUFFICIENT_SECURITY
#define L2CAP_ERR_PARAM_OUT_OF_MAND_RANGE   HCI_ERROR_PARAM_OUT_OF_MAND_RANGE
#define L2CAP_ERR_SCM_NO_LONGER_REQD        HCI_ERROR_SCM_NO_LONGER_REQD
#define L2CAP_ERR_ROLE_SWITCH_PENDING       HCI_ERROR_ROLE_SWITCH_PENDING
#define L2CAP_ERR_SCM_PARAM_CHANGE_PENDING  HCI_ERROR_SCM_PARAM_CHANGE_PENDING
#define L2CAP_ERR_RESVD_SLOT_VIOLATION      HCI_ERROR_RESVD_SLOT_VIOLATION
#define L2CAP_ERR_ROLE_SWITCH_FAILED        HCI_ERROR_ROLE_SWITCH_FAILED
#define L2CAP_ERR_INQUIRY_RESPONSE_DATA_TOO_LARGE HCI_ERROR_INQUIRY_RESPONSE_DATA_TOO_LARGE
#define L2CAP_ERR_SP_NOT_SUPPORTED_BY_HOST  HCI_ERROR_SP_NOT_SUPPORTED_BY_HOST
#define L2CAP_ERR_HOST_BUSY_PAIRING         HCI_ERROR_HOST_BUSY_PAIRING
#define L2CAP_ERR_CONN_REJ_NO_SUITABLE_CHANNEL_FOUND HCI_ERROR_CONN_REJ_NO_SUITABLE_CHANNEL_FOUND
#define L2CAP_ERR_CONTROLLER_BUSY           HCI_ERROR_CONTROLLER_BUSY
#define L2CAP_ERR_UNACCEPTABLE_CONN_INTERVAL HCI_ERROR_UNACCEPTABLE_CONN_INTERVAL
#define L2CAP_ERR_DIRECTED_ADVERTISING_TIMEOUT HCI_ERROR_DIRECTED_ADVERTISING_TIMEOUT
#define L2CAP_ERR_CONN_TERMINATED_DUE_TO_MIC_FAILURE HCI_ERROR_CONN_TERMINATED_DUE_TO_MIC_FAILURE
#define L2CAP_ERR_CONNECTION_FAILED_TO_BE_ESTABLISHED HCI_ERROR_CONNECTION_FAILED_TO_BE_ESTABLISHED

#define L2CAP_INVALID_CONN_STATE            l2cap_status_invalid_conn_state
#define L2CAP_CONN_DISALLOWED               l2cap_status_conn_disallowed
#define L2CAP_CONN_NOT_ONGOING              l2cap_status_conn_not_ongoing
#define L2CAP_BUFFER_FULL                   l2cap_status_buffer_full
#define L2CAP_LIMITED_ADVERTISING_TIMEOUT   ls_status_limited_advertising_timeout
#define L2CAP_RESULT_INVALID                sys_status_invalid

#define GATT_RESULT_INVALID_HANDLE          gatt_status_invalid_handle
#define GATT_RESULT_READ_NOT_PERMITTED      gatt_status_read_not_permitted
#define GATT_RESULT_WRITE_NOT_PERMITTED     gatt_status_write_not_permitted
#define GATT_RESULT_INVALID_PDU             gatt_status_invalid_pdu
#define GATT_RESULT_INSUFFICIENT_AUTHENTICATION \
                                            gatt_status_insufficient_authentication
#define GATT_RESULT_REQUEST_NOT_SUPPORTED   gatt_status_request_not_supported
#define GATT_RESULT_INVALID_OFFSET          gatt_status_invalid_offset
#define GATT_RESULT_INSUFFICIENT_AUTHORIZATION \
                                            gatt_status_insufficient_authorization
#define GATT_RESULT_PREPARE_QUEUE_FULL      gatt_status_prepare_queue_full
#define GATT_RESULT_ATTR_NOT_FOUND          gatt_status_attr_not_found
#define GATT_RESULT_NOT_LONG                gatt_status_not_long
#define GATT_RESULT_INSUFFICIENT_ENCR_KEY_SIZE \
                                            gatt_status_insufficient_encr_key_size
#define GATT_RESULT_INVALID_LENGTH          gatt_status_invalid_length
#define GATT_RESULT_UNLIKELY_ERROR          gatt_status_unlikely_error
#define GATT_RESULT_INSUFFICIENT_ENCRYPTION gatt_status_insufficient_encryption
#define GATT_RESULT_UNSUPPORTED_GROUP_TYPE  gatt_status_unsupported_group_type
#define GATT_RESULT_INSUFFICIENT_RESOURCES  gatt_status_insufficient_resources
#define GATT_RESULT_APP_MASK                gatt_status_app_mask
#define GATT_RESULT_DEVICE_NOT_FOUND        gatt_status_device_not_found
#define GATT_RESULT_SIGN_FAILED             gatt_status_sign_failed
#define GATT_RESULT_BUSY                    gatt_status_busy
#define GATT_RESULT_TIMEOUT                 gatt_status_timeout
#define GATT_RESULT_INVALID_MTU             gatt_status_invalid_mtu
#define GATT_RESULT_INVALID_UUID            gatt_status_invalid_uuid
#define GATT_RESULT_SUCCESS_MORE            gatt_status_success_more
#define GATT_RESULT_SUCCESS_SENT            gatt_status_success_sent
#define GATT_RESULT_INVALID_CID             gatt_status_invalid_cid
#define GATT_RESULT_INVALID_DB              gatt_status_invalid_db
#define GATT_RESULT_DB_FULL                 gatt_status_db_full
#define GATT_RESULT_INVALID_PERMISSIONS     gatt_status_invalid_permissions
#define GATT_RESULT_INVALID_OPERATION       gatt_status_invalid_operation
#define GATT_RESULT_INVALID_PARAM_VALUE     gatt_status_invalid_param_value
#define GATT_RESULT_DATA_VALIDATION_FAILED  gatt_status_data_validation_failed

#define SM_ERROR_RESERVED                   sm_status_reserved
#define SM_ERROR_PASSKEY_ENTRY_FAILED       sm_status_passkey_entry_failed
#define SM_ERROR_OOB_NOT_AVAILABLE          sm_status_oob_not_available
#define SM_ERROR_AUTHENTICATION_REQUIREMENTS sm_status_authentication_requirements
#define SM_ERROR_CONFIRM_VALUE_FAILED       sm_status_confirm_value_failed
#define SM_ERROR_PAIRING_NOT_SUPPORTED      sm_status_pairing_not_supported
#define SM_ERROR_ENCRYPTION_KEY_SIZE        sm_status_encryption_key_size
#define SM_ERROR_COMMAND_NOT_SUPPORTED      sm_status_command_not_supported
#define SM_ERROR_UNSPECIFIED_REASON         sm_status_unspecified_reason
#define SM_ERROR_REPEATED_ATTEMPTS          sm_status_repeated_attempts
#define SM_ERROR_INVALID_PARAMETER          sm_status_invalid_parameter

#endif  /* __STATUS_DEPRECATED_H__ */
