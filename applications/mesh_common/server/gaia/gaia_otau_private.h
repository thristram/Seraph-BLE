/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_otau_private.h
 * \brief Private defines for the GAIA OTAu module
 *
 */
#ifndef GAIA_OTAU_PRIVATE_H_
#define GAIA_OTAU_PRIVATE_H_


/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>
#include <time.h>

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

/*
 * this is the maximum amount of data which can be requested based on the
 * minimum MTU size and the overhead of the GAIA and VM Upgrade protocol
 */
#define DATA_REQUEST_SIZE                                       12

/* VM upgrade file format header related definitions */

/* used when the next header ID is of an unknown type */
#define UNKNOWN_TYPE_ID_SIZE                                    8

/* header definitions */
#define HEADER_ID_SIZE                                          8
#define HEADER_LENGTH_SIZE                                      4

/* partition definitions */
#define PARTITION_HEADER_ID_OFFSET                              0
#define PARTITION_HEADER_ID_SIZE                                8
#define PARTITION_HEADER_LENGTH_OFFSET                          8
#define PARTITION_HEADER_LENGTH_SIZE                            4
#define PARTITION_HEADER_TYPE_OFFSET                            12
#define PARTITION_HEADER_TYPE_SIZE                              2
#define PARTITION_HEADER_NUMBER_OFFSET                          14
#define PARTITION_HEADER_NUMBER_SIZE                            2

/* footer definitions */
#define FOOTER_ID_OFFSET                                        0
#define FOOTER_ID_SIZE                                          8
#define FOOTER_LENGTH_OFFSET                                    8
#define FOOTER_LENGTH_SIZE                                      4
#define FOOTER_SIGNATURE_OFFSET                                 12

/* Upgrade protocol related definitions */
#define PROTOCOL_CURRENT_VERSION                                PROTOCOL_VERSION_3

#define VM_UPGRADE_MSG_SIZE                                     4

/*! Number of payload bytes in an UPGRADE_VARIANT_CFM protocol message */
#define UPGRADE_HOST_VARIANT_CFM_BYTE_SIZE                      8

#define UPGRADE_HOST_DATA_BYTES_REQ_BYTE_SIZE                   8
/*! Number of payload bytes in a UPGRADE_VERSION_CFM protocol message */
#define UPGRADE_HOST_VERSION_CFM_BYTE_SIZE                      6

/*! Size of a device variant field, in UPGRADE_HOST_VARIANT_CFM_T message */
#define DEVICE_VARIANT_FIELD_SIZE                               8

#define UPGRADE_HOST_IS_CSR_VALID_DONE_CFM_BYTE_SIZE            2

#define UPGRADE_HOST_ERRORWARN_IND_BYTE_SIZE                    2

#define UPGRADE_HOST_SYNC_CFM_BYTE_SIZE                         6

/* UPGRADE DATA packet information */
#define UPGRADE_DATA_MORE_DATA_OFFSET                           3
#define UPGRADE_DATA_IMAGE_DATA_OFFSET                          4
#define UPGRADE_DATA_MORE_DATA_SIZE                             1
#define UPGRADE_DATA_LAST_PACKET                                1

#define UPGRADE_SEND_ERROR_IN_PAYLOAD_SIZE                      2
#define UPGRADE_TRANSFER_COMPLETE_RES_ACTION_OFFSET             3
#define UPGRADE_TRANSFER_COMPLETE_RES_ACTION_CONTINUE_UPGRADE   0x00
#define UPGRADE_TRANSFER_COMPLETE_IND_PAYLOAD_SIZE              0
#define UPGRADE_HOST_START_CFM_PAYLOAD_SIZE                     3
#define UPGRADE_HOST_ABORT_CFM_PAYLOAD_SIZE                     0
#define UPGRADE_HOST_COMMIT_REQ_PAYLOAD_SIZE                    0
#define UPGRADE_HOST_UPDATE_COMPLETE_IND_PAYLOAD_SIZE           0
#define UPGRADE_COMMIT_CFM_ACTION_COMMIT_UPGRADE                0
#define UPGRADE_ERROR_IND_PAYLOAD_SIZE                          2
#define UPGRADE_VALIDATION_CFM_PAYLOAD_SIZE                     2
#define UPGRADE_VALIDATION_BACKOFF_TIME_DEFAULT                 5000

