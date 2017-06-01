/******************************************************************************
 *  Copyright 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  CSRmesh is a product of Qualcomm Technologies International Ltd.
 */
 /*! \file cm_hal.h
 *  \brief Header file for connection manager hardware abstraction layer
 */

#ifndef __CM_HAL_H__
#define __CM_HAL_H__

/*! \addtogroup CM_Hal
 * @{
 */

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <core_event_types.h>
#include <bt_event_types.h>
#include <status.h>
#include <bluetooth.h>
#include <security.h>

#ifndef CSR101x_A05
#include <gatt_msg.h>
#include <ls_msg.h>
#include <sm_msg.h>
#include <msg_id.h>
#else
#include <gap_app_if.h>
#include <bt_event_types.h>
#include <hci_event_types.h>
#endif

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "cm_types.h"
#include "cm_private.h"

 /*============================================================================*
 *  Public Data Types
 *============================================================================*/

/* Baldrick datatype for all events received */
#if defined (CSR101x_A05)
typedef struct
{
    lm_event_code   id;
    LM_EVENT_T      *body;

}baldrick_msg_t;
#endif /* CSR101x_A05 */

/*! \brief Incoming event message type */
typedef union
{
#ifndef CSR101x_A05
    msg_t *msg;                /*! \brief Non-specfic message type for Flashheart messages */  
#else
    baldrick_msg_t *msg;       /*! \brief Non-specfic message type for Baldrick messages */ 
#endif
}h_msg_t;

#ifndef CSR101x_A05

/* FLASHHEART types */

/*! \brief Status Type structure */
typedef status_t                            h_status_t;                        

/* LS Events Structures */

/*! \brief Message body for HAL_ADVERTISING_REPORT_IND */
typedef ls_enh_advertising_report_ind_t     h_ls_advertising_report_ind_t;      

/*! \brief Message body for HAL_CONNECTION_COMPLETE_IND */
typedef ls_connection_complete_ind_t        h_ls_connection_complete_ind_t;    

/*! \brief Message body for HAL_DISCONNECT_COMPLETE_IND */ 
typedef ls_disconnect_complete_ind_t        h_disconnect_complete_ind_t;    

/*! \brief Message body for HAL_LS_RADIO_EVENT_IND message */    
typedef ls_radio_event_ind_t                h_ls_radio_event_ind_t;     

/*! \brief Message body for HAL_LS_RADIO_EVENT_IND message */    
typedef ls_early_wakeup_ind_t               h_ls_early_wakeup_ind_t;     

/*! \brief Message body for HAL_LS_DATA_RX_TIMING_IND */        
typedef ls_data_rx_timing_ind_t             h_ls_data_rx_timing_ind_t;          

/*! \brief Message body for a HAL_LS_CONNECTION_PARAM_UPDATE_CFM event */
typedef ls_connection_param_update_cfm_t    h_ls_connection_param_update_cfm_t; 

/*! \brief Message body for HAL_LS_CONNECTION_UPDATE_IND */
typedef ls_connection_update_ind_t          h_ls_connection_update_ind_t;      

/*! \brief Message body for a HAL_LS_CONNECTION_PARAM_UPDATE_IND event */ 
typedef ls_connection_param_update_ind_t    h_ls_connection_param_update_ind_t; 

/*! \brief Message body for a HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND indication */
typedef ls_connection_update_signalling_ind_t
                                        h_ls_connection_update_signalling_ind_t; 

/* SM Events Structures */

/*! \brief Message body for HAL_SM_KEYS_IND */
typedef sm_keys_ind_t                       h_sm_keys_ind_t;  

/*! \brief Message body for HAL_SM_LONG_TERM_KEY_IND */           
typedef sm_long_term_key_ind_t              h_sm_long_key_ind_t;  

/*! \brief Message body for Security Information block */       
typedef SM_KEYSET_T                         h_sm_keyset_t;   

/*! \brief Message body for HAL_SM_PASSKEY_INPUT_IND */            
typedef sm_passkey_input_ind_t              h_sm_passkey_input_ind_t;   

/*! \brief Message body for HAL_SM_PAIRING_AUTH_IND */ 
typedef sm_pairing_auth_ind_t               h_sm_pairing_auth_ind_t;  

/*! \brief Message body for HAL_SM_ENCRYPTION_CHANGE_IND */   
typedef ls_encryption_change_ind_t          h_ls_encryption_change_ind_t;

/*! \brief Message body for HAL_SM_PAIRING_COMPLETE_IND  */
typedef sm_pairing_complete_ind_t           h_sm_pairing_complete_ind_t; 

/*! \brief Message body for HAL_SM_DIV_APPROVAL_IND */
typedef sm_div_approval_ind_t               h_sm_div_approval_ind_t; 

/*! \brief Message body for HAL_SM_KEY_REQUEST_IND */    
typedef sm_key_request_ind_t                h_sm_key_request_ind_t;      

/*! \brief Message body for HAL_LS_NUMBER_COMPLETED_PACKETS_IND */
typedef uint16  h_ls_number_completed_packets_ind_t;

/* GATT Events Structures */
/*! \brief Message body for Add Database confirm */
typedef gatt_add_db_cfm_t                   h_gatt_add_db_cfm_t;

