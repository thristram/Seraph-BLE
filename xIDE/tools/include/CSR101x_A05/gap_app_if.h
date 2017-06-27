/*! \file gap_app_if.h
 *  \brief  Generic Access Profile interface for Applications
 *
 * Copyright (c) 2010 - 2013 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __GAP_APP_IF_H__
#define __GAP_APP_IF_H__

/*============================================================================*
Header Files
*============================================================================*/
#include "types.h"
#include "bluetooth.h"
#include "ls_err.h"
#include "ls_app_if.h"
#include "gap_types.h"


/*============================================================================*
Public Definitions
*============================================================================*/

/*! \addtogroup GAP
 * @{
 */
/*! \name Discoverability Timeouts
 *
 *  \brief GAP spec. sections 4.1 and 16
 *
 * @{
 */

/*! \brief 10.24s - time to perform device discovery */
#define GAP_TGAP_100                    10240000U
/*! \brief 10.625ms - time to be discoverable */
#define GAP_TGAP_101                       10625U
/*! \brief 2.56s - time between being discoverable */
#define GAP_TGAP_102                     2560000U
/*! \brief 30.72s - min. time to be discoverable */
#define GAP_TGAP_103                    30720000U
/*! \brief 60.00s - max. time to be discoverable */
#define GAP_TGAP_104                    60000000U
/*! \brief 30.72s - limited discovery max. advertise time */
#define GAP_TGAP_lim_disc_adv_max       30720000U
/*! \brief 250ms - limited discovery min. advertising interval */
#define GAP_TGAP_lim_disc_adv_intvl_min   250000U
/*! \brief 500ms - limited discovery max. advertising interval */
#define GAP_TGAP_lim_disc_adv_intvl_max   500000U
/*! \brief 1.28s - general discovery min. advertising interval */
#define GAP_TGAP_gen_disc_adv_intvl_min  1280000U
/*! \brief 2.56s - general discovery min. advertising interval */
#define GAP_TGAP_gen_disc_adv_intvl_max  2560000U

/* @} */

/*! \name Scan Timeouts
 *
 * See Bluetooth Specification v4 Volume 6 Part B section 4.4.3
 *
 * Note - scan interval and scan window share the same start time, so the
 *   two being equal means continuous scanning.
 *
 * @{
 */

/*! \brief 10.24s - limited discovery min. time to perform scan */
#define GAP_TGAP_lim_disc_scan_min      10240000U
/*! \brief 11.25ms - limited discovery scan interval */
#define GAP_TGAP_lim_disc_scan_intvl       11250U
/*! \brief 11.25ms - limited discovery scan duration */
#define GAP_TGAP_lim_disc_scan_window      11250U
/*! \brief 10.24s - general discovery min. time to perform scan */
#define GAP_TGAP_gen_disc_scan_min      10240000U
/*! \brief 11.25ms - general discovery scan interval */
#define GAP_TGAP_gen_disc_scan_intvl       11250U
/*! \brief 11.25ms - general discovery scan duration */
#define GAP_TGAP_gen_disc_scan_window      11250U

/* @} */


/*============================================================================*
Public Data Types
*============================================================================*/


/*============================================================================*
Public Data Declarations
*============================================================================*/


/*============================================================================*
Public Function Prototypes
*============================================================================*/


/*----------------------------------------------------------------------------*
 *  GapSetMode
 */