#define GAIA_COMMAND_VM_UPGRADE_CONTROL_DATA_OFFSET             3
#define GAIA_COMMAND_VM_UPGRADE_CONTROL_LENGTH_OFFSET           1

#define UPGRADE_STORE_APPID_FIRST                               1
#define UPGRADE_STORE_APPID_LAST                                2

#define STORE_SIZE_MULTIPLIER_1                                 (0<<14)
#define STORE_SIZE_MULTIPLIER_16                                (1<<14)
#define STORE_SIZE_MULTIPLIER_4K                                (2<<14)
#define STORE_SIZE_MULTIPLIER_RESERVED                          (3<<14)
#define UPGRADE_STORE_SIZE                                      (STORE_SIZE_MULTIPLIER_4K | 1)

#define STORE_HEADER_SIZE_OCTETS                                8
#define STORE_HEADER_SIZE_WORDS                                 4

#define GAIA_NVM_IN_PROGRESS_IDENTIFIER_OFFSET                  0
#define GAIA_NVM_IN_PROGRESS_IDENTIFIER_SIZE_WORDS              2
#define GAIA_NVM_RESUME_POINT_OFFSET                            2
#define GAIA_NVM_RESUME_POINT_SIZE_WORDS                        1
#define GAIA_OTAU_SERVICE_NVM_MEMORY_WORDS  ((GAIA_NVM_IN_PROGRESS_IDENTIFIER_SIZE_WORDS) +   \
                                            (GAIA_NVM_RESUME_POINT_SIZE_WORDS) )

#define GAIA_IN_PROGRESS_IDENTIFIER_NONE                        0
#define GAIA_IN_PROGRESS_IDENTIFIER_DEFAULT     GAIA_IN_PROGRESS_IDENTIFIER_NONE
#define GAIA_RESUME_POINT_DEFAULT               UPGRADE_RESUME_POINT_START

#define SHA256_SIZE_WORDS                                       16

#define GAIA_OTAU_COMMIT_HOST_TIMEOUT                           (300 *  SECOND)

#define SETSTOREID_SIZE_MIN                                     10

/* Size encoding: bits 15:14 = multiplier flags, 13:0 = value */
/* Max size in bytes for each multiplier */
#define MAX_SIZE_1B                                 (0x3FFFUL * 1)
#define MAX_SIZE_16B                                (0x3FFFUL * 16)
#define MAX_SIZE_4KB                                (0x3FFFUL * 4096)
/* Check for remainder for each multiplier */
#define REMAINDER_1B                                0x00000000UL
#define REMAINDER_16B                               0x0000000FUL
#define REMAINDER_4KB                               0x00000FFFUL
/* Number of bits to shift for each multiplier */
#define SHIFT_1B                                    0
#define SHIFT_16B                                   4
#define SHIFT_4KB                                   12
/* Multiplier flags */
#define FLAG_1B                                     0x0000
#define FLAG_16B                                    0x4000
#define FLAG_4KB                                    0x8000


/*=============================================================================*
 *  Private Data Types
 *============================================================================*/

typedef enum {
    data_transfer_state_header_id = 0x0,
    data_transfer_state_header_length,
    data_transfer_state_header_body,
    data_transfer_state_unknown_header_id,
    data_transfer_state_partition_header_body,
    data_transfer_state_partition_data,
    data_transfer_state_footer_length,
    data_transfer_state_footer_oem_signature
} TRANSFER_DATA_TYPE;

typedef enum {
    CTRL_SECURITY,          /*!< The host must perform the authentication procedure before going further. */
    CTRL_WAIT_FOR_HOST,     /*!< Device is connected and waiting for the host to start the procedure. */
    CTRL_WAIT_FOR_DEVICE,   /*!< Device is awaiting for approval from the application code. */
    CTRL_READY,             /*!< The device is ready to start the data transfer. */
    CTRL_IN_PROGRESS,       /*!< Data transfer is in progress. */
    CTRL_PAUSED,            /*!< Not used at the moment. */
    CTRL_COMPLETED,         /*!< Upgrade completed successfully. */
    CTRL_FAILED,            /*!< Integrity check failed. */
    CTRL_ABORTED,           /*!< Abort of the transfer caused by a fatal error. */
    CTRL_DISCONNECTING,     /*!< Link is disconnecting soon. */
    CTRL_PRE_FAIL,          /*!< OTAu cannot continue due to a failure before the upgrade started */
    CTRL_WAITING_TO_REBOOT, /*!< OTAu cannot continue due to a failure before the upgrade started */

} TRANSFER_CTRL;

