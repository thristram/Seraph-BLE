/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      advertisement_handler.c
 *
 *  DESCRIPTION
 *      Implementation of CSRmesh App's Advertisement related routines
 *
 *****************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gap_types.h>
#include <ls_err.h>
#include <ls_types.h>
#ifndef CSR101x_A05
#include <ls_api.h>
#include <gap_api.h>
#else
#include <ls_app_if.h>
#include <gap_app_if.h>
#endif
#include <gatt.h>
#include <gatt_uuid.h>
#include <timer.h>
#include <random.h>
#include <mem.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "app_util.h"
#include "csr_mesh.h"
#include "appearance.h"
#include "gap_service.h"
#include "app_gatt_db.h"
#include "advertisement_handler.h"
#include "connection_handler.h"
#include "mesh_control_service.h"
#include "mesh_control_service_uuids.h"
#include "csr_sched.h"
#include "gatt_service.h"
#include "app_mesh_handler.h"
#ifdef GAIA_OTAU_RELAY_SUPPORT
#include "scan_handler.h"
#include "gaia_client_service_event.h"
#endif
/*============================================================================*
 *  Private Data
 *===========================================================================*/
CsrUint8        ad_data[MAX_USER_ADV_DATA_LEN];

CsrUint8        scan_rsp_data[MAX_USER_ADV_DATA_LEN];

/* Value for which advertisement timer needs to be started */
CsrUint32       advert_timer_value;

/* Store timer id for GATT connectable advertisements */
timer_id        gatt_advert_tid;

/* Local Device's Random Bluetooth Device Address. */
#ifdef USE_STATIC_RANDOM_ADDRESS
    BD_ADDR_T                      random_bd_addr;
#endif /* USE_STATIC_RANDOM_ADDRESS */
 
#ifdef ENABLE_LOT_MODEL
/* LOT Interest Service ID */
CsrUint8 g_lot_service_id[16];

/* LOT Advert Count */
CsrUint16 g_lot_advert_count = 0;
#endif /* ENABLE_LOT_MODEL */

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      getSupported16BitUUIDServiceList
 *
 *  DESCRIPTION
 *      This function prepares the list of supported 16-bit service UUIDs to be 
 *      added to Advertisement data. It also adds the relevant AD Type to the 
 *      starting of AD array.
 *
 *  RETURNS
 *      Return the size AD Service UUID data.
 *
 *---------------------------------------------------------------------------*/
static uint16 getSupported16BitUUIDServiceList(uint8 *p_service_uuid_ad)
{
    uint8 i = 0;

    /* Add 16-bit UUID for supported main service  */
    p_service_uuid_ad[i++] = AD_TYPE_SERVICE_UUID_16BIT_LIST;

    p_service_uuid_ad[i++] = LE8_L(MESH_CONTROL_SERVICE_UUID);
    p_service_uuid_ad[i++] = LE8_H(MESH_CONTROL_SERVICE_UUID);

    return ((uint16)i);

}
/*----------------------------------------------------------------------------*
 *  NAME
 *     csrStoreUserAdvData
 *
 * DESCRIPTION
 *     This function is called by the application to add either advertising or scan 
 *     response data. Each call to the function will add a single AD Structure 
 *     (refer to Bluetooth specification Vol.3 Part C Section 11). Repeated calls 
 *     will append new structures, to build up the data content.
 *     The application should not include the "length" parameter within the supplied 
 *     octet array - the GAP layer will add the length field in the appropriate 
 *     position. The first octet of the array should be the AD Type field (see ad_type) 
 *
 *  PARAMETERS
 *      param Pointer to a structure containing Advertising and Scan
 *           timing parameters to be set.
 *
 *  RETURNS/MODIFIES
 *      TRUE if data was stored successfully
 *
 *----------------------------------------------------------------------------*/
