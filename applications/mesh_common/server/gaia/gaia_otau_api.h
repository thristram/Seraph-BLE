/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
/*! \file gaia_otau_api.h
 * \brief GAIA OTAu Public API
 *
 */
#ifndef GAIA_OTAU_API_H_
#define GAIA_OTAU_API_H_


/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <store_update_msg.h>
#include <cm_types.h>

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/
/*!
 * \brief Partition Data subsection marker for upgrade files
 *
 * The GAIA OTAu upgrade file parser will look for this marker to determine the
 * type of the section
 * This is exported so applications can act as both an upgrade host and client
 */
#define UPGRADE_PARTITION_ID                                    "PARTDATA"

/*!
 * \brief Footer subsection marker for upgrade files
 *
 * The GAIA OTAu upgrade file parser will look for this marker to determine the
 * type of the section
 * This is exported so applications can act as both an upgrade host and client
 */
#define UPGRADE_FOOTER_ID                                       "APPUPFTR"

/*=============================================================================*
 *  Public Data Types
 *============================================================================*/
/*!
 * \brief GAIA OTAu event notifications that can be sent to the application.
 *
 * An event handler callback function must be registered before events will
 * be received. See \link GaiaOtauRegisterCallback \endlink
 */
typedef enum
{
    gaia_otau_event_upgrade_starting,       /*!< \brief Start of an upgrade. \link see GAIA_OTAU_EVENT_UPGRADE_STARTING_T \endlink   */
    gaia_otau_event_validate_header,        /*!< \brief The first 8 ASCII characters of the header require validation. \link see GAIA_OTAU_EVENT_VALIDATE_HEADER_T \endlink*/
    gaia_otau_event_validate_header_body,   /*!< \brief The body of the header requires validation. \link see GAIA_OTAU_EVENT_VALIDATE_HEADER_BODY_T \endlink */
    gaia_otau_event_partition_info,         /*!< \brief Information about the current partition. \link see GAIA_OTAU_EVENT_PARTITION_INFO_T \endlink */
    gaia_otau_event_reboot_warning,         /*!< \brief A reboot is about to occur. \link see GAIA_OTAU_EVENT_REBOOT_WARNING_T \endlink */
    gaia_otau_event_upgraded_application,   /*!< \brief This application is the upgraded but non-committed version. \link see GAIA_OTAU_EVENT_UPGRADED_APPLICATION_T \endlink */
    gaia_otau_event_new_app_commit,         /*!< \brief The application that is running is about to be committed. \link see GAIA_OTAU_EVENT_NEW_APP_COMMIT_T \endlink */
    gaia_otau_event_upgrade_failed          /*!< \brief The upgrade has failed. \link see GAIA_OTAU_EVENT_UPGRADE_FAILED_T \endlink */
} gaia_otau_event;

/*!
 * \brief Event data sent with \link gaia_otau_event_upgrade_starting \endlink
 *
 * Sent when the host makes the first indication that an upgrade is going to occur.
 * The application has the opportunity to delay the upgrade process at this point.
 */
typedef struct
{
    /*!
     * The application may change this value to delay the start of the upgrade
     * process. Defaults to TRUE, in which case the upgrade will start as soon
     * as the event handler function returns. If changed to FALSE then the OTAu
     * library will wait for a call to \link GaiaOtauContinueUpgrade \endlink
     * before continuing. This allows the application to, for example, change
     * the connection parameters for a faster download.
     */
    bool continue_immediately;

} GAIA_OTAU_EVENT_UPGRADE_STARTING_T;

/*!
 * \brief Event data sent with \link gaia_otau_event_validate_header \endlink
 *
 * Sent when the OTAu library requires the application to validate the first 8
 * ASCII characters of the header.
 */
typedef struct
{
    /*!
     * \brief Defaults to TRUE, but the application may change this value to
     * FALSE to indicate that the header has failed validation, in which case
     * the upgrade will be aborted.
     */
    bool header_valid;

    uint8 *header;  /*!< Pointer to the start of the header string (length 8) */

} GAIA_OTAU_EVENT_VALIDATE_HEADER_T;

/*!
 * \brief Event data sent with \link gaia_otau_event_validate_header_body \endlink
 *
 * Sent when the OTAu library requires the application to validate a portion of
 * the body of the header. This event may be triggered multiple times as the
 * host may split the header body up over multiple packets.
 */
typedef struct
{
    /*!
     * \brief Defaults to TRUE meaning that the header body, so far, has been
     * correct. The application may change this value to FALSE at any time to
     * indicate that the header body has failed validation, in which case the
     * upgrade will be aborted.
     */
    bool header_body_valid;

    uint32 header_body_size;    /*!< \brief Total size of the header body */
    uint8 portion_size;         /*!< \brief Size of this header body portion */
    uint8 *header_body;         /*!< \brief Portion of the header body */

} GAIA_OTAU_EVENT_VALIDATE_HEADER_BODY_T;


/*!
 * \brief Event data sent with \link gaia_otau_event_partition_info \endlink
 *
 * Sent when the OTAu library starts writing a new partition to the Flash.
 */
typedef struct
{
    uint16 partition_type;      /*!< \brief Partition type */
    uint16 partition_id;        /*!< \brief Partition ID */
    uint32 partition_size;      /*!< \brief Number of octets in this partition */
} GAIA_OTAU_EVENT_PARTITION_INFO_T;