typedef enum {
    PROTOCOL_VERSION_1 = 1,
    PROTOCOL_VERSION_2,
    PROTOCOL_VERSION_3
} PROTOCOL_VERSION;

typedef enum {
    UPGRADE_HOST_START_REQ = 1,
    UPGRADE_HOST_START_CFM,
    UPGRADE_HOST_DATA_BYTES_REQ,
    UPGRADE_HOST_DATA,
    UPGRADE_HOST_SUSPEND_IND,
    UPGRADE_HOST_RESUME_IND,
    UPGRADE_HOST_ABORT_REQ,
    UPGRADE_HOST_ABORT_CFM,
    UPGRADE_HOST_PROGRESS_REQ,
    UPGRADE_HOST_PROGRESS_CFM,
    UPGRADE_HOST_TRANSFER_COMPLETE_IND,
    UPGRADE_HOST_TRANSFER_COMPLETE_RES,
    UPGRADE_HOST_IN_PROGRESS_IND,
    UPGRADE_HOST_IN_PROGRESS_RES,
    UPGRADE_HOST_COMMIT_REQ,
    UPGRADE_HOST_COMMIT_CFM,
    UPGRADE_HOST_ERRORWARN_IND, /* _RES below */
    UPGRADE_HOST_COMPLETE_IND,
    UPGRADE_HOST_SYNC_REQ,
    UPGRADE_HOST_SYNC_CFM,

    UPGRADE_HOST_START_DATA_REQ,
    UPGRADE_HOST_IS_CSR_VALID_DONE_REQ,
    UPGRADE_HOST_IS_CSR_VALID_DONE_CFM,
    UPGRADE_HOST_SYNC_AFTER_REBOOT_REQ,

    UPGRADE_HOST_VERSION_REQ,
    UPGRADE_HOST_VERSION_CFM,
    UPGRADE_HOST_VARIANT_REQ,
    UPGRADE_HOST_VARIANT_CFM,

    UPGRADE_HOST_ERASE_SQIF_REQ,
    UPGRADE_HOST_ERASE_SQIF_CFM,

    UPGRADE_HOST_ERRORWARN_RES
} UPGRADE_MESSAGE_HOST;