static bool csrStoreUserAdvData(uint16 len, uint8 *data, ad_src const src)
{
    uint8 offset;
    uint8 *p_data;

    /* Move to the end of the already stored data */
    if(src == ad_src_advertise)
    {
        p_data = ad_data;
    }
    else
    {
        p_data = scan_rsp_data;
    }

    if(data == NULL)
    {
        /* Data is NULL clear the data */
        MemSet(p_data, 0, MAX_USER_ADV_DATA_LEN);
        return TRUE;
    }

    offset = 0;
    while( offset < MAX_USER_ADV_DATA_LEN)
    {
        if(p_data[offset] == 0)
        {
            break;
        }
        offset += p_data[offset] + 1;
    }
    if( (offset + len) < MAX_USER_ADV_DATA_LEN )
    {
        p_data[offset] = (uint8)len;
        MemCopy(p_data + offset + 1, data, len);
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      addDeviceNameToAdvData
 *
 *  DESCRIPTION
 *      This function is used to add device name to advertisement or scan 
 *      response data. It follows below steps:
 *      a. Try to add complete device name to the advertisment packet
 *      b. Try to add complete device name to the scan response packet
 *      c. Try to add shortened device name to the advertisement packet
 *      d. Try to add shortened (max possible) device name to the scan 
 *         response packet
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void addDeviceNameToAdvData(uint16 adv_data_len, uint16 scan_data_len)
{

    uint8 *p_device_name = NULL;
    uint16 device_name_adtype_len;

    /* Read device name along with AD Type and its length */
    p_device_name = GapGetNameAndLength(&device_name_adtype_len);

    /* Add complete device name to Advertisement data */
    p_device_name[0] = AD_TYPE_LOCAL_NAME_COMPLETE;

    /* Increment device_name_length by one to account for length field
     * which will be added by the GAP layer. 
     */

    /* Check if Complete Device Name can fit in remaining advertisement 
     * data space 
     */
    if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - adv_data_len))
    {
        /* Add Complete Device Name to Advertisement Data */
        if (csrStoreUserAdvData(device_name_adtype_len , p_device_name, 
                      ad_src_advertise) != TRUE)
        {
            ReportPanic(app_panic_set_advert_data);
        }

    }
    /* Check if Complete Device Name can fit in Scan response message */
    else if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - scan_data_len)) 
    {
        /* Add Complete Device Name to Scan Response Data */
        if (csrStoreUserAdvData(device_name_adtype_len , p_device_name, 
                      ad_src_scan_rsp) != TRUE)
        {
            ReportPanic(app_panic_set_scan_rsp_data);
        }

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

       if (csrStoreUserAdvData(SHORTENED_DEV_NAME_LEN , p_device_name, 
                      ad_src_advertise) != TRUE)
        {
            ReportPanic(app_panic_set_advert_data);
        }

    }
    else /* Add device name to remaining Scan reponse data space */
    {
        /* Add as much as can be stored in Scan Response data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

       if (csrStoreUserAdvData(MAX_ADV_DATA_LEN - scan_data_len, 
                                    p_device_name, 
                                    ad_src_scan_rsp) != TRUE)
        {
            ReportPanic(app_panic_set_scan_rsp_data);
        }

    }

}
/*----------------------------------------------------------------------------*
 *  NAME
 *      gattSetAdvertData
 *
 *  DESCRIPTION
 *      This function is used to set advertisement data 
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void gattSetAdvertData(uint8* lot_interest_service_id)
{
    uint8 advert_data[MAX_ADV_DATA_LEN];
    uint16 length;

    int8 tx_power_level = 0xff; /* Signed value */

    /* Tx power level value prefixed with 'Tx Power' AD Type */
    uint8 device_tx_power[TX_POWER_VALUE_LENGTH] = {
                AD_TYPE_TX_POWER
                };

    uint8 device_appearance[DEVICE_APPEARANCE_LENGTH + 1] = {
                AD_TYPE_APPEARANCE,
                LE8_L(APPEARANCE_UNKNOWN_VALUE),
                LE8_H(APPEARANCE_UNKNOWN_VALUE)
                };

    /* A variable to keep track of the data added to AdvData. The limit is 
     * MAX_ADV_DATA_LEN. GAP layer will add AD Flags to AdvData which 
     * is 3 bytes. Refer BT Spec 4.0, Vol 3, Part C, Sec 11.1.3.
     */
    uint16 length_added_to_adv = 3;

#ifdef ENABLE_LOT_MODEL
    uint16 i;
    if(lot_interest_service_id != NULL)
    {
        advert_data[0] = AD_TYPE_SERVICE_UUID_128BIT;
        for( i = 0 ; i < 16; i++)
        {
            advert_data[1+i] = lot_interest_service_id[i];
        }      
        length = 17;
    }
    else
#endif /* ENABLE_LOT_MODEL */
    {
        /* Add UUID list of the services supported by the device */
        length = getSupported16BitUUIDServiceList(advert_data);
    }

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (length + 1);

    if (csrStoreUserAdvData(length, advert_data, 
                                       ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (sizeof(device_appearance) + 1);

    /* Add device appearance to the advertisements */
    if (csrStoreUserAdvData(ATTR_LEN_DEVICE_APPEARANCE + 1, 
        device_appearance, ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    /* Read tx power of the chip */
    if(LsReadTransmitPowerLevel(&tx_power_level) != ls_err_none)
    {
        /* Reading tx power failed */
        ReportPanic(app_panic_read_tx_pwr_level);
    }

    /* Add the read tx power level to device_tx_power 
      * Tx power level value is of 1 byte 
      */
    device_tx_power[TX_POWER_VALUE_LENGTH - 1] = (uint8 )tx_power_level;

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (TX_POWER_VALUE_LENGTH + 1);

    /* Add tx power value of device to the advertising data */
    if (csrStoreUserAdvData(TX_POWER_VALUE_LENGTH, device_tx_power, 
                          ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    addDeviceNameToAdvData(length_added_to_adv, 0);

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gattAdvertTimerHandler
 *
 *  DESCRIPTION
 *      This function is used to handle Gatt advertisement timer expiry.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void gattAdvertTimerHandler(timer_id tid)
{
    if (gatt_advert_tid == tid)
    {
        gatt_advert_tid = TIMER_INVALID;

            /* After the timer expiry switch to fast advertising if device is
             * not connected.
             */
        GattTriggerConnectableAdverts(NULL);
    }
}

#ifdef USE_STATIC_RANDOM_ADDRESS
/*-----------------------------------------------------------------------------*
 *  NAME
 *      generateStaticRandomAddress
 *
 *  DESCRIPTION
 *      This function generates a static random address.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void generateStaticRandomAddress(BD_ADDR_T *addr)
{
    uint16 temp[3];
    uint16 idx = 0;

    if (!addr) return;

    for (idx = 0; idx < 3;)
    {
        temp[idx] = Random16();
        if ((temp[idx] != 0) && (temp[idx] != 0xFFFF))
        {
            idx++;
        }
    }

    addr->lap = ((uint32)(temp[1]) << 16) | (temp[0]);
    addr->lap &= 0x00FFFFFFUL;
    addr->uap = (temp[1] >> 8) & 0xFF;
    addr->nap = temp[2];
    
    addr->nap &= ~BD_ADDR_NAP_RANDOM_TYPE_MASK;
    addr->nap |=  BD_ADDR_NAP_RANDOM_TYPE_STATIC;
}
#endif /* USE_STATIC_RANDOM_ADDRESS */


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

#ifdef ENABLE_LOT_MODEL
/*----------------------------------------------------------------------------*
 *  NAME
 *      GattTriggerLOTAdverts
 *
 *  DESCRIPTION
 *      This function is used to triggers LOT adverts
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void GattTriggerLOTAdverts(uint8* lot_interest_service_id)
{
    MemCopy(g_lot_service_id,lot_interest_service_id,16);
    g_lot_advert_count = LOT_INTEREST_ADVERT_COUNT;    
}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattTriggerConnectableAdverts
 *
 *  DESCRIPTION
 *      This function is used to trigger fast advertisements 
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void GattTriggerConnectableAdverts(uint8* lot_interest_service_id)
{
#ifdef GAIA_OTAU_RELAY_SUPPORT
    if(!scanning_ongoing)
#endif   
    {  
    CSRSchedResult result = CSR_SCHED_RESULT_FAILURE;
    CSR_SCHED_ADV_DATA_T gatt_adv_data;
    
#ifndef DISABLE_BEARER_SETTINGS
    /* Bridge is disabled do not send user adverts */
    if(!IsGattBearerEnabled())
    {
        AppSetState(app_state_idle);
        return;
    }
#endif /* DISABLE_BEARER_SETTINGS */

    /* Reset existing advertising data */
    csrStoreUserAdvData(0, NULL, ad_src_advertise);
    csrStoreUserAdvData(0, NULL, ad_src_scan_rsp);

#ifdef USE_STATIC_RANDOM_ADDRESS
    /* Restore the Random Address of the Bluetooth Device */
    MemCopy(&gatt_adv_data.adv_params.bd_addr.addr, 
            &random_bd_addr, 
            sizeof(BD_ADDR_T));
    gatt_adv_data.adv_params.bd_addr.type = L2CA_RANDOM_ADDR_TYPE;
#else
    gatt_adv_data.adv_params.bd_addr.type = L2CA_PUBLIC_ADDR_TYPE;
#endif /* USE_STATIC_RANDOM_ADDRESS */


    /* Set GAP peripheral params */
    gatt_adv_data.adv_params.role = gap_role_peripheral;
    gatt_adv_data.adv_params.bond = gap_mode_bond_no;
    gatt_adv_data.adv_params.connect_mode = gap_mode_connect_undirected;
    gatt_adv_data.adv_params.discover_mode = gap_mode_discover_general;
    gatt_adv_data.adv_params.security_mode = gap_mode_security_unauthenticate;

#ifdef ENABLE_LOT_MODEL
    if(g_lot_advert_count > 0)
    {
        /* Form the ad type data */
        gattSetAdvertData(g_lot_service_id);
        g_lot_advert_count--;
    }
    else
#endif /* ENABLE_LOT_MODEL */
    {
        /* Form the ad type data */
        gattSetAdvertData(NULL);
    }

    MemCopy(gatt_adv_data.ad_data, ad_data, MAX_USER_ADV_DATA_LEN);
    gatt_adv_data.ad_data_length = MAX_USER_ADV_DATA_LEN;
    MemCopy(gatt_adv_data.scan_rsp_data, scan_rsp_data, MAX_USER_ADV_DATA_LEN);
    gatt_adv_data.scan_rsp_data_length= MAX_USER_ADV_DATA_LEN;

    
    result = CSRSchedSendUserAdv(&gatt_adv_data, NULL);
   }
 
    
    advert_timer_value = ADVERT_INTERVAL;

    /* Restart the advertising timer */
    TimerDelete(gatt_advert_tid);        
    gatt_advert_tid = TimerCreate(advert_timer_value, TRUE, 
                                                gattAdvertTimerHandler);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattStopAdverts
 *
 *  DESCRIPTION
 *      This function is used to stop on-going advertisements.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GattStopAdverts(void)
{
    TimerDelete(gatt_advert_tid);
    gatt_advert_tid = TIMER_INVALID;
   
#ifdef ENABLE_LOT_MODEL
    /* Initialise LOT advert count */
    g_lot_advert_count = 0;
#endif /* ENABLE_LOT_MODEL */
}

#ifdef USE_STATIC_RANDOM_ADDRESS
/*----------------------------------------------------------------------------*
 *  NAME
 *      GattSetRandomAddress
 *
 *  DESCRIPTION
 *      This function is used to set the random address
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GattSetRandomAddress(void)
{
    /* Generate random address for the CSRmesh Device. */
    generateStaticRandomAddress(&random_bd_addr);

    /* Set the Static Random Address of the device. */
   GapSetRandomAddress(&random_bd_addr);
    
}

extern void GattGetRandomAddress(BD_ADDR_T* p_addr)
{
    MemCopy(p_addr, &random_bd_addr, sizeof(BD_ADDR_T));
}
#endif /* USE_STATIC_RANDOM_ADDRESS */