/*! \brief Message body for HAL_GATT_EXCHANGE_MTU_IND */         
typedef gatt_exchange_mtu_ind_t             h_gatt_exchange_mtu_ind_t;   

/*! \brief Message body for response from a server-side application to a 
 * GATT_EXCHANGE_MTU_IND indication */
typedef gatt_exchange_mtu_rsp_t             hal_gatt_exchange_mtu_rsp_t; 

/*! \brief Message body for HAL_GATT_CHAR_VAL_INDICATION_CFM */
typedef gatt_char_val_indication_cfm_t      h_gatt_char_val_ind_cfm_t;   

/*! \brief Message body for HAL_GATT_CONNECT_CFM */
typedef gatt_connect_cfm_t                  h_gatt_connect_cfm_t;   

/*! \brief Message body for HAL_GATT_CANCEL_CONNECT_CFM */     
typedef gatt_cancel_connect_cfm_t           h_gatt_cancel_connect_cfm_t; 

/*! \brief Message body for HAL_GATT_ACCESS_IND */
typedef gatt_access_ind_t                   h_gatt_access_ind_t;

/*! \brief Message body for HAL_GATT_SERV_INFO_IND */         
typedef gatt_serv_info_ind_t                h_gatt_serv_info_ind_t;  

/*! \brief Message body for HAL_GATT_DISC_ALL_PRIM_SERV_CFM */    
typedef gatt_disc_all_prim_serv_cfm_t       h_gatt_disc_all_prim_serv_cfm_t; 

/*! \brief Message body for HAL_GATT_CHAR_DECL_INFO_IND */
typedef gatt_char_decl_info_ind_t           h_gatt_char_decl_info_ind_t;     

/*! \brief Message body for HAL_GATT_DISC_SERVICE_CHAR_CFM. */
typedef gatt_disc_service_char_cfm_t        h_gatt_disc_service_char_cfm_t;  

 /*! \brief Message body for HAL_GATT_CHAR_DESC_INFO_IND */
typedef gatt_char_desc_info_ind_t           h_gatt_char_desc_info_ind_t; 

/*! \brief Message body for HAL_GATT_DISC_ALL_CHAR_DESC_CFM */   
typedef gatt_disc_all_char_desc_cfm_t       h_gatt_disc_all_char_desc_cfm_t;

/*! \brief Message body for HAL_GATT_WRITE_CHAR_VAL_CFM */
typedef gatt_write_char_val_cfm_t           h_gatt_write_char_val_cfm_t;     

/*! \brief Message body for HAL_GATT_READ_CHAR_VAL_CFM */
typedef gatt_read_char_val_cfm_t            h_gatt_read_char_val_cfm_t;  

/*! \brief Message body for HAL_GATT_INDICATION_IND */    
typedef gatt_char_val_ind_t                 h_gatt_char_val_ind_t;           

