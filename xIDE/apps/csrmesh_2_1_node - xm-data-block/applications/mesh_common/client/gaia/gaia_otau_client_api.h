/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_otau_client_api.h
 * \brief GAIA OTAu Public API
 *
 */
#ifndef GAIA_OTAU_CLIENT_API_H_
#define GAIA_OTAU_CLIENT_API_H_

#ifdef GAIA_OTAU_RELAY_SUPPORT

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <store_update_msg.h>
#include "gaia_otau_api.h"

/*=============================================================================*
 *  Public Data Types
 *============================================================================*/
/* This structure defines the states used for GAIA Client OTAu */
typedef enum
{
	STATE_VM_UPGRADE_IDLE,
    STATE_VM_UPGRADE_CONNECT,
	STATE_VM_UPGRADE_SYNC_REQ,
	STATE_VM_UPGRADE_START_REQ,
	STATE_VM_UPGRADE_DATA_REQ,
	STATE_VM_UPGRADE_TRANSFER,
	STATE_VM_UPGRADE_WAIT_VALIDATION,
    STATE_VM_UPGRADE_WAIT_VALIDATION_RSP,
    STATE_VM_UPGRADE_WAIT_POST_TRANSFER_DISCONNECT,
    STATE_VM_UPGRADE_WAIT_POST_TRANSFER_WAIT_DISCONNECT,
    STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION_DELAY,
	STATE_VM_UPGRADE_WAIT_POST_TRANSFER_RECONNECTION,
	STATE_VM_UPGRADE_START_REQ_AFTER_REBOOT,
    STATE_VM_UPGRADE_IN_PROGRESS_REQ,
	STATE_VM_UPGRADE_COMMIT_REQ,
    STATE_VM_UPGRADE_DISCONNECT,
    STATE_VM_UPGRADE_COMPLETED,
    STATE_VM_UPGRADE_ABORTING_DISCONNECT,
} GAIA_VMUPGRADE_STATE;

/*!
 * \brief GAIA OTAu Client event notifications that can be sent to the application.
 *
 * An event handler callback function must be registered before events will
 * be received. See \link GaiaOtauClientRegisterCallback \endlink
 */
typedef enum
{
    gaia_otau_client_event_upgrade_starting,       /*!< \brief Start of an upgrade. */
} gaia_otau_client_event;

/*!
 * \brief Event data sent with \link gaia_otau_client_event_upgrade_starting \endlink
 *
 * Sent when the host makes the first indication that an upgrade is going to occur.
 * The application has the opportunity to delay the upgrade process at this point.
 */
typedef struct
{

} GAIA_OTAU_CLIENT_EVENT_UPGRADE_STARTING_T;

/*!
 * \brief Union of all possible event data types.
 *
 * Event specific data may be accessed via the corresponding member according
 * to the event type (see \link gaia_otau_client_event \endlink ).
 */
typedef union
{
    GAIA_OTAU_CLIENT_EVENT_UPGRADE_STARTING_T      upgrade_starting;       /*!< Data for gaia_otau_event_upgrade_starting */
} GAIA_OTAU_CLIENT_EVENT_T;

/*!
 * \brief Prototype for a GAIA OTAu Client event handler callback function.
 *
 * Called by the OTAu library to notify the application of various events, see
 * \link gaia_otau_client_event \endlink for a list of all possible events.
 *
 * \param[in] event The type of event being notified.
 * \param[in,out] data Pointer to the event data associated with the event.
 *
 * \return sys_status - The application should return sys_status_success if the
                        event was handled, and sys_status_failure otherwise, in
                        which case any expected return data will be ignored.
 */
typedef sys_status (*gaia_otau_client_event_handler)(gaia_otau_client_event event, GAIA_OTAU_CLIENT_EVENT_T *data);

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

extern void StorePartitionSignatureToNVM(uint8* p_sig_data);
extern void StorePartitionDataToNVM(GAIA_OTAU_EVENT_PARTITION_INFO_T* p_part_data);
extern void StoreHeaderDataToNVM(uint8* p_hdr_data);
extern void StoreRelayStoreInfo(GAIA_OTAU_EVENT_PARTITION_INFO_T *p_store_info);
extern void SendLOTAnnouncePacket(void);
extern void StartUpgrade(void);
extern void GaiaOtauClientConfigStoreMsg(msg_t *msg);
extern void GaiaOtauClientHandleStoreUpdateMsg(device_handle_id device_id, store_update_msg_t *msg);
extern uint16 GaiaOtauClientGetState(void);
extern void GaiaOtauClientResetTransferState(void);
extern void GaiaOtauSetCommitStatus(bool status);
extern void GaiaOtauSetRelayStore(bool commit_successful);
extern void GaiaOtauClientRegisterCallback(gaia_otau_client_event_handler callback);

#endif
#endif /* GAIA_OTAU_CLIENT_API_H_ */
