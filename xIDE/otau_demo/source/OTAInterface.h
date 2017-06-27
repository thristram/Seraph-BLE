/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK
 *
 *  FILE
 *      OTAINterface.h
 *
 *  DESCRIPTION
 *      This class handles  is used for OTA Interface for the Updater application
 *
 ******************************************************************************/

#ifndef _OTA_INTERFACE_H
#define _OTA_INTERFACE_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include "stdafx.h"

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "BleRadio.h"
#include "uEnergyImage.h"
#include "MessageDefs.h"
#include "BLEInterface.h"
#include "BLESupport.h"
#include "uEnergyImage.h"
#include "cskeysinfo.h"

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/* This type defines the action requests that can be made of this class by
 * other system modules.
 */
typedef enum 
{
    ACTION_START_SCANNING,
    ACTION_CANCEL_SCANNING,
    ACTION_CONNECT_TOGGLE,
    ACTION_CANCEL_CONNECTING,
    ACTION_GET_APP_ID,
    ACTION_SET_APP_ID,
    ACTION_EXPECT_CHALLENGE
} ACTION;

/* This type defines the transfer-control commands understood by the OTAU boot-loader. */
typedef enum
{
    CMD_SET_TRANSFER_CONTROL    = 2,
    CMD_SET_TRANSFER_PAUSED     = 3,
    CMD_SET_TRANSFER_COMPLETED  = 4,
    CMD_SET_TRANSFER_FAILED     = 5,
    CMD_ABORT_DATA_TRANSFER     = 6,
} TRANSFER_CONTROL;

/* This type defines the CS-key identifiers, used when reading CS-key data from the
 * connected device.
 */
typedef enum
{
    KEY_BT_ADDRESS              = 1,
    KEY_XTAL_TRIM               = 2,
    KEY_IDTY_ROOT               = 17,
    KEY_ENCR_ROOT               = 18,

} KEY_IDENTIFIER;

/* This type defines a function return code used to indicate the status of the
 * read/write operation.
 */
typedef enum
{
    GATT_OP_READ_PENDING,
    GATT_OP_READ_COMPLETE,
    GATT_OP_WRITE_PENDING,
    GATT_OP_WRITE_COMPLETE
} GATT_OP_STATUS;



/*============================================================================*
 *  Public Class Definition
 *============================================================================*/

// The OTA module internal state
typedef enum
{
   // Nothing doing
   STATE_OTA_IDLE = 0x0,           

   // Indicates that the current state is part of the connection initialisation
   STATE_OTA_INIT_MASK = 0x80,

   // Indicates that the current state is part of the data transfer
   STATE_OTA_DTRANS_MASK = 0x4,

   // Read the GATT database from the device
   STATE_OTA_INIT_READ_DATABASE   = (0x1 | STATE_OTA_INIT_MASK),
   // Read the specification version from the device
   STATE_OTA_INIT_READ_VERSION    = (0x2 | STATE_OTA_INIT_MASK),
   // Requesting notifications on all relevant characteristics
   STATE_OTA_INIT_CONFIGURING_CCD = (0x3 | STATE_OTA_INIT_MASK),
   // Reading the device BT address
   STATE_OTA_INIT_READ_BT_ADDRESS = (0x4 | STATE_OTA_INIT_MASK),
   // Reading the device crystal trim
   STATE_OTA_INIT_READ_XTAL_TRIM  = (0x5 | STATE_OTA_INIT_MASK),
     // Reading the identity root 
   STATE_OTA_INIT_READ_IDENTITY_ROOT  = (0x6 | STATE_OTA_INIT_MASK),
  // Reading the encryption root
   STATE_OTA_INIT_READ_ENCRYPTION_ROOT  = (0x7 | STATE_OTA_INIT_MASK),
   // Read the challenge value from the connected device
   STATE_OTA_INIT_READ_CHALLENGE  = (0x8 | STATE_OTA_INIT_MASK),
   // Reading the active application
   STATE_OTA_INIT_READ_CURRENT_APP= (0x9 | STATE_OTA_INIT_MASK),
   // Setting the active application
   STATE_OTA_INIT_SET_CURRENT_APP = (0xa | STATE_OTA_INIT_MASK),
   // Reading the CS block
   STATE_OTA_INIT_READ_CS_BLOCK = (0xb | STATE_OTA_INIT_MASK),
   // Reading the CS block bd address using offset
   STATE_OTA_INIT_READ_CS_BLOCK_BD_ADDR = (0xc | STATE_OTA_INIT_MASK),
   // Reading the CS block crystal trim using offset
   STATE_OTA_INIT_READ_CS_BLOCK_CRYSTRIM = (0xd | STATE_OTA_INIT_MASK),
   // Reading the CS block crystal trim using offset
   STATE_OTA_INIT_READ_CS_BLOCK_IDROOT = (0xe | STATE_OTA_INIT_MASK),
   // Reading the CS block crystal trim using offset
   STATE_OTA_INIT_READ_CS_BLOCK_ENCRROOT = (0xf | STATE_OTA_INIT_MASK),

   // Image transfer in progress
   STATE_OTA_DATA_TRANSFER        = (0x1 | STATE_OTA_DTRANS_MASK),
   // Image transfer paused
   STATE_OTA_PAUSE_DATA_TRANSFER  = (0x2 | STATE_OTA_DTRANS_MASK),
   // Image transfer reset
   STATE_OTA_RESET_TRANSFER_CTRL  = (0x3 | STATE_OTA_DTRANS_MASK),
   // Image transfer aborted
   STATE_OTA_ABORT_DATA_TRANSFER  = (0x4 | STATE_OTA_DTRANS_MASK),

   // Switching connected device into the boot-loader
   STATE_OTA_SET_MODE             = 0x10,
   // Setting the active application
   STATE_OTA_SET_CURRENT_APP      = 0x11,
   // Reading the active application
   STATE_OTA_GET_CURRENT_APP      = 0x12,
   // Write the challenge response to the connected device
   STATE_OTA_WRITE_RESPONSE       = 0x13,
   // Setting transfer control state to "in progress"
   STATE_OTA_SET_TRANSFER_CTRL    = 0x14
} OTA_STATE;
    
   
class COTAInterface : public CBLEInterface
{
public:

    /*============================================================================*
     *  Public Function Prototypes
     *============================================================================*/

   COTAInterface(){}
    /* The class destructor */
    ~COTAInterface();

    /* This function initialises the underlying radio dongle */
    RADIO_FAILURES BleInit(HWND hWnd, UINT& bleClientHandle, BOOL bLogging);
    
   void OnBleReady(WPARAM wParam, LPARAM lParam);

    /* Process a message from the uEnergy DLL */
    void BleProcessMessage(WPARAM wParam, LPARAM lParam);

    /* Cancel any outstanding transaction */
    void BleCancelCurrent();

    /* Handle the response to an "abort download" instruction */
    void OtaAbortUpdateRsp(int result);

    /* Send a "transfer complete" message to the remote device */
    void OtaSetTransferComplete();

    /* Send a "starting transfer now" message to the remote device */
    void OtaSetTransferControlReq();

    /* Handle the "starting transfer now" response from the remote device */
    void OtaSetTransferControlRsp(int result);
    /* Sends the next 20 bytes of data */
    bool SendNextImageChunk();
    /* Handles the packet transfer result */
    void OnOtaImagePacketTransferResult(WORD result);
    /* Returns the supported OTA mode */
    WORD CheckDeviceOTAMode();

    /* Handles all BLE notifications */
    void OnBleNotificationReceived(WPARAM wParam, LPARAM lParam);
    /* Handles BLE Write result */
    BOOL OnBleWriteCharResult(WPARAM wParam, LPARAM lParam);
    /* Handles BLE Read result */
    BOOL OnBleReadCharResult(WPARAM wParam, LPARAM lParam);
    /* Sets the device state */
    void SetDeviceState(DeviceState newState);
    /* Configures challenge response */
    void ConfigureChallengeResponse(BOOL enable);

    /* Gets the state of OTA */
    OTA_STATE StateGetOta()
    {
       return otaState;
    }

    /* Sets the image file name */
    bool SetImageFileName(CString fileName);
    /* Opens the image file for update */
    bool OpenImageFileForUpdate();
    /* Closes the image file */
    bool CloseImageFile();

    BOOL ValidateImageFile();

protected:
   /* App Initialize function */
    virtual void AppInit();
    void InitializeOta();
private:

    /*============================================================================*
     *  Private Data
     *============================================================================*/

    /* Information from the user indicating whether the challenge-response
     * authentication is being used by the connected device.
     */
    BOOL mExpectChallenge;

    /* Information from the user identifying the application currently being 
     * updated. 
     */
    BYTE    mAppId;

    /* Information read from the remote device, recording whether we are connected
     * to the device application or to the OTAU boot-loader.
     */
    WORD    mDeviceMode;
    BOOL    mRetryDbRead;
    OTA_STATE   otaState;
    BYTE    m_BootVersion;
    BOOL    m_BLSwitched;

    // Now request the challenge value
    CSR_BLE_UUID m_OtaServiceUuid;
    UUIDCharacteristicPair::iterator m_BleOtaCharsTableIterator;
    BleService    m_BleOtaService;

    ImageFileHandler mImageFileHandler;
    CCsKeysInfo m_KeysInfo;

    /* Application identifiers */
    static const int CSR_OTA_BOOT_OTA   =  0x0;



    /*============================================================================*
     *  Private Function Prototypes
     *============================================================================*/
private:
   /* Gets the Data transfer handle */
   WORD getDataTransHandle();
   /* Gets the current app handle */
   WORD getCurAppHandle();
   /* Gets the Ctrl transfer handle */
   WORD getTransCtrlHandle();
   /* Gets the Ota version handle */
   WORD getOtaVersionHandle();
   /* Gets the read cs key handle */
   WORD getReadCsKeyHandle();
   /* Gets the cs block read handle */
   WORD getReadCsBlockHandle();

   /* Writes the Gett Client char desc value */
   WORD WriteGattClientCharDescValue();
   
   void StartOTA();

   // TRANSFER STATE
   void stateSetOta(OTA_STATE newState)
   {
      otaState = newState;
   }
};


#endif /* _OTA_INTERFACE_H */