/*! \brief Events Types */
typedef enum
{
    /*! \brief Add database confirm */
    HAL_GATT_ADD_DB_CFM                 = GATT_ADD_DB_CFM,      
    /*! \brief Indication received by the server as part of the remotely initiated
    * Exchange MTU procedure */    
    HAL_GATT_EXCHANGE_MTU_IND           = GATT_EXCHANGE_MTU_IND,  
    /*! \brief Enhanced advertising report */    
    HAL_ADVERTISING_REPORT_IND          = LS_ENH_ADVERTISING_REPORT_IND, 
    /*! \brief Connection complete indication */
    HAL_CONNECTION_COMPLETE_IND         = LS_CONNECTION_COMPLETE_IND,
    /*! \brief Confirmation of completion of a connection attempt */    
    HAL_GATT_CONNECT_CFM                = GATT_CONNECT_CFM,   
    /*! \brief Notification received by the client as part of the server initiated Characteristic Value Notification procedure */    
    HAL_GATT_CHAR_VAL_NOTIFICATION_CFM  = GATT_CHAR_VAL_NOTIFICATION_CFM,  
    /*! \brief Indication received by the client as part of the server initiated Characteristic Value Indication procedure */
    HAL_GATT_CHAR_VAL_INDICATION_CFM    = GATT_CHAR_VAL_INDICATION_CFM,
    /*! \brief Disconnect complete indication */
    HAL_DISCONNECT_COMPLETE_IND         = LS_DISCONNECT_COMPLETE_IND,
    /*! \brief Keys and associated security information indication */    
    HAL_SM_KEYS_IND                     = SM_KEYS_IND, 
    /*! \brief Request a Long Term Key from the Security Manager */    
    HAL_SM_LONG_TERM_KEY_IND            = SM_LONG_TERM_KEY_IND,  
    /*! \brief Pairing request received from the peer device */    
    HAL_SM_PAIRING_AUTH_IND             = SM_PAIRING_AUTH_IND,
    /*! \brief Security Manager is in pairing mode,
    * and need the user to enter the pass key displayed by 
    * the peer */    
    HAL_SM_PASSKEY_INPUT_IND            = SM_PASSKEY_INPUT_IND,      
    /*! \brief Indicates the pairing procedure has completed */    
    HAL_SM_PAIRING_COMPLETE_IND         = SM_PAIRING_COMPLETE_IND, 
    /*! \brief Security Manager received a encryption request from the peer */    
    HAL_SM_DIV_APPROVAL_IND             = SM_DIV_APPROVAL_IND,        
    /*! \brief Encryption change indication */    
    HAL_SM_ENCRYPTION_CHANGE_IND        = LS_ENCRYPTION_CHANGE_IND,
    /*!<\brief  Indicates the number of HCI data packets completed */
    HAL_LS_NUMBER_COMPLETED_PACKETS_IND = LS_NUMBER_COMPLETED_PACKETS_IND,        
    /*! \brief Request keys before starting a security procedure */
    HAL_SM_KEY_REQUEST_IND              = SM_KEY_REQUEST_IND,
    /*! \brief Access indication received by the server */
    HAL_GATT_ACCESS_IND                 = GATT_ACCESS_IND,
    /*! \brief Radio event indication */
    HAL_LS_RADIO_EVENT_IND              = LS_RADIO_EVENT_IND,
    /*! \brief Indication for an early wake up request */
    HAL_LS_EARLY_WAKEUP_IND             = LS_EARLY_WAKEUP_IND, 
    /*! \brief Report containing transmit receive timing information */
    HAL_LS_DATA_RX_TIMING_IND           = LS_RX_TIMING_IND,
    /*! \brief Indication received during a find included services procedure */
    HAL_GATT_SERV_INFO_IND              = GATT_SERV_INFO_IND,
    /*! \brief Confirmation at the end of a Discover All Primary Services procedure */
    HAL_GATT_DISC_ALL_PRIM_SERV_CFM     = GATT_DISC_ALL_PRIM_SERV_CFM,
    /*! \brief Indications received during Discover All Characteristics of a Service */
    HAL_GATT_CHAR_DECL_INFO_IND         = GATT_CHAR_DECL_INFO_IND,  
    /*! \brief Confirmation at the end of a Discover All Characteristics of a Service procedure */    
    HAL_GATT_DISC_SERVICE_CHAR_CFM      = GATT_DISC_SERVICE_CHAR_CFM,   
    /*! \brief Indications received during Discover All Characteristic Descriptors procedure */    
    HAL_GATT_CHAR_DESC_INFO_IND         = GATT_CHAR_DESC_INFO_IND,  
    /*! \brief Confirmation at the end of a Discover All Characteristic Descriptors procedure */    
    HAL_GATT_DISC_ALL_CHAR_DESC_CFM     = GATT_DISC_ALL_CHAR_DESC_CFM,  
    /*! \brief Confirmation of the completion of a Write Characteristic Value procedure */  
    HAL_GATT_WRITE_CHAR_VAL_CFM         = GATT_WRITE_CHAR_VAL_CFM,    
    /*! \brief Confirmation of the completion of a Read Characteristic Value procedure */    
    HAL_GATT_READ_CHAR_VAL_CFM          = GATT_READ_CHAR_VAL_CFM,       
    /*! \brief Indication received by the client as part of the server initiated Characteristic Value Notification procedure */    
    HAL_GATT_NOTIFICATION_IND           = GATT_NOTIFICATION_CHAR_VAL_IND, 
    /*! \brief Indication received by the client as part of the server initiated Characteristic Value Indication procedure */ 
    HAL_GATT_INDICATION_IND             = GATT_INDICATION_CHAR_VAL_IND,
    /*! \brief Confirmation, success or otherwise of the cancel connect request message */  
    HAL_GATT_CANCEL_CONNECT_CFM         = GATT_CANCEL_CONNECT_CFM,   
    /*! \brief Indication, send to the application, indicating the remote device has requested a connection parameter update */
    HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND \
                                        = LS_CONNECTION_UPDATE_SIGNALLING_IND, 
    /*! \brief Indication sent to application when connection parameters change */
    HAL_LS_CONNECTION_PARAM_UPDATE_IND  = LS_CONNECTION_PARAM_UPDATE_IND,  
    /*! \brief Indication for connection update */    
    HAL_LS_CONNECTION_UPDATE_IND        = LS_CONNECTION_UPDATE_IND, 
     /*! \brief Confirmation for update to a connection's interval, latency and time out parameters */    
    HAL_LS_CONNECTION_PARAM_UPDATE_CFM  = LS_CONNECTION_PARAM_UPDATE_CFM      
}hal_events;

#else

/* Baldrick types */

/* Status Type */
typedef void                                sys_status_t;

/* LM Events Structures */
typedef LM_EV_ADVERTISING_REPORT_T          h_ls_advertising_report_ind_t;
typedef LS_RADIO_EVENT_IND_T                h_ls_radio_event_ind_t;
typedef LS_DATA_RX_TIMING_IND_T             h_ls_data_rx_timing_ind_t;
typedef LM_EV_CONNECTION_UPDATE_T           h_ls_connection_update_ind_t;
typedef LS_CONNECTION_PARAM_UPDATE_CFM_T    h_ls_connection_param_update_cfm_t;
typedef LS_CONNECTION_PARAM_UPDATE_IND_T    h_ls_connection_param_update_ind_t;
typedef LS_CONNECTION_UPDATE_SIGNALLING_IND_T
                                        h_ls_connection_update_signalling_ind_t;
typedef uint16  h_ls_number_completed_packets_ind_t;

