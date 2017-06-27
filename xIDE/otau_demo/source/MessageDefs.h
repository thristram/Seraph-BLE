/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      MessageDefs.h
 *
 *  DESCRIPTION
 *    Application message definitions
 *
 ******************************************************************************/
#ifndef __MESSAGE_DEFS__
#define __MESSAGE_DEFS__

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

#define DIALOG_MESSAGE_ID                       WM_USER+10

#define OTA_MSG                                 1
#define LOGGING_MSG                             2

// General UI message identifiers
#define SEARCH_RESULT                           200
#define DEVICE_SCANNING_UPDATE                  201
#define DEVICE_CONNECTION_UPDATE                202
#define BONDED_DEVICE                           203
#define HW_ERR                                  204
#define SEC_ERR                                 205
#define AES_STATUS                              206
#define HW_READY                                207
#define DEBONDED_DEVICE                         208


// OTA Message identifiers
#define PROGRESS_STATUS                         300
#define SET_OTA_MODE_RSP                        301
#define SET_CUR_APP_RSP                         302
#define READ_CUR_APP_RSP                        303
#define CHALLENGE_RESPONSE_STATUS               304
#define READ_VERSION_RESPONSE                   305
#define BT_ADDRESS_READ_FAILED                  306
#define REMOTE_APPLICATION_MODE                 400
#define BT_CURAPP_READ_FAILED                   307
#define BT_CURAPP_NOT_SUPPORTED                 308
#define OTA_READY                               309
#define OTA_MERGE_CSKEY_FAILED                  310

// Remote App Mode
#define NOT_SUPPORTING_OTA                      0x00
#define OTA_BOOTLOADER_MODE                     0x01 
#define OTA_APPLICATION_MODE                    0x02

// OTA Status message identifiers
#define DOWNLOADING                             1
#define DOWNLOAD_COMPLETE                       2
#define ERR_UNABLE_TO_OPEN_FILE                 3
#define ERR_REMOTE_DEVICE_DOESNT_SUPPORT        4
#define ERR_IMAGE_TRANSFER_FAILED               5
#define DOWNLOAD_PAUSED                         6
#define DOWNLOAD_CANCELLED                      7
#define ERR_IMAGE_TRANSFER_ABORTED              8

// Challenge-response status
#define CHALLENGE_RESPONSE_FAILED               0
#define CHALLENGE_RESPONSE_SUCCESS              1

/*! The attribute cannot be read */
#define ATT_RESULT_READ_NOT_PERMITTED           0x0002
/*! The attribute cannot be written */
#define ATT_RESULT_WRITE_NOT_PERMITTED          0x0003
/*! The attribute requires an authentication before it can be read or
    written */
#define ATT_RESULT_INSUFFICIENT_AUTHENTICATION  0x0005
/*! The attribute requires encryption before it can be read or written */
#define ATT_RESULT_INSUFFICIENT_ENCRYPTION      0x000f

//Bluetooth Address types
#define DEVICE_TYPE_PUBLIC 0x00
#define DEVICE_TYPE_RANDOM 0x01

/* Supplier ID used by Synergy to denote GATT protocol - csr_bt_result.h */
#define CSR_BT_SUPPLIER_ATT     0x002D

/* This error is returned when an update is aborted because the image is too
 * big.
 */
#define CSR_OTA_IMAGE_TOO_BIG   0x81

/* This error is returned when an update fails because the image was corrupt */
#define CSR_OTA_IMAGE_CORRUPT   0x82

/* This error is returned when an update fails because the battery voltage is too low */
#define CSR_OTA_BATTERY_LOW     0x83

typedef struct _CSR_BLE_BT_ADDRESS {
    BYTE                        type;  /* Type of Bluetooth address (Public or random) */
    WORD                        nAp;   /**< Non-significant part of Bluetooth address [47..32] */
    BYTE                        uAp;   /**< Upper part of Bluetooth address [31..24] */
    DWORD                       lAp;   /**< Lower part of Bluetooth address [23..0] */
} CSR_BLE_BT_ADDRESS;

// The structure of the data sent as part of the SEARCH_RESULT message
typedef struct 
{
	CString DeviceName;
	CSR_BLE_BT_ADDRESS DevAddr;
} SEARCH_RESULT_T;

// The structure of the data sent as part of a PROGRESS_STATUS message
typedef struct 
{
	int status;
	long progress;
} PROGRESS_STATUS_T;

// All messages into the UI have this structure
typedef struct 
{
	int MsgId;
	LPVOID MsgData;
} UPDATE_UI_MSG_T;

typedef enum MSG_TYPE_T
{
   INFO_TYPE,
   ERROR_TYPE,
   WARN_TYPE,
   STATUS_TYPE,
};

// Logging messages have this structure
typedef struct 
{
   MSG_TYPE_T msgType;
	CString logMessage;
} LOGGING_MSG_T;

#endif /* __MESSAGE_DEFS__*/