/*! \brief Set the GAP modes.
 *
 *  The \c discover parameter is used to select both the Discoverability mode
 *  (when operating in the Peripheral role) and the Discovery Procedure (when
 *  operating in the Central role). An Observer does not use the \c discover
 *  parameter and will ignore the setting. A Broadcaster is not allowed to be
 *  discoverable, therefore with this role \c discover must always be set to
 *  \ref gap_mode_discover_no.
 *
 *  The \c connect parameter is only used when the role is Peripheral. In other
 *  roles it must be set to \ref gap_mode_connect_no.
 *
 *  The bonding and security flags are used for connections, therefore apply to
 *  Central and Peripheral roles. In other roles they are ignored.
 *
 *  Calling this function will reset the scan type to \ref ls_scan_type_active
 *  and the advertising channel mask to 0x07 (i.e. all channels available).
 *
 *  \param  role  The GAP operational mode to use.
 *  \param  discover  The GAP discovery mode to use.
 *  \param  connect  The GAP connection mode to use.
 *  \param  bond  The GAP bonding mode to use.
 *  \param  security  The GAP security mode to use.
 *
 *  \returns  ls_err_none for success, or an error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetMode(gap_role const role,
                         gap_mode_discover const discover,
                         gap_mode_connect const connect,
                         gap_mode_bond const bond,
                         gap_mode_security const security);


 /*----------------------------------------------------------------------------*
  *  GapGetRandomAddress
  */
 /*! \brief Return the current random address for this device.
  *
  *  If the application has not previously set a random address then the address
  *  returned by this function is the default static address, which is set on
  *  power-up. A random address is set/changed when the application calls
  *  GapSetRandomAddress(), GapSetStaticAddress(), or SMPrivacyRegenerateAddress().
  *
  *  \param  bd_addr  The (untyped) Bluetooth address to use as the device's
  *  					random address.
  *
  *  \returns  ls_err_none on success, or ls_err_arg if the address is
  *      unacceptable for some reason.
  */
 /*---------------------------------------------------------------------------*/
extern ls_err GapGetRandomAddress(BD_ADDR_T* bd_addr);


/*----------------------------------------------------------------------------*
 *  GapSetRandomAddress
 */
/*! \brief Set the random address for this device.
 *
 *      The random address may subsequently be used in scan/advertise.
 *
 *  \param  ra  The Bluetooth address to use as the device's random
 *      address.
 *
 *  \returns  ls_err_none on success, or ls_err_arg if the address is
 *      unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetRandomAddress(const BD_ADDR_T *ra);


/*----------------------------------------------------------------------------*
 *  GapSetStaticAddress
 */
/*! \brief Set the static address for this device.
 *
 *   The static address is set at manufacture and may be used in scan or
 *   advertise if a Random type address is selected. This function should be
 *   called after GapSetRandomAddress() or SMPrivacyRegenerateAddress() to
 *   return to using the static address
 *
 */
/*---------------------------------------------------------------------------*/
extern void GapSetStaticAddress( void );

/*----------------------------------------------------------------------------*
 *  GapSetAdvAddress
 */
/*! \brief Set the advertising direct address (i.e. the target peer)
 *
 *      See also Bluetooth Specification v4 Volume 2 Part E section 7.8.5
 *
 *  \param  direct_addr  The Bluetooth address to use in directed
 *     advertising.
 *
 *  \returns  ls_err_none on success, or ls_err_arg if the address is
 *     unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetAdvAddress(TYPED_BD_ADDR_T const *direct_addr);

/*----------------------------------------------------------------------------*
 *  GapSetScanInterval
 */
/*! \brief Set the scan interval and scan window times in microseconds.
 *
 *      Range: 2.5 msec (2500) to 10240 msec (10240000)
 *
 *      See also Bluetooth Specification v4 Volume 2 Part E section 7.8.10
 *
 *  \param  interval_us  The scan interval in microseconds.
 *  \param  window_us  The scan window in microseconds.
 *
 *  \returns  ls_err_none on success, or ls_err_arg if the parameters
 *      are unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetScanInterval(
    uint32 const interval_us,
    uint32 const window_us
    );

/*----------------------------------------------------------------------------*
 *  GapSetAdvInterval
 */
/*! \brief Set the advertising interval min & max times in microseconds.
 *
 *  This function is called by the application to store the advertising
 *  interval min and max times in microseconds. Valid ranges for the parameters
 *  are 20ms (20000) to 10240ms (10240000). The maximum interval must be greater
 *  than the minimum interval.
 *
 *  The firmware is free to use any advertising interval between the minimum
 *  and maximum values. The current implementation will always use the maximum
 *  value, to save power by transmitting as little as possible. However, future
 *  releases of the firmware library may use a different value if it improves
 *  advertising performance when one or more connections are also active.
 *
 *  \param adv_min_us Minimum advertising interval requested by application.
 *  \param adv_max_us Maximum advertising interval requested by application.
 *
 *  \return  ls_err_none on success, or ls_err_arg if the intervals
 *      are unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetAdvInterval(uint32 const adv_min_us,
                                uint32 const adv_max_us);

/*----------------------------------------------------------------------------*
 *  GapSetScanType
 */