/* HCI Event Structures */
typedef HCI_EV_DATA_DISCONNECT_COMPLETE_T   h_disconnect_complete_ind_t;
typedef HCI_EV_DATA_ENCRYPTION_CHANGE_T     h_ls_encryption_change_ind_t;
typedef HCI_EV_DATA_ULP_CONNECTION_COMPLETE_T
                                            h_ls_connection_complete_ind_t;

/* SM Event Structures */
typedef SM_KEYS_IND_T                       h_sm_keys_ind_t;
typedef SM_LONG_TERM_KEY_IND_T              h_sm_long_key_ind_t;
typedef SM_KEYSET_T                         h_sm_keyset_t;
typedef SM_PAIRING_AUTH_IND_T               h_sm_pairing_auth_ind_t;
typedef SM_SIMPLE_PAIRING_COMPLETE_IND_T    h_sm_pairing_complete_ind_t;
typedef SM_DIV_APPROVE_IND_T                h_sm_div_approval_ind_t;
typedef SM_PASSKEY_INPUT_IND_T              h_sm_passkey_input_ind_t;
typedef SM_KEY_REQUEST_IND_T                h_sm_key_request_ind_t;

/* GATT Event Structures */
typedef GATT_ADD_DB_CFM_T                   h_gatt_add_db_cfm_t;
typedef GATT_EXCHANGE_MTU_IND_T             h_gatt_exchange_mtu_ind_t;
typedef GATT_CHAR_VAL_IND_CFM_T             h_gatt_char_val_ind_cfm_t;
typedef GATT_CONNECT_CFM_T                  h_gatt_connect_cfm_t;
typedef GATT_CANCEL_CONNECT_CFM_T           h_gatt_cancel_connect_cfm_t;
typedef GATT_ACCESS_IND_T                   h_gatt_access_ind_t;
typedef GATT_SERV_INFO_IND_T                h_gatt_serv_info_ind_t;
typedef GATT_DISC_ALL_PRIM_SERV_CFM_T       h_gatt_disc_all_prim_serv_cfm_t;
typedef GATT_CHAR_DECL_INFO_IND_T           h_gatt_char_decl_info_ind_t;
typedef GATT_DISC_SERVICE_CHAR_CFM_T        h_gatt_disc_service_char_cfm_t;
typedef GATT_CHAR_DESC_INFO_IND_T           h_gatt_char_desc_info_ind_t;
typedef GATT_DISC_ALL_CHAR_DESC_CFM_T       h_gatt_disc_all_char_desc_cfm_t;
typedef GATT_WRITE_CHAR_VAL_CFM_T           h_gatt_write_char_val_cfm_t;
typedef GATT_READ_CHAR_VAL_CFM_T            h_gatt_read_char_val_cfm_t;
typedef GATT_CHAR_VAL_IND_T                 h_gatt_char_val_ind_t;


/* Event Types */
typedef enum
{
    HAL_GATT_ADD_DB_CFM                 = GATT_ADD_DB_CFM,
    HAL_GATT_EXCHANGE_MTU_IND           = GATT_EXCHANGE_MTU_IND,
    HAL_ADVERTISING_REPORT_IND          = LM_EV_ADVERTISING_REPORT,
    HAL_CONNECTION_COMPLETE_IND         = LM_EV_CONNECTION_COMPLETE,
    HAL_GATT_CONNECT_CFM                = GATT_CONNECT_CFM,
    HAL_GATT_CHAR_VAL_NOTIFICATION_CFM  = GATT_CHAR_VAL_NOT_CFM,
    HAL_GATT_CHAR_VAL_INDICATION_CFM    = GATT_CHAR_VAL_IND_CFM,
    HAL_DISCONNECT_COMPLETE_IND         = LM_EV_DISCONNECT_COMPLETE,
    HAL_SM_KEYS_IND                     = SM_KEYS_IND,
    HAL_SM_LONG_TERM_KEY_IND            = SM_LONG_TERM_KEY_IND,
    HAL_SM_PAIRING_AUTH_IND             = SM_PAIRING_AUTH_IND,
    HAL_SM_PAIRING_COMPLETE_IND         = SM_SIMPLE_PAIRING_COMPLETE_IND,
    HAL_SM_PASSKEY_INPUT_IND            = SM_PASSKEY_INPUT_IND,
    HAL_SM_DIV_APPROVAL_IND             = SM_DIV_APPROVE_IND,
    HAL_SM_ENCRYPTION_CHANGE_IND        = LM_EV_ENCRYPTION_CHANGE,
    HAL_LS_NUMBER_COMPLETED_PACKETS_IND = LS_NUMBER_COMPLETED_PACKETS_IND,
    HAL_SM_KEY_REQUEST_IND              = SM_KEY_REQUEST_IND,
    HAL_GATT_ACCESS_IND                 = GATT_ACCESS_IND,
    HAL_LS_RADIO_EVENT_IND              = LS_RADIO_EVENT_IND,
    HAL_LS_DATA_RX_TIMING_IND           = LS_DATA_RX_TIMING_IND,
    HAL_GATT_SERV_INFO_IND              = GATT_SERV_INFO_IND,
    HAL_GATT_DISC_ALL_PRIM_SERV_CFM     = GATT_DISC_ALL_PRIM_SERV_CFM,
    HAL_GATT_CHAR_DECL_INFO_IND         = GATT_CHAR_DECL_INFO_IND,
    HAL_GATT_DISC_SERVICE_CHAR_CFM      = GATT_DISC_SERVICE_CHAR_CFM,
    HAL_GATT_CHAR_DESC_INFO_IND         = GATT_CHAR_DESC_INFO_IND,
    HAL_GATT_DISC_ALL_CHAR_DESC_CFM     = GATT_DISC_ALL_CHAR_DESC_CFM,
    HAL_GATT_WRITE_CHAR_VAL_CFM         = GATT_WRITE_CHAR_VAL_CFM,
    HAL_GATT_READ_CHAR_VAL_CFM          = GATT_READ_CHAR_VAL_CFM,
    HAL_GATT_NOTIFICATION_IND           = GATT_NOT_CHAR_VAL_IND,
    HAL_GATT_INDICATION_IND             = GATT_IND_CHAR_VAL_IND,
    HAL_GATT_CANCEL_CONNECT_CFM         = GATT_CANCEL_CONNECT_CFM,
    HAL_LS_CONNECTION_UPDATE_SIGNALLING_IND \
                                        = LS_CONNECTION_UPDATE_SIGNALLING_IND,
    HAL_LS_CONNECTION_PARAM_UPDATE_IND  = LS_CONNECTION_PARAM_UPDATE_IND,
    HAL_LS_CONNECTION_UPDATE_IND        = LM_EV_CONNECTION_UPDATE,
    HAL_LS_CONNECTION_PARAM_UPDATE_CFM  = LS_CONNECTION_PARAM_UPDATE_CFM

}hal_events;

