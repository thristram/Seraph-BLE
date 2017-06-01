/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      scan_handler.h
 *
 *  DESCRIPTION
 *      Header file for scanning related functions
 *
 *
 ******************************************************************************/

#ifndef _SCAN_HANDLER_H
#define _SCAN_HANDLER_H

#ifdef GAIA_OTAU_RELAY_SUPPORT
/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function starts scanning */
extern void StartScanning(uint8* lot_service_id);

/* This function stops scanning */
extern void StopScanning(bool delete_only_timer);
#endif
#endif /* _SCAN_HANDLER_H */
