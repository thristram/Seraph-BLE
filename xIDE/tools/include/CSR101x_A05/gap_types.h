/*! \file
 *  \brief  Generic Access Profile interface for Applications
 *
 * Copyright (c) 2010 - 2015 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __GAP_TYPES_H__
#define __GAP_TYPES_H__

/*============================================================================*
Header Files
*============================================================================*/


/*============================================================================*
Public Definitions
*============================================================================*/


/*============================================================================*
Public Data Types
*============================================================================*/

/*! \addtogroup GAP
 * @{
 */

/*! \brief  GAP LE operational modes.  See Vol 3 Part C Section 2.2.2 of
 *  the Bluetooth Specification Version 4.
 */
typedef enum
{
    /* Internal default - DO NOT USE. */
    gap_role_null,

    /*! \brief GAP 2.2.2.1
     *
     * A device operating in the Broadcaster role is a device that sends
     *  advertising events as described in Bluetooth Core Spec.
     *  Vol.6, Part B, Section 4.4.2.
     */
    gap_role_broadcaster,

    /*! \brief GAP 2.2.2.2
     *
     * A device operating in the Observer role is a device that receives
     * advertising events as described in Bluetooth Core Spec.
     * Vol.6, Part B Section 4.4.3.
     */
    gap_role_observer,

    /*! \brief GAP 2.2.2.3
     *
     * Any device that accepts the establishment of an LE physical link is
     *  referred to as being in the Peripheral role, and will be in the
     *  Slave role in the Link Layer Connection State as described in
     *  Bluetooth Core Spec. Vol.6, Part B Section 4.5.
     */
    gap_role_peripheral,

    /*! \brief GAP 2.2.2.4
     *
     * A device that supports the Central role initiates the establishment
     *  of a physical connection and will be in the Master role in the
     *  Link Layer Connection State as described in Bluecore Core spec.
     *  Vol.6, Part B Section 4.5.
     */
    /* GAP 9.3.5.2
     *  A device in the Central role may support the auto connection
     *   establishment procedure. The device in Central role shall support
     *   the general connection establishment procedure.
     */
    gap_role_central
} gap_role;


/*! \brief GAP Discovery modes.  See Vol 3 Part C Section 9.2 of
 *  the Bluetooth Specification Version 4.
 */
/* GAP 2.2.2.4 and 9.2.5.2, 9.2.6.2
 *  A device in the Central role may support either the limited or general
 *  discovery procedures. While a device in the Broadcaster, Observer or
 *  Peripheral role shall support neither procedure.
 */
typedef enum
{
    /* The un-initialised mode. FOR INTERNAL USE ONLY. */
    gap_mode_discover_null,

    /*! \brief GAP 9.2.2.2
     *
     * A device in non-discoverable mode and shall support neither
     * limited nor general discoverable mode.
     */
    gap_mode_discover_no,

    /*! \brief GAP 9.2.3.2
     *
     * While a device is the Peripheral role the device may support the
     * limited discoverable mode.
     *
     * But while a device is in the Broadcaster, Observer or Central role
     * the device shall not support the limited discoverable mode.
     */
    gap_mode_discover_limited,

    /*! \brief GAP 9.2.4.2
     *
     * While a device is in the Peripheral role the device may support the
     * general discoverable mode.
     *
     * But while a device is in the Broadcaster, Observer or Central role
     * the device shall not support the general discoverable mode.
     */
    gap_mode_discover_general,
} gap_mode_discover;


/*! \brief GAP Connection modes.  See Vol 3 Part C Section 9.3 of
 *  the Bluetooth Specification Version 4.
 */
typedef enum
{
    /* The un-initialised mode. FOR INTERNAL USE ONLY. */
    gap_mode_connect_null,

    /*! \brief GAP 9.3.2
     *
     * A device in the non-connectable mode shall not allow a connection
     * to be established. While a device is in the Peripheral role it shall
     * support the non-connectable mode.
     *
     * A Peripheral device in the non-connectable mode may send
     * non-connectable undirected advertising events or discoverable
     * undirected advertising events.
     */
    gap_mode_connect_no,

    /*! \brief GAP 9.3.3
     *
     * A device in the Peripheral role may support the directed
     * connectable mode. \note To use directed connectable mode with low duty cycle
     * this option must be chosen.For enabling the advertisements, application
     * call the following API's: \n (1) GapSetMode to set the GAP role as
     * peripheral, type of advertisements (undirected, directed etc) and few
     * other settings.\n (2) GattConnectReq() to start the advertisements. This
     * API takes an input parameter i.e connection flag(masks) containing the
     * type of advertisement.\n In case of low duty cycle directed advertisements
     * flag is OR-ed with L2CAP_CONNECTION_SLAVE_DIRECTED_LDC.
     *
     */
    gap_mode_connect_directed,

    /*! \brief GAP 9.3.4
     *
     * A device in the Peripheral role may support the undirected
     * connectable mode.
     */
    gap_mode_connect_undirected

} gap_mode_connect;


/*! \brief GAP Bonding modes.  See Vol 3 Part C Section 9.4 of
 *  the Bluetooth Specification Version 4.
 */
typedef enum
{
    /* The uninitialised mode. FOR INTERNAL USE ONLY. */
    gap_mode_bond_null,

    /*! \brief GAP 9.4.2.1
     *
     * A device in the non-bondable mode does not allow a bond to be
     * created with a peer device. A device in the Peripheral or Central
     * role shall support the non-bondable mode.
     */
    gap_mode_bond_no,

    /*! \brief GAP 9.4.3.1
     *
     * A device in the bondable mode allows a bond to be created with a
     * peer device also in the bondable mode. A device in the Peripheral
     * or Central role shall support the bondable mode.
     */
    gap_mode_bond_yes

} gap_mode_bond;


/*! \brief GAP Security modes.  See Vol 3 Part C Section 10.2 of
 *  the Bluetooth Specification Version 4.  Only LE Security Mode 1
 *  modes (section 10.2.1) are specified here.
 */
typedef enum
{
    /*! \brief  GAP 10.2
     *
     *  No security.
     */
    gap_mode_security_none,

    /*! \brief GAP 10.3
     *
     * Unauthenticated pairing involves performing the pairing procedure
     * with authentication set to "No MITM protection" (i.e. no prior
     * link-key theft and impersonation guard).
     */
    gap_mode_security_unauthenticate,

    /*! \brief GAP 10.3
     *
     * Authenticated pairing involves performing the pairing procedure
     * defined in Bluetooth Core Spec. Vol.3, Part H Section 2.1, with
     * the authentication set to "MITM protection" (i.e. new exchange
     * of link keys).
     */
    gap_mode_security_authenticate,
} gap_mode_security;


/*! \brief GAP feature set.
 *
 * See Bluetooth Specification v4 Volume 6 Part B section 4.6,
 * and GAP Volume 3 section 9.
 *
 * LE supported features is limited to encryption.
 */
typedef enum
{
    gap_feature_set_encryption = 0

} gap_feature_set;


/*! @} */


#endif  /* __GAP_TYPES_H__ */