#endif


/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/*----------------------------------------------------------------------------
 *  HALGetMsgId
 *----------------------------------------------------------------------------*/
/*! \brief Gets the message id
 *
 * This function gets the message id
 * \param[in] msg Pointer to incoming message event
 * \returns Message ID
 *
 */
extern uint16 HALGetMsgId(h_msg_t *msg);

/*----------------------------------------------------------------------------
 *  HALGetMsg
 *----------------------------------------------------------------------------*/
/*! \brief Gets the message data 
 *
 * This function gets the message data
 * \param[in] msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void *HALGetMsg(h_msg_t *msg);

/*----------------------------------------------------------------------------
 *  HALGetDeviceId
 *----------------------------------------------------------------------------*/
/*! \brief Gets the device id from the event 
 *
 * This function gets the device id from the event 
 * \param[in] event_id Event ID
 * \param[in] event Event
 * \param[in] p_main_data Pointer to \ref CM_MAIN_DATA_T structure
 * \returns device handle
 *
 */
extern device_handle_id HALGetDeviceId(uint16 event_id, void *event,
                             CM_MAIN_DATA_T *p_main_data);

/*----------------------------------------------------------------------------
 *  HALAddNewDevice
 *----------------------------------------------------------------------------*/
/*! \brief Adds the new device to the connection info table 
 *
 * This function adds the new device to the connection info table 
 * \param[out] p_conn_info Pointer to \ref CM_CONN_INFO_T structure
 * \param[in] p_event_data Pointer to \ref h_ls_connection_complete_ind_t structure
 * \returns Nothing
 *
 */
extern void HALAddNewDevice(CM_CONN_INFO_T *p_conn_info,
                            h_ls_connection_complete_ind_t *p_event_data);

/*----------------------------------------------------------------------------
 * HALParseConnCompleteInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the connection complete indication 
 *
 * This function parses the connection complete indication 
 * \param[out] conn_complete_ind Pointer to \ref h_ls_connection_complete_ind_t structure
 * \param[in] msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void HALParseConnCompleteInd(
                            h_ls_connection_complete_ind_t *conn_complete_ind,
                            h_msg_t *msg);

/*----------------------------------------------------------------------------
 *  HALParseDisconnCompleteInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the disconnection complete indication 
 *
 * This function parses the disconnection complete indication 
 * \param[out] disconn_complete_ind Pointer to \ref h_disconnect_complete_ind_t structure
 * \param[in] msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void HALParseDisconnCompleteInd(
                            h_disconnect_complete_ind_t *disconn_complete_ind,
                            h_msg_t *msg);

/*----------------------------------------------------------------------------
 *  HALParseEncChangeInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the encryption change indication
 *
 * This function parses the encryption change indication
 * \param[out] enc_change_ind Pointer to \ref h_ls_encryption_change_ind_t structure
 * \param[in] msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void HALParseEncChangeInd(
                            h_ls_encryption_change_ind_t *enc_change_ind,
                            h_msg_t *msg);

/*----------------------------------------------------------------------------
 *  HALParseRadioEventInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the radio event indication
 *
 * This function parses the radio event indication
 * \param[out] radio_event_ind Pointer to \ref CM_RADIO_EVENT_IND_T structure
 * \param[in] p_event_data Pointer to \ref h_ls_radio_event_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseRadioEventInd(CM_RADIO_EVENT_IND_T *radio_event_ind,
                                  h_ls_radio_event_ind_t *p_event_data);

/*----------------------------------------------------------------------------
 *  HALParseConnParamUpdate
 *----------------------------------------------------------------------------*/