/*!
 * \brief Event data sent with \link gaia_otau_event_reboot_warning \endlink
 *
 * Sent when a reboot is about to occur.
 */
typedef struct
{
    /*!
     * \brief Defaults to FALSE, the reboot will not wait
     *  Set to TRUE to stop the reboot until further notice
     */
    bool reboot_wait;

} GAIA_OTAU_EVENT_REBOOT_WARNING_T;

/*!
 * \brief Event data sent with \link gaia_otau_event_upgraded_application \endlink
 *
 * Sent after a reboot (after \link gaia_otau_event_upgrade_starting \endlink)
 * when the running application is the upgraded but non-committed version.
 */
typedef struct
{
} GAIA_OTAU_EVENT_UPGRADED_APPLICATION_T;

/*!
 * \brief Event data sent with \link gaia_otau_event_new_app_commit \endlink
 *
 * The current application will become the default application from this point
 * forward. The CS key for the application ID will be updated.
 */
typedef struct
{
} GAIA_OTAU_EVENT_NEW_APP_COMMIT_T;

/*!
 * \brief Event data sent with \link gaia_otau_event_upgrade_failed \endlink
 *
 * Sent when the upgrade fails or is aborted.
 */
typedef struct
{
} GAIA_OTAU_EVENT_UPGRADE_FAILED_T;

/*!
 * \brief Union of all possible event data types.
 *
 * Event specific data may be accessed via the corresponding member according
 * to the event type (see \link gaia_otau_event \endlink ).
 */
typedef union
{
    GAIA_OTAU_EVENT_UPGRADE_STARTING_T      upgrade_starting;       /*!< Data for gaia_otau_event_upgrade_starting */
    GAIA_OTAU_EVENT_VALIDATE_HEADER_T       validate_header;        /*!< Data for gaia_otau_event_validate_header */
    GAIA_OTAU_EVENT_VALIDATE_HEADER_BODY_T  validate_header_body;   /*!< Data for gaia_otau_event_validate_header_body */
    GAIA_OTAU_EVENT_PARTITION_INFO_T        partition_info;         /*!< Data for gaia_otau_event_partition_info */
    GAIA_OTAU_EVENT_REBOOT_WARNING_T        reboot_warning;         /*!< Data for gaia_otau_event_reboot_warning */
    GAIA_OTAU_EVENT_UPGRADED_APPLICATION_T  upgraded_application;   /*!< Data for gaia_otau_event_upgraded_application */
    GAIA_OTAU_EVENT_NEW_APP_COMMIT_T        new_app_commit;         /*!< Data for gaia_otau_event_new_app_commit */
    GAIA_OTAU_EVENT_UPGRADE_FAILED_T        upgrade_failed;         /*!< Data for gaia_otau_event_upgrade_failed */

} GAIA_OTAU_EVENT_T;

/*!
 * \brief Prototype for a GAIA OTAu event handler callback function.
 *
 * Called by the OTAu library to notify the application of various events, see
 * \link gaia_otau_event \endlink for a list of all possible events.
 *
 * \param[in] event The type of event being notified.
 * \param[in,out] data Pointer to the event data associated with the event.
 *
 * \return sys_status - The application should return sys_status_success if the
                        event was handled, and sys_status_failure otherwise, in
                        which case any expected return data will be ignored.
 */
typedef sys_status (*gaia_otau_event_handler)(gaia_otau_event event, GAIA_OTAU_EVENT_T *data);


/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*! \brief Handle Config Store messages
 *
 *  All messages of type CONFIG_STORE_GROUP_ID should be forwarded to this
 *  function
 *
 * \param[in]   msg     config store message from the firmware
 *
 */
extern void GaiaOtauConfigStoreMsg(msg_t *msg);

/*! \brief Handle Store Update messages
 *
 *  All messages of type STORE_UPDATE_GROUP_ID should be forwarded to this
 *  function
 *
 * \param[in]   msg     config store message from the firmware
 *
 */
extern void GaiaOtauHandleStoreUpdateMsg(device_handle_id device_id, store_update_msg_t *msg);

/*! \brief Register callback function to handle OTAu events
 *
 * See \link gaia_otau_event_handler \endlink
 *
 * \param[in] callback Pointer to application function that will handle OTAu events
 *
 */
extern void GaiaOtauRegisterCallback(gaia_otau_event_handler callback);

/*! \brief Request an OTAu progress update
 *
 * This function will return, via the arguments, the total size of the
 * partition, and the amount of the partition that has already been transferred.
 *
 * \param[out]   partition_size      total size of the partition being transferred, in bytes
 * \param[out]   partition_progress  amount of the partition that has been transfressed, in bytes
 *
 */
extern void GaiaOtauUpgradeProgress( uint32 *partition_size, uint32 *partition_progress );

/*! \brief Continue with the upgrade process after waiting for the application
 *
 * After certain callbacks the application may request to wait before continuing with the
 * upgrade, for example at the start of an upgrade it may want to send a connection parameter
 * update. This function should be called after such events have completed to allow the rest
 * of the upgrade process to continue.
 *
 */
extern void GaiaOtauContinueUpgrade(void);

#endif /* GAIA_OTAU_API_H_ */