typedef enum {
    UPGRADE_HOST_SUCCESS = 0,
    UPGRADE_HOST_OEM_VALIDATION_SUCCESS,

    UPGRADE_HOST_ERROR_INTERNAL_ERROR_DEPRECATED = 0x10,
    UPGRADE_HOST_ERROR_UNKNOWN_ID,
    UPGRADE_HOST_ERROR_BAD_LENGTH_DEPRECATED,
    UPGRADE_HOST_ERROR_WRONG_VARIANT,
    UPGRADE_HOST_ERROR_WRONG_PARTITION_NUMBER,

    UPGRADE_HOST_ERROR_PARTITION_SIZE_MISMATCH,
    UPGRADE_HOST_ERROR_PARTITION_TYPE_NOT_FOUND_DEPRECATED,
    UPGRADE_HOST_ERROR_PARTITION_OPEN_FAILED,
    UPGRADE_HOST_ERROR_PARTITION_WRITE_FAILED_DEPRECATED,
    UPGRADE_HOST_ERROR_PARTITION_CLOSE_FAILED_DEPRECATED,

    UPGRADE_HOST_ERROR_SFS_VALIDATION_FAILED,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_DEPRECATED,
    UPGRADE_HOST_ERROR_UPDATE_FAILED,
    UPGRADE_HOST_ERROR_APP_NOT_READY,

    UPGRADE_HOST_ERROR_LOADER_ERROR,
    UPGRADE_HOST_ERROR_UNEXPECTED_LOADER_MSG,
    UPGRADE_HOST_ERROR_MISSING_LOADER_MSG,

    UPGRADE_HOST_ERROR_BATTERY_LOW,
    UPGRADE_HOST_ERROR_INVALID_SYNC_ID,
    UPGRADE_HOST_ERROR_IN_ERROR_STATE,
    UPGRADE_HOST_ERROR_NO_MEMORY,

    /* The remaining errors are grouped, each section starting at a fixed
     * offset */
    UPGRADE_HOST_ERROR_BAD_LENGTH_PARTITION_PARSE = 0x30,
    UPGRADE_HOST_ERROR_BAD_LENGTH_TOO_SHORT,
    UPGRADE_HOST_ERROR_BAD_LENGTH_UPGRADE_HEADER,
    UPGRADE_HOST_ERROR_BAD_LENGTH_PARTITION_HEADER,
    UPGRADE_HOST_ERROR_BAD_LENGTH_SIGNATURE,
    UPGRADE_HOST_ERROR_BAD_LENGTH_DATAHDR_RESUME,

    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_HEADERS = 0x38,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_UPGRADE_HEADER,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_PARTITION_HEADER1,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_PARTITION_HEADER2,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_PARTITION_DATA,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_FOOTER,
    UPGRADE_HOST_ERROR_OEM_VALIDATION_FAILED_MEMORY,

    UPGRADE_HOST_ERROR_PARTITION_CLOSE_FAILED = 0x40,
    UPGRADE_HOST_ERROR_PARTITION_CLOSE_FAILED_HEADER,
    /*! When sent, the error indicates that an upgrade could not be completed
     * due to concerns about space in Persistent Store.  No other upgrade
     * activities will be possible until the device restarts.
     * This error requires a UPGRADE_HOST_ERRORWARN_RES response (following
     * which the library will cause a restart, if the VM application permits)
     */
    UPGRADE_HOST_ERROR_PARTITION_CLOSE_FAILED_PS_SPACE,

    UPGRADE_HOST_ERROR_PARTITION_TYPE_NOT_MATCHING = 0x48,
    UPGRADE_HOST_ERROR_PARTITION_TYPE_TWO_DFU,

    UPGRADE_HOST_ERROR_PARTITION_WRITE_FAILED_HEADER = 0x50,
    UPGRADE_HOST_ERROR_PARTITION_WRITE_FAILED_DATA,

    UPGRADE_HOST_ERROR_FILE_TOO_SMALL = 0x58,
    UPGRADE_HOST_ERROR_FILE_TOO_BIG,

    UPGRADE_HOST_ERROR_INTERNAL_ERROR_1 = 0x65, /* 101 - Human readable decimal*/
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_2,
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_3,
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_4,
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_5,
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_6,
    UPGRADE_HOST_ERROR_INTERNAL_ERROR_7,

    UPGRADE_HOST_WARN_APP_CONFIG_VERSION_INCOMPATIBLE = 0x80,
    UPGRADE_HOST_WARN_SYNC_ID_IS_DIFFERENT
} UPGRADE_HOST_ERROR_CODE;


typedef enum {
    /*! Resume from the beginning, includes download phase. */
    UPGRADE_RESUME_POINT_START,

    /*! Resume from the start of the validation phase, i.e. download is complete. */
    UPGRADE_RESUME_POINT_PRE_VALIDATE,

    /*! Resume after the validation phase, but before the device has rebooted
     *  to action the upgrade. */
    UPGRADE_RESUME_POINT_PRE_REBOOT,

    /*! Resume after the reboot */
    UPGRADE_RESUME_POINT_POST_REBOOT,

    /*! Resume at final stage of an upgrade, ready for host to commit */
    UPGRADE_RESUME_POINT_COMMIT,

    /*! Final stage of an upgrade, partition erase still required */
    UPGRADE_RESUME_POINT_ERASE,

    /*! Resume in error handling, before reset unhandled error message have been sent */
    UPGRADE_RESUME_POINT_ERROR

} UPGRADE_RESUME_POINT;

#endif /* GAIA_OTAU_PRIVATE_H_ */