/*! \brief Parses the connection parameter update
 *
 * This function parses the connection parameter update
 * \param[out] connection_param Pointer to \ref CM_DEV_CONN_PARAM_T structure
 * \param[in] p_event_data Pointer to \ref h_ls_connection_update_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseConnParamUpdate(CM_DEV_CONN_PARAM_T *connection_param,
                                    h_ls_connection_update_ind_t* p_event_data);

#if defined (CENTRAL) || defined(OBSERVER)
/*----------------------------------------------------------------------------
 *  HALGapSetCentralMode
 *----------------------------------------------------------------------------*/
/*! \brief Sets the GAP for Central mode
 *
 * This function sets the GAP for Central mode
 * \param[in] bond_mode The GAP bond mode to set
 * \param[in] security_mode The GAP security mode to set
 * \returns Nothing
 *
 */
extern void HALGapSetCentralMode(gap_mode_bond bond_mode,
                                 gap_mode_security security_mode);

/*----------------------------------------------------------------------------
 *  HALParseAdvertisingReportInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the advertising report indication
 *
 * This function parses the advertising report indication
 * \param[out] cm_adv_report_ind Pointer to \ref CM_ADV_REPORT_IND_T structure
 * \param[in] adv_ind Pointer to \ref h_ls_advertising_report_ind_t
 * \returns Nothing
 *
 */
extern void HALParseAdvertisingReportInd(CM_ADV_REPORT_IND_T *cm_adv_report_ind,
        h_ls_advertising_report_ind_t *adv_ind);

/*----------------------------------------------------------------------------
 *  HALIsAdvertisingReport
 *----------------------------------------------------------------------------*/
/*! \brief Checks the event is advertising report or not
 *
 * This function checks the event is advertising report or not
 * \param[in] adv_ind Pointer to \ref h_ls_advertising_report_ind_t structure
 * \returns TRUE if success,otherwise FALSE
 *
 */
extern bool HALIsAdvertisingReport(h_ls_advertising_report_ind_t *adv_ind);

/*----------------------------------------------------------------------------
 *  HALGapLsFindAdType
 *----------------------------------------------------------------------------*/
/*! \brief Finds the Ad Type in the advertisement report
 *
 * This function finds the Ad Type in the advertisement report
 * \param[in] adv_ind Pointer to \ref h_ls_advertising_report_ind_t structure
 * \param[in] adv_type The AD type to search for
 * \param[in] data Pointer to a buffer to copy AD data into
 * \param[in] size The maximum number of bytes to copy
 * \returns Number of bytes copied (0 if type was not found or had empty data)
 *
 */
extern uint16 HALGapLsFindAdType(h_ls_advertising_report_ind_t *adv_ind,
                          uint16 adv_type,
                          uint16 *data,
                          uint16 size);

#endif

#if defined (PERIPHERAL)
/*----------------------------------------------------------------------------
 *  HALGapSetPeripheralMode
 *----------------------------------------------------------------------------*/
/*! \brief Sets the GAP for Peripheral mode 
 *
 * This function sets the GAP for Peripheral mode 
 * \param[in] role The GAP role to use
 * \param[in] bond_mode The GAP bonding mode to use
 * \param[in] security_mode The GAP security mode to use
 * \param[in] discover The GAP discover mode to use
 * \param[in] connect The GAP connect mode to use
 * \returns Nothing
 *
 */
extern void HALGapSetPeripheralMode(gap_role role,
                                    gap_mode_bond bond_mode,
                                    gap_mode_security security_mode,
                                    gap_mode_discover discover,
                                    gap_mode_connect connect);
#endif

#if defined (SERVER)
/*----------------------------------------------------------------------------
 *  HALServerWriteInit
 *----------------------------------------------------------------------------*/
/*! \brief  Initialises the server write 
 *
 *  This function initialises the server write
 * \returns Nothing
 *
 */
extern void HALServerWriteInit(void);

/*----------------------------------------------------------------------------
 *  HALParseAccessWriteInd
 *----------------------------------------------------------------------------*/
/*! \brief  Parses the Access Write Indication 
 *
 *  This function parses the Access Write Indication
 * \param[out] cm_server_write Pointer to \ref CM_WRITE_ACCESS_T structure
 * \param[in] p_event_data Pointer to \ref h_gatt_access_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseAccessWriteInd(CM_WRITE_ACCESS_T *cm_server_write,
        h_gatt_access_ind_t *p_event_data);

/*----------------------------------------------------------------------------
 *  HALParseAccessReadInd
 *----------------------------------------------------------------------------*/
/*! \brief  Parses the Access Read Indication 
 *
 *  This function parses the Access Read Indication
 * \param[out] cm_server_read Pointer to \ref CM_READ_ACCESS_T structure
 * \param[in] p_event_data Pointer to \ref h_gatt_serv_info_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseAccessReadInd(CM_READ_ACCESS_T *cm_server_read,
        h_gatt_access_ind_t *p_event_data);
#endif

#if defined (CLIENT)
/*----------------------------------------------------------------------------
 *  HALParseDiscoverServiceInd
 *----------------------------------------------------------------------------*/