/*! \brief Set the scan type. The firmware defaults to passive scanning.
 *
 *  See also Bluetooth Specification v4 Volume 2 Part E section 7.8.10
 *
 *  \param  st  The type of scan to perform when scanning.
 *
 *  \returns  Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void GapSetScanType(ls_scan_type const st);

/*----------------------------------------------------------------------------*
 *  GapSetAdvChanMask
 */
/*! \brief Set the advertising channel mask.
 *
 *      See also Bluetooth Specification v4 Volume 2 Part E section 7.8.5
 *
 *  \param  mask  a bit-mask of the advertising channels to use.
 *      Valid values are from 0 to 7.
 *
 *  \returns  ls_err_none on success, or ls_err_arg if the address is
 *     unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapSetAdvChanMask(uint8 const mask);

/*----------------------------------------------------------------------------*
 *  GapSetConnChanMask
 */
/*! \brief Set or update the connection channel mask, used when the device is master of the connection.
 *
 *      See also Bluetooth Specification v4.0 Volume 2 Part E section 7.8.19
 *  The Channel map is a 5 octet array, where the least significant bit of
 *  the first octet corresponds to the first channel. The fourth most significant
 *  bit of the fifth octet corresponds to the last data channel. The three most
 *  significant bits of the fifth octet are cleared to restrict the mask to
 *  data channels.
 *
 *  \param channel_map
 *      Pointer to uint8 array containing the bit-mask of channels to use when
 *      operating as master. Must have at least 2 bits set.
 *
 *  \returns  ls_err_none on success, or ls_err_arg if the mask is
 *     unacceptable for some reason.
 */
/*---------------------------------------------------------------------------*/
ls_err GapSetConnChanMask(const uint8 *channel_map);

/*----------------------------------------------------------------------------*
 *      GapGetConnChanMask
 */
/*! \brief Reads the connection channel mask, for the specified connection.
 *
 *      See also Bluetooth Specification v4.0 Volume 2 Part E section 7.8.20, but
 *      note that this function is passed a GATT connection ID, not an HCI
 *      connection handle.
 *  The Channel map is a 5 octet array, where the least significant bit of
 *  the first octet corresponds to the first channel. The fourth most significant
 *  bit of the fifth octet corresponds to the last data channel.

 *  \param cid
 *      GATT connection id
 *  \param channel_map
 *      Pointer to uint8 array which will be filled with the
 *      connection channel map.
 *
 *  \returns  ls_err_none on success
 */
ls_err GapGetConnChanMask(uint16 cid, uint8 *channel_map);


/*----------------------------------------------------------------------------*
 *      GapLtkAvailable
 */
/*! \brief Indicate if a LTK is available for the connection.
 *
 *  \details When the application receives the GATT_CONNECT_CFM message,
 *           it can call this function to indicate that a LTK is available for the
 *           connection (i.e a bond exists between the devices).
             This information is used to respond with status code 'insufficient
 *           encryption' when the connection is unencrypted and a service
 *           request is issued to an attribute that requires the link to be
 *           encrypted.
 *           If the bond is ever removed while in a connection, the new function 
 *           should be called again to indicate that the bond no longer exists unless
 *           the link is disconnected.
 *
 *  \param bd_addr
 *      Peer address used as an index for this data.
 *
 *  \param ltk_available
 *      Whether a LTK is available for this connection.
 *
 *  \return  \ref ls_err_none for success, or an error code on failure.
 */
/*---------------------------------------------------------------------------*/
extern ls_err GapLtkAvailable(const TYPED_BD_ADDR_T *bd_addr, bool ltk_available);

/*! @} */

#endif
