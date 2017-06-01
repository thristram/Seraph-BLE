/******************************************************************************
 *  Copyright 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      gaia_client_service_event.h
 *
 *  DESCRIPTION
 *      Header file for the GAIA Client event functions
 *
 *  NOTES
 *
 ******************************************************************************/

#ifndef _GAIA_CLIENT_SERVICE_EVENT_H
#define _GAIA_CLIENT_SERVICE_EVENT_H

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*============================================================================*
 *  Public Data Types
 *============================================================================*/
/*! \brief A node wanting to provide a large object to neighbouring Mesh Nodes issues an ANNOUNCE with the associated content type. This message will have TTL=0, thus will only be answered by its immediate neighbours. The ANNOUNCE has the total size of the packet to be issued. The format and encoding of the large object is subject to the provided type and is out of scope of this document. The destination ID can either be 0, a group or a specific Device ID. In case the destination ID is not zero, only members of the group (associated with the LOT model) or the device with the specified Device ID responds with the intent to download the object for their own consumption. Every other node either ignores or accepts the offer for the purpose of relaying the packet. */
typedef struct
{
    uint16 companycode; /*!< \brief Bluetooth Company Code */
    uint8 platformtype; /*!< \brief Platform this object is intended for (e.g. CSR1010, CSR1020) */
    uint8 typeencoding; /*!< \brief Type description of intended payload (e.g. firmware, application) */
    uint8 imagetype; /*!< \brief Type of image (e.g. Light, Beacon) */
    uint8 size; /*!< \brief Number of kilobytes in the Large Object (0 = < 1K bytes, 1 = >=1K bytes, < 2K bytes, and so on) */
    uint16 objectversion; /*!< \brief Object Version (e.g. Firmware version x.y.z). A node can use this to decide if it already has this version of the object. Major version is 6 bits. (MSB), Minor version is 4 bits, Revision is 6 bits (LSB). */
    uint16 targetdestination; /*!< \brief Destination of the Large Object */
} LARGEOBJECTTRANSFER_ANNOUNCE_T;

/*============================================================================*
 *  Public Data
 *============================================================================*/
bool scanning_ongoing;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* The function sends the LOT model announce packet */
extern void LotModelSendAnnounce(LARGEOBJECTTRANSFER_ANNOUNCE_T *data);


extern bool IsAppAssociated(void);
#endif

#endif /* _GAIA_CLIENT_SERVICE_EVENT_H */