/*! \brief  Parses the discovered service indication 
 *
 *  This function parses the discovered service indication 
 * \param[out] serv_instance Pointer to \ref CM_SERVICE_INSTANCE structure
 * \param[in] p_prim Pointer to \ref h_gatt_serv_info_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseDiscoverServiceInd(CM_SERVICE_INSTANCE
                            *serv_instance, h_gatt_serv_info_ind_t *p_prim);

/*----------------------------------------------------------------------------
 *  HALParseReadCharValCfm
 *----------------------------------------------------------------------------*/
/*! \brief  Parses the read confirmation
 *
 *  This function parses the read confirmation
 * \param[out] read_cfm Pointer to \ref CM_READ_CFM_T structure
 * \param[in] p_event_data Pointer to \ref h_gatt_read_char_val_cfm_t structure
 * \returns Nothing
 *
 */
extern void HALParseReadCharValCfm(CM_READ_CFM_T *read_cfm,
                    h_gatt_read_char_val_cfm_t *p_event_data);


/*----------------------------------------------------------------------------
 *  HALParseNotifInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses notification and indication
 *
 * This function parses notification and indication
 * \param[out] notif_ind Pointer to \ref CM_NOTIFICATION_T structure
 * \param[in] p_event_data Pointer to \ref h_gatt_char_val_ind_t structure
 * \returns Nothing
 *
 */
extern void HALParseNotifInd(CM_NOTIFICATION_T *notif_ind,
                h_gatt_char_val_ind_t *p_event_data);
#endif

/*----------------------------------------------------------------------------
 *  HALSMInit
 *----------------------------------------------------------------------------*/
/*! \brief Initialises the security manager
 *
 * This function initialises the security manager
 * \param[in] diversifier Diversifier
 * \returns  Nothing
 *
 */
extern void HALSMInit(uint16 diversifier);

/*----------------------------------------------------------------------------
 *  HALParseSmPairingAuthInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses SM Pairing Auth Indication
 *
 * This function parses SM Pairing Auth Indication
 * \param[out] bonding_auth_ind Pointer to \ref CM_BONDING_AUTH_IND_T structure
 * \param[in] p_event_data Pointer to \ref h_sm_pairing_auth_ind_t structure
 * \returns  Nothing
 *
 */
extern void HALParseSmPairingAuthInd(CM_BONDING_AUTH_IND_T *bonding_auth_ind,
                                     h_sm_pairing_auth_ind_t *p_event_data);

/*----------------------------------------------------------------------------
 *  HALStartEncryption
 *----------------------------------------------------------------------------*/
/*! \brief Starts the encryption with the remote device
 *
 * This function starts the encryption with the remote device
 * \param[in] device_id handle of the device
 * \returns Nothing
 *
 */
extern void HALStartEncryption(device_handle_id device_id);

/*----------------------------------------------------------------------------
 *  HALSecuritySetConfiguration
 *----------------------------------------------------------------------------*/
/*! \brief Sets the security configurations for flashheart and baldrick accordingly
 *
 * This function sets the security configurations for flashheart and baldrick accordingly
 * \param[in] io_capability io capability supported
 * \returns Connection manager status code
 *
 */
extern cm_status_code HALSecuritySetConfiguration(uint16 io_capability);

#if defined (SERVER)

/*----------------------------------------------------------------------------
 *  HALSendValueNotification
 *----------------------------------------------------------------------------*/
/*! \brief Sends the Notification Value to the remote device
 *
 * This function sends the Notification Value to the remote device
 * \param[in] cm_value_notify Pointer to \ref CM_VALUE_NOTIFICATION_T structure
 * \returns Nothing
 *
 */
extern void HALSendValueNotification(CM_VALUE_NOTIFICATION_T *cm_value_notify);

#ifndef CSR101x_A05
/*----------------------------------------------------------------------------
 *  HALSendValueNotificationExt
 *----------------------------------------------------------------------------*/
/*! \brief Sends the Notification Value to the remote device
 *
 * This function sends the Notification Value to the remote device
 * \param[in] cm_value_notify Pointer to \ref CM_VALUE_NOTIFICATION_T structure
 * \returns the status immediately
 *
 */
extern sys_status HALSendValueNotificationExt(CM_VALUE_NOTIFICATION_T *cm_value_notify);
#endif 

/*----------------------------------------------------------------------------
 *  HALSendValueIndication
 *----------------------------------------------------------------------------*/
/*! \brief Sends the Indication Value to the remote device
 *
 * This function sends the Indication Value to the remote device
 * \param[in] *cm_value_notify Pointer to \ref CM_VALUE_NOTIFICATION_T structure
 * \returns Nothing
 *
 */
extern cm_status_code HALSendValueIndication(CM_VALUE_NOTIFICATION_T *cm_value_notify);
#endif /*! \brief SERVER */

/*----------------------------------------------------------------------------
 *  HALSendAuthRsp
 *----------------------------------------------------------------------------*/
/*! \brief Sends the bonding authorisation response
 *
 * This function sends the bonding authorisation response
 * \param[in] auth_resp Pointer to \ref CM_AUTH_RESP_T structure
 * \returns Nothing
 *
 */
extern void HALSendAuthRsp(CM_AUTH_RESP_T *auth_resp);

