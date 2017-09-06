/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 *
 *  FILE
 *      cm_peripheral.c
 *
 *  DESCRIPTION
 *      This file defines connection manager peripheral functionality
 *
 ******************************************************************************/
#if defined (PERIPHERAL)

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <gap_types.h>
#include <ls_err.h>
#include <ls_types.h>
#include <panic.h>
#include <gatt.h>
#include <gatt_uuid.h>
#include <timer.h>
#include <mem.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_peripheral.h"
#include "cm_types.h"
#include "cm_api.h"
#include "cm_private.h"
#include "cm_server.h"
#include "cm_security.h"
#include "cm_hal.h"

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* This constant is used in the main server app to define array that is
   large enough to hold the advertisement data.
 */
#define MAX_ADV_DATA_LEN                                    (31)

/* Acceptable shortened device name length that can be sent in advertisement
 * data
 */
#define SHORTENED_DEV_NAME_LEN                              (8)

/* length of Tx Power prefixed with 'Tx Power' AD Type */
#define TX_POWER_VALUE_LENGTH                               (2)

/* Number of AD flags added by GAP layer to AdvData (3 bytes)
 * Refer BT Spec 4.0, Vol 3, Part C, Sec 11.1.3.
 */
#define NUM_OF_GAP_AD_FLAGS                                 (3)

/* Macro to save the bond id to be unbonded */
#define SET_BONDID_TO_BE_UNBONDED(x) \
                          g_cm_peripheral_data.bond_id_to_be_removed = x

/* Macro to get the bond id to be unbonded */
#define GET_BOND_ID_TO_BE_UNBONDED g_cm_peripheral_data.bond_id_to_be_removed

/*============================================================================*
 *  Private Data Type
 *============================================================================*/

/* CM Peripheral data type */
typedef struct
{
    /* Advertisement parameters instance */
    CM_PERIPHERAL_ADVERT_PARAMETERS_T           advert_params;

    /* Advertisement type */
    uint16                                      cur_adv_type;

    /* Advertisement timer */
    timer_id                                    advert_tid;

    /* Flag when set to True stops the ongoing advertisements */
    bool                                        stop_adverts;

    /* Flag to know more advertising types available for advertising */
    bool                                        moreAdvertisementTypes;

    /* Flag to to know the unbonding is in progress */
    bool                                        unbonding;

    /* bonding removal bond id */
    bond_handle_id                              bond_id_to_be_removed;

    /* Advertising state */
    cm_advert_state                             advert_state;

} CM_PERIPHERAL_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Peripheral data */
static CM_PERIPHERAL_DATA_T g_cm_peripheral_data;

#ifndef THIN_CM4_MESH_NODE
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Called while exiting advertising state */
static void advertisingExit(void);

/* Adds the device name to the adveritising packet */
static void addDeviceNameToAdvData(uint16 adv_data_len, uint16 scan_data_len);

/* Adds the device name to the scan response packet */
static void addDeviceNameToScanRspData(uint16 scan_data_len);

/* Adds the advertisement and scan data */
static void addAdvScanData(CM_PERIPHERAL_ADVERT_TYPE_T *cur_advert_type);

/* Starts the advertisement procedure */
static void startAdvertisements(void);

/* Handles advertisement timer expiry */
static void gattAdvertTimerHandler(timer_id tid);

/* Handles the signal GATT_CONNECT_CFM */
static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data);