/*----------------------------------------------------------------------------
 *  HALSendLongTermKeyAuthRsp
 *----------------------------------------------------------------------------*/
/*! \brief  Sends the LTK response for current connection
 *
 *  This function sends the LTK response for current connection
 * \param[in] ltk_resp Pointer to \ref CM_LTK_RESP_T structure
 * \returns Nothing
 *
 */
extern void HALSendLongTermKeyAuthRsp(CM_LTK_RESP_T *ltk_resp);

/*----------------------------------------------------------------------------
 *  HALSendPasskeyRsp
 *----------------------------------------------------------------------------*/
/*! \brief Handles the passkey response based on the hardware API's
 *
 * This function handles the passkey response based on the hardware API's
 * \param[in] device_id handle of the device
 * \param[in] pass_key The passkey value input provided
 * \returns Nothing
 *
 */
extern void HALSendPasskeyRsp(device_handle_id device_id,
                              const uint32 *pass_key);

/*----------------------------------------------------------------------------
 *  HALSMKeyRequestResponse
 *----------------------------------------------------------------------------*/
/*! \brief Handles the Keys Request response based on the hardware API's
 *
 * This function handles the Keys Request response based on the hardware API's
 * \param[in] device_id handle of the device
 * \param[in] keys Pointer to security keys or NULL if none available
 * \returns   Nothing
 *
 */
extern void HALSMKeyRequestResponse(device_handle_id device_id,
                                    h_sm_keyset_t   *keys);

/*----------------------------------------------------------------------------
 *  HALSMDivApprovalRsp
 *----------------------------------------------------------------------------*/
/*! \brief Handles the SM diversifier approval response
 *
 * This function handles the SM diversifier approval response
 * \param[in] cid The connection identifier of the link
 * \param[in] approve_div Set to SM_DIV_APPROVED if the diversifier refers to an LTK which 
 *            are valid for encrypting the link. Set to SM_DIV_REVOKED, if the 
 *            diversifier referencing to a revoked LTK
 * \returns Nothing
 *
 */
extern void HALSMDivApprovalRsp(uint16 cid, sm_div_verdict approve_div);

/*----------------------------------------------------------------------------
 *  HALConnectionParamUpdateReq
 *----------------------------------------------------------------------------*/
/*! \brief Requests for the connection parameters update
 *
 * This function requests for the connection parameters update
 * \param[in] device_id handle of the device
 * \param[in] new_params New connection parameters (minimum & maximum interval, 
 *            slave latency, & supervision timeout)
 * \returns Nothing
 *
 */
extern void HALConnectionParamUpdateReq(device_handle_id device_id,
                              ble_con_params *new_params);

/*----------------------------------------------------------------------------
 *  HALConnectionUpdateSignallingRsp
 *----------------------------------------------------------------------------*/
/*! \brief Response to the Connection Parameter Update Signalling Indication
 *
 * This function sends response to the Connection Parameter Update Signalling Indication
 * \param[in] device_id handle of the device
 * \param[in] sig_identifier An identifier for the specific connection update 
 *            signal that this response applies to
 * \param[in] accepted TRUE if the updated parameters are acceptable to the  
 *            application,FALSE otherwise
 * \returns   Nothing
 *
 */
extern void HALConnectionUpdateSignallingRsp(device_handle_id device_id,
                                            uint16  sig_identifier,
                                            bool    accepted);

/*----------------------------------------------------------------------------
 *  HALParseConnectionUpdateInd
 *----------------------------------------------------------------------------*/
/*! \brief Parses the connection update indication 
 *
 * This function parses the connection update indication 
 * \param[in]  p_event_data Pointer to message body for connection update 
 *             completion event \ref h_ls_connection_update_ind_t
 * \param[out] connection_param Pointer to \ref CM_DEV_CONN_PARAM_T structure
 * \returns Nothing
 *
 */
extern void HALParseConnectionUpdateInd(h_ls_connection_update_ind_t *p_event_data,
                                        CM_DEV_CONN_PARAM_T *connection_param);

/*----------------------------------------------------------------------------
 *  HALMatchBluetoothAddress
 *----------------------------------------------------------------------------*/
/*! \brief Matches the given Bluetooth Addresses
 *
 * This function gets the message id
 * \param[in] bdaddr1 Pointer to the first Bluetooth address
 * \param[in] bdaddr2 Pointer to the second  Bluetooth address
 * \param[in] irk Pointer to an array of IRKs if first bluetooth address 
 *            is resolvable private address
 * \returns TRUE if success
 *
 */
extern bool HALMatchBluetoothAddress(TYPED_BD_ADDR_T *bdaddr1,
                                     TYPED_BD_ADDR_T *bdaddr2,
                                     uint16 *irk);

/*----------------------------------------------------------------------------
 *  CMProcessEvent
 *----------------------------------------------------------------------------*/
/*! \brief Called whenever a LM-specific event is received by the
 * system
 *
 * This function is called whenever a LM-specific event is received by the
 * system
 * \param[in] h_msg Pointer to incoming message event
 * \returns Nothing
 *
 */
extern void CMProcessEvent(h_msg_t *h_msg);

/*!@} */

#endif /* __CM_HAL_H_ */