/* Handles the signal HAL_GATT_CANCEL_CONNECT_CFM */
static void handleSignalLsCancelConnectCfm(h_gatt_cancel_connect_cfm_t
                                           *p_event_data);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      advertisingExit
 *
 *  DESCRIPTION
 *     Called while exiting advertising state
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void advertisingExit(void)
{
    /* Cancel advertisement timer */
    TimerDelete(g_cm_peripheral_data.advert_tid);
    g_cm_peripheral_data.advert_tid                 = TIMER_INVALID;

    /* Initialise the advertisement flags */
    g_cm_peripheral_data.stop_adverts               = FALSE;
    g_cm_peripheral_data.cur_adv_type               = 0;
    g_cm_peripheral_data.moreAdvertisementTypes     = FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      addDeviceNameToAdvData
 *
 *  DESCRIPTION
 *      Adds device name to advertisement or scan
 *      response data. It follows below steps:
 *      a. Try to add complete device name to the advertisement packet
 *      b. Try to add complete device name to the scan response packet
 *      c. Try to add shortened device name to the advertisement packet
 *      d. Try to add shortened (max possible) device name to the scan
 *         response packet
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void addDeviceNameToAdvData(uint16 adv_data_len, uint16 scan_data_len)
{
    uint8 *p_device_name = NULL;
    uint16 device_name_adtype_len;

    /* Read device name along with AD Type and its length */
    p_device_name = g_cm_peripheral_data.advert_params.
                                advert_data.device_name.name;

    device_name_adtype_len = g_cm_peripheral_data.advert_params.
                             advert_data.device_name.length;

    /* Add complete device name to Advertisement data */
    p_device_name[0] = AD_TYPE_LOCAL_NAME_COMPLETE;

    /* Check if Complete Device Name can fit in remaining advertisement
     * data space including the one byte length field which will be added
     * by the GAP layer
     */
    if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - adv_data_len))
    {
        /* Add Complete Device Name to Advertisement Data */
        LsStoreAdvScanData(device_name_adtype_len , p_device_name,
                      ad_src_advertise);
    }
    /* Check if Complete Device Name can fit in Scan response message */
    else if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - scan_data_len))
    {
        /* Add Complete Device Name to Scan Response Data */
        LsStoreAdvScanData(device_name_adtype_len , p_device_name,
                      ad_src_scan_rsp);
    }
    /* Check if Shortened Device Name can fit in remaining advertisement
     * data space
     */
    else if((MAX_ADV_DATA_LEN - adv_data_len) >=
            (SHORTENED_DEV_NAME_LEN + 2)) /* Added 2 for Length and AD type
                                           * added by GAP layer
                                           */
    {
        /* Add shortened device name to Advertisement data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

        /* Add the short name to the advertisement data */
        LsStoreAdvScanData(SHORTENED_DEV_NAME_LEN , p_device_name,
                      ad_src_advertise);
    }
    else /* Add device name to remaining Scan reponse data space */
    {
        /* Add as much as can be stored in Scan Response data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

        /* Add the name to the scan response */
        LsStoreAdvScanData(MAX_ADV_DATA_LEN - scan_data_len,
                                    p_device_name,
                                    ad_src_scan_rsp);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      addDeviceNameToScanRspData
 *
 *  DESCRIPTION
 *      Adds device name to scan response packet
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void addDeviceNameToScanRspData(uint16 scan_data_len)
{
    uint8 *p_device_name = NULL;
    uint16 device_name_len;

    /* Read device name along with AD Type and its length */
    p_device_name = g_cm_peripheral_data.advert_params.
                                advert_data.device_name.name;

    device_name_len = g_cm_peripheral_data.advert_params.
                             advert_data.device_name.length;

    if((device_name_len + 1) <= (MAX_ADV_DATA_LEN - scan_data_len))
    {
        /* Add complete device name to Advertisement data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_COMPLETE;

        /* Add Complete Device Name to Scan Response Data */
        LsStoreAdvScanData(device_name_len , p_device_name,
                           ad_src_scan_rsp);
    }
    else /* Add device name to remaining Scan reponse data space */
    {
        /* Add as much as can be stored in Scan Response data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

        /* Add the name to the scan response */
        LsStoreAdvScanData(MAX_ADV_DATA_LEN - scan_data_len,
                                    p_device_name,
                                    ad_src_scan_rsp);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gattAdvertTimerHandler
 *
 *  DESCRIPTION
 *      Handles advertisement timer expiry.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void gattAdvertTimerHandler(timer_id tid)
{
    /* Based upon the timer id, stop on-going advertisements */
    if(g_cm_peripheral_data.advert_tid == tid)
    {
        g_cm_peripheral_data.advert_tid = TIMER_INVALID;

        if((g_cm_peripheral_data.cur_adv_type + 1) <
                g_cm_peripheral_data.advert_params.num_adv_types)
        {
            /* Move to the next advertisement type */
            ++g_cm_peripheral_data.cur_adv_type;

            /* More advertisements available */
            g_cm_peripheral_data.moreAdvertisementTypes = TRUE;
        }
        else
        {
            /* More advertisements un available */
            g_cm_peripheral_data.moreAdvertisementTypes = FALSE;

            /* Reset the current advertisement type */
            g_cm_peripheral_data.cur_adv_type = 0;
        }

        /* Stop the current advertisement type */
        GattCancelConnectReq();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      addAdvScanData
 *
 *  DESCRIPTION
 *      Adds the advertisement and scan data
 *
 *  RETURNS
 *     Nothing
 *
 *---------------------------------------------------------------------------*/
static void addAdvScanData(CM_PERIPHERAL_ADVERT_TYPE_T *cur_advert_type)
{
    /* A variable to keep track of the data added to AdvData. The limit is
     * MAX_ADV_DATA_LEN. GAP layer will add AD Flags to AdvData which
     * is 3 bytes. Refer BT Spec 4.0, Vol 3, Part C, Sec 11.1.3.
     */
    uint16 length_added_to_adv = NUM_OF_GAP_AD_FLAGS;

    /* Reset existing advertising data */
    LsStoreAdvScanData(0, NULL, ad_src_advertise);

    /* Reset existing scan response data */
    LsStoreAdvScanData(0, NULL, ad_src_scan_rsp);

    /* Add UUID list of the services supported by the device */
    if((cur_advert_type->advert_flags) & uuid_present)
    {
        uint8 *data = (uint8*)g_cm_peripheral_data.advert_params.
                        advert_data.device_uuid.uuid;

        uint16 length = g_cm_peripheral_data.advert_params.
                          advert_data.device_uuid.length;

        /* One added for Length field, which will be added to Adv Data by
         * GAP layer
         */
        length_added_to_adv += (length + 1);

        /* Add the UUID data */
        LsStoreAdvScanData(length, data, ad_src_advertise);
    }

    /* Add device appearance to the advertisements */
    if((cur_advert_type->advert_flags) & appearance_present)
    {
        uint8 *data = g_cm_peripheral_data.advert_params.
                      advert_data.device_appearance.value;

        uint16 length = g_cm_peripheral_data.advert_params.
                        advert_data.device_appearance.length;

        /* One added for Length field, which will be added to Adv Data by
         * GAP layer
         */
        length_added_to_adv += (length + 1);

        /* Add the appearance data */
        LsStoreAdvScanData(length, data, ad_src_advertise);
    }

    /* Add tx power value of device to the advertising data */
    if((cur_advert_type->advert_flags) & tx_power_present)
    {
        /* Tx power level value prefixed with 'Tx Power' AD Type */
        uint8 device_tx_power[TX_POWER_VALUE_LENGTH] = {AD_TYPE_TX_POWER};

        /* Add the read tx power level to device_tx_power
         * Tx power level value is of 1 byte
         */
        device_tx_power[TX_POWER_VALUE_LENGTH - 1] =
                        g_cm_peripheral_data.advert_params.advert_data.
                        tx_power_level;

        /* One added for Length field, which will be added to Adv Data by
         * GAP layer
         */
        length_added_to_adv += (TX_POWER_VALUE_LENGTH + 1);

        /* Add the TX power */
        LsStoreAdvScanData(TX_POWER_VALUE_LENGTH, device_tx_power,
                              ad_src_advertise);
    }

    /* Add manufacturer data to the advertising data */
    if((cur_advert_type->advert_flags) & manufacturer_data_present)
    {
        uint8 *data = g_cm_peripheral_data.advert_params.
                      advert_data.manuf_data.value;

        uint16 length = g_cm_peripheral_data.advert_params.
                        advert_data.manuf_data.length;

        /* One added for Length field, which will be added to Adv Data by
         * GAP layer
         */
        length_added_to_adv += (length + 1);

        /* Add the appearance data */
        LsStoreAdvScanData(length, data, ad_src_advertise);
    }

    /* Add device name to the advertising data */
    if((cur_advert_type->advert_flags) & name_present)
    {
       /* Add the name */
       addDeviceNameToAdvData(length_added_to_adv, 0);
    }

    else if((cur_advert_type->advert_flags) & name_present_scan_rsp)
    {
       /* Add the name to scan response packet */
       addDeviceNameToScanRspData(0);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startAdvertisements
 *
 *  DESCRIPTION
 *      Starts the advertising procedure
 *
 *  RETURNS
 *     Nothing
 *
 *---------------------------------------------------------------------------*/
static void startAdvertisements(void)
{
    uint16 connect_flags = 0;  /* connection flags */
    CM_PERIPHERAL_ADVERT_TYPE_T *cur_advert_type = &g_cm_peripheral_data.
                                                   advert_params.adv_type
                                                   [g_cm_peripheral_data.
                                                    cur_adv_type];

    if(cur_advert_type->advert_flags & local_address_random)
    {
        /* Local Bluetooth Address type is random */
        connect_flags |= L2CAP_OWN_ADDR_TYPE_RANDOM;
    }
    else
    {
        /* Local Bluetooth Address type is public */
        connect_flags |= L2CAP_OWN_ADDR_TYPE_PUBLIC;
    }

    if((cur_advert_type->advert_flags & directed_advert_supported) ||
       (cur_advert_type->advert_flags & ldc_directed_advert_supported))
    {
        if(cur_advert_type->advert_flags & directed_advert_supported)
        {
            /* Directed advertisement type  */
            connect_flags |= L2CAP_CONNECTION_SLAVE_DIRECTED;
        }
        else
        {
            /* Low duty cycle directed advertisement type  */
            connect_flags |= L2CAP_CONNECTION_SLAVE_DIRECTED_LDC;
        }

        /* Set the Bluetooth Address for the directed advertising */
        GapSetAdvAddress(&cur_advert_type->directed_bd_addr);

        /* Set the Advertising state to directed advertising */
        CMSetAdvState(cm_advert_state_directed_advertising);
    }
    else if (cur_advert_type->advert_flags & non_conn_advert_supported)
    {
        /* Set the GAP mode for peripheral */
        HALGapSetPeripheralMode(g_cm_peripheral_data.advert_params.role,
                            g_cm_peripheral_data.advert_params.bond,
                            g_cm_peripheral_data.advert_params.security,
                            cur_advert_type->discover,
                            cur_advert_type->connect);

        /* clear the existing advertisement data, if any */
        LsStoreAdvScanData(0, NULL, ad_src_advertise);

        /*Set the advertisement interval,API accepts the value in microseconds*/
        GapSetAdvInterval(cur_advert_type->adv_interval_min * MILLISECOND,
                          cur_advert_type->adv_interval_max * MILLISECOND);

        /* Add advertisement data if any */
        addAdvScanData(cur_advert_type);

        if(cur_advert_type->advert_flags & local_address_random )
        {
            /* Start broadcasting */
            LsStartStopAdvertise(TRUE, whitelist_disabled, ls_addr_type_random);
        }
        else
        {
            /* Start broadcasting */
            LsStartStopAdvertise(TRUE, whitelist_disabled, ls_addr_type_public);
        }

        return;
    }
    else
    {
        /* Advertisement type undirected */
        connect_flags |= L2CAP_CONNECTION_SLAVE_UNDIRECTED;

        if(cur_advert_type->advert_flags & whitelist_supported)
        {
            /* Whitelist supported */
            connect_flags |= L2CAP_CONNECTION_SLAVE_WHITELIST;
        }

        if(cur_advert_type->advert_flags & fast_advert_supported)
        {
            /* Set the Advertising state to fast advertising */
            CMSetAdvState(cm_advert_state_fast_advertising);
        }
        if(cur_advert_type->advert_flags & slow_advert_supported)
        {
            /* Set the Advertising state to slow advertising */
            CMSetAdvState(cm_advert_state_slow_advertising);
        }

        /* Add the advertisement and scan data */
        addAdvScanData(cur_advert_type);
    }

    if(!(cur_advert_type->advert_flags & directed_advert_supported))
    {
        /* Set the advertising intervals */
        GapSetAdvInterval(cur_advert_type->adv_interval_min,
                          cur_advert_type->adv_interval_max);
    }

    /* Set the GAP mode for peripheral */
    HALGapSetPeripheralMode(g_cm_peripheral_data.advert_params.role,
                            g_cm_peripheral_data.advert_params.bond,
                            g_cm_peripheral_data.advert_params.security,
                            cur_advert_type->discover,
                            cur_advert_type->connect);

    /* Start GATT connection in Slave role */
    GattConnectReq(NULL, connect_flags);

    /* Start advertisement timer */
    if(!(cur_advert_type->advert_flags & directed_advert_supported) &&
                            cur_advert_type->timeout_value)
    {
        TimerDelete(g_cm_peripheral_data.advert_tid);

        /* Start advertisement timer  */
        g_cm_peripheral_data.advert_tid = TimerCreate(
                                        cur_advert_type->timeout_value, TRUE,
                                        gattAdvertTimerHandler);
    }

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalGattConnectCfm
 *
 *  DESCRIPTION
 *      Handles the signal GATT_CONNECT_CFM
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

static void handleSignalGattConnectCfm(h_gatt_connect_cfm_t *p_event_data)
{
    if(!CMIsAdvertising())
        return;

    if(p_event_data->result == HCI_ERROR_DIRECTED_ADVERTISING_TIMEOUT)
    {
        /* HDC direct advertisement stopped */
        CMPeripheralDirectAdvTimeout();

        return;
    }

    /* Set the advertisement state to idle */
    CMSetAdvState(cm_advert_state_idle);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      handleSignalLsCancelConnectCfm
 *
 *  DESCRIPTION
 *      Handles the HAL_GATT_CANCEL_CONNECT_CFM for peripheral
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
static void handleSignalLsCancelConnectCfm(h_gatt_cancel_connect_cfm_t
                                           *p_event_data)
{
    if(!CMIsAdvertising())
        return; /* Not for peripheral role */

    if(g_cm_peripheral_data.stop_adverts)
    {
        /* Stop advertising was requested */

        if(g_cm_peripheral_data.unbonding)
        {
            /* Clearing the bonding information */
            ClearBondingInfo(GET_BOND_ID_TO_BE_UNBONDED);

            /* Reset the unbonding flag */
            g_cm_peripheral_data.unbonding = FALSE;

            /* Reset the bond id to un bonded */
            GET_BOND_ID_TO_BE_UNBONDED = CM_INVALID_BOND_ID;
        }

        /* Reset the stop advertisements flag */
        g_cm_peripheral_data.stop_adverts = FALSE;

        /* Move the Advertising state to idle */
        CMSetAdvState(cm_advert_state_idle);
    }
    else
    {
        /* Check if more advertisement types are availble */
        if(g_cm_peripheral_data.moreAdvertisementTypes)
        {
            /* One of the advertisement type has expired */
            startAdvertisements();
        }
        else
        {
            /* Move the Advertising state to idle */
            CMSetAdvState(cm_advert_state_idle);
        }
    }
}
#endif /* THIN_CM4_MESH_NODE */

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralInit
 *
 *  DESCRIPTION
 *      Initialises the peripheral module
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMPeripheralInit(void)
{
    /*  Initialise the advertising state */
    g_cm_peripheral_data.advert_state = cm_advert_state_idle;

    /* Initialise the bond id to removed flag */
    g_cm_peripheral_data.bond_id_to_be_removed = CM_INVALID_BOND_ID;

    /*  Initialise the unbonding flag */
    g_cm_peripheral_data.unbonding = FALSE;
}

#ifndef THIN_CM4_MESH_NODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CMSetAdvState
 *
 *  DESCRIPTION
 *      Sets the Advertising state
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMSetAdvState(cm_advert_state new_state)
{
    cm_advert_state old_state = g_cm_peripheral_data.advert_state;

    if (old_state != new_state)
    {
        CM_ADVERT_STATE_IND_T advert_state_ind;

        /* Handle exiting old state */
        switch (old_state)
        {
            case cm_advert_state_directed_advertising:
                /* FALLTHROUGH */
            case cm_advert_state_fast_advertising:
                /* FALLTHROUGH */
            case cm_advert_state_slow_advertising:
            {
                if(new_state != cm_advert_state_directed_advertising &&
                   new_state != cm_advert_state_fast_advertising &&
                    new_state != cm_advert_state_slow_advertising)
                {
                    advertisingExit();
                }
            }
            break;
            default:
            /* Nothing to do */
            break;
        }

        /* Save the new state */
        g_cm_peripheral_data.advert_state =  new_state;

        /* Notify the Application about the Advertising state change */
        advert_state_ind.new_state = new_state;
        advert_state_ind.old_state = old_state;
        CMNotifyEventToApplication(CM_ADVERT_STATE_IND,
                                       (CM_EVENT_T *)&advert_state_ind);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMGetAdvState
 *
 *  DESCRIPTION
 *      Gets the Advertising state
 *
 *  RETURNS
 *      cm_advert_state: Advertising state
 *
 *---------------------------------------------------------------------------*/

extern cm_advert_state CMGetAdvState(void)
{
    return g_cm_peripheral_data.advert_state;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralDirectAdvTimeout
 *
 *  DESCRIPTION
 *      Handles the directed advertisements timeout.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMPeripheralDirectAdvTimeout(void)
{
    g_cm_peripheral_data.moreAdvertisementTypes = FALSE;

    if((g_cm_peripheral_data.cur_adv_type + 1) <
            g_cm_peripheral_data.advert_params.num_adv_types)
    {
        /* Move to the next advertisement type */
        ++g_cm_peripheral_data.cur_adv_type;

        /* More advertisements available */
        g_cm_peripheral_data.moreAdvertisementTypes = TRUE;
    }

    /* Handles the advertisement exit */
    handleSignalLsCancelConnectCfm(NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralSetAdvertParams
 *
 *  DESCRIPTION
 *      Sets the advertising parameters
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMPeripheralSetAdvertParams(
                              CM_PERIPHERAL_ADVERT_PARAMETERS_T
                              *advertising_parameters)
{
    if((advertising_parameters->num_adv_types == 0) ||
            (advertising_parameters->num_adv_types > CM_MAX_ADV_TYPES))
    {
        /* Invalid number of advertisement types */
        return cm_status_failed;
    }

    g_cm_peripheral_data.advert_params  = *advertising_parameters;

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralStartAdvertising
 *
 *  DESCRIPTION
 *      Starts the advertising procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMPeripheralStartAdvertising(void)
{
    if(CMGetAdvState() != cm_advert_state_idle)
    {
        /* Advertising state is busy */
        return cm_status_busy;
    }

    /* Initialise the advertisement flags */
    g_cm_peripheral_data.cur_adv_type     = 0;
    g_cm_peripheral_data.moreAdvertisementTypes = FALSE;

    /* Start advertisement */
    startAdvertisements();

    return cm_status_success;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralStopAdvertising
 *
 *  DESCRIPTION
 *      Stops the advertising procedure
 *
 *  RETURNS
 *      cm_status_code: cm_status_success if the request is processed
 *
 *---------------------------------------------------------------------------*/
extern cm_status_code CMPeripheralStopAdvertising(void)
{
    if(CMIsAdvertising())
    {
        /* Stop advertisement timer */
        TimerDelete(g_cm_peripheral_data.advert_tid);
        g_cm_peripheral_data.advert_tid = TIMER_INVALID;

        /* Set the stop advertisement flag */
        g_cm_peripheral_data.stop_adverts = TRUE;

        /* Stop advertisements */
        GattCancelConnectReq();

        /* return success */
        return cm_status_success;
    }

    /* return failure */
    return cm_status_failed;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralRemovePairing
 *
 *  DESCRIPTION
 *       This function is called to remove pairing from peripheral device
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void CMPeripheralRemovePairing(bond_handle_id bond_id)
{
    switch(CMGetAdvState())
    {
        case cm_advert_state_directed_advertising:
        case cm_advert_state_fast_advertising:
        case cm_advert_state_slow_advertising:
        {
            /* CM currently advertising */

            /* Save the bond id to unbonded */
            SET_BONDID_TO_BE_UNBONDED(bond_id);

            /* Set the unbonding flag */
            g_cm_peripheral_data.unbonding = TRUE;

            /* Set the stop advertisement flag */
            g_cm_peripheral_data.stop_adverts = TRUE;

            /* Stop advertisements */
            GattCancelConnectReq();
        }
        break;
        default:
        {
            /* Clear the bonding information */
            ClearBondingInfo(bond_id);
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMIsAdvertising
 *
 *  DESCRIPTION
 *      Checks if CM is advertising
 *
 *  RETURNS
 *      bool: TRUE if the CM is advertising
 *
 *---------------------------------------------------------------------------*/
extern bool CMIsAdvertising(void)
{
    switch(CMGetAdvState())
    {
        case cm_advert_state_directed_advertising:
        case cm_advert_state_fast_advertising:
        case cm_advert_state_slow_advertising:
        {
            /* CM is in advertising state */
            return TRUE;
        }
        break;
        default:
            return FALSE;
        break;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CMPeripheralHandleProcessLmEvent
 *
 *  DESCRIPTION
 *      Handles the firmware events related to the peripheral role
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/

extern void CMPeripheralHandleProcessLmEvent(h_msg_t *msg)
{
    switch(HALGetMsgId(msg))
    {
        case HAL_GATT_CANCEL_CONNECT_CFM:
        {
            handleSignalLsCancelConnectCfm(
                    (h_gatt_cancel_connect_cfm_t*)HALGetMsg(msg));
        }
        break;

        case HAL_GATT_CONNECT_CFM:
        {
            handleSignalGattConnectCfm((h_gatt_connect_cfm_t*)HALGetMsg(msg));
        }
        break;

        default:
             /* Ignore any other event */
        break;
    }
}
#endif /* THIN_CM4_MESH_NODE */
#endif /* PERIPHERAL */
