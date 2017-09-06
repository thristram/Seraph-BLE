/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *    app_data_stream.c
 *
 * DESCRIPTION
 *    This file implements a simple protocol over the data model to exchange
 *    device information.
 *    The protocol:
 *       | CODE | LEN (1 or 2 Octets| Data (LEN Octets)|
 *       CODE Defined by APP_DATA_STREAM_CODE_T
 *       LEN - if MS Bit of First octet is 1, then len is 2 octets
 *       if(data[0] & 0x80) LEN = data[0]
 *
 ******************************************************************************/

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <timer.h>
#include <mem.h>

/*============================================================================*
 *  CSR Mesh Header Files
 *============================================================================*/
#include <csr_mesh.h>
#include <data_server.h>
#include <data_client.h>

/*=============================================================================*
 *  Local Header Files
*============================================================================*/
#include "data_model_handler.h"
#include "label.h"
#include "define.h"
#include "app_mesh_handler.h"
#ifdef ENABLE_WATCHDOG_MODEL
#include "watchdog_model_handler.h"
#endif

#ifdef ENABLE_DATA_MODEL

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/
/* Data stream send retry wait time */
#define STREAM_SEND_RETRY_TIME            (500 * MILLISECOND)
#define BLOCK_SEND_RETRY_TIME            (150 * MILLISECOND)
/* Data stream received timeout value */
#define RX_STREAM_TIMEOUT                 (5 * SECOND)

/* Max number of retries */
#define MAX_SEND_RETRIES                  (3)

/* Max data per per stream send */
#define MAX_DATA_STREAM_PACKET_SIZE       (8)
#define MAX_DATA_BLACK_PACKET_SIZE       (10)
/*============================================================================*
 *  Private Data Type
 *===========================================================================*/
/* Enum to define different states of the model when sending a stream */
typedef enum
{
    stream_send_idle = 1,     /* No stream is in progress */
    stream_start_flush_sent,  /* Stream_flush sent to start a stream */
    stream_send_in_progress,  /* stream in progress */
    stream_finish_flush_sent  /* stream_finish_flush_sent */
}stream_send_status_t;

/* Enum for different receiver states */
typedef enum
{
    stream_receive_idle = 1,     /* No stream is in progress */
    stream_start_flush_received, /* Received flush to start stream */
    stream_receive_in_progress   /* stream in progress */
}stream_recv_status_t;




/* Application protocol codes used to exchange device data over the
 * data stream model
 */
typedef enum
{
    CSR_DEVICE_INFO_REQ = 0x01,
    CSR_DEVICE_INFO_RSP = 0x02,
    CSR_DEVICE_INFO_SET = 0x03,
    CSR_DEVICE_INFO_RESET = 0x04
}APP_DATA_STREAM_CODE_T;

typedef struct
{
    uint16 src_id;
    uint16 nesn;  /* Next expected sequence number from the sender */
    stream_recv_status_t status;
}STREAM_RX_T;

typedef struct
{
    uint16 dest_id; /* Data stream destination ID */
    uint16 sn;    /* Sequence number to be sent with next pkt */
    uint16 last_data_len; /* Length of last transmitted stream data */
    stream_send_status_t   status; /* Stream status */
}STREAM_TX_T;


typedef struct
{
    STREAM_RX_T rx;
    STREAM_TX_T tx;
}APP_STREAM_STATE_DATA_T;


typedef enum
{
    block_receive_idle = 1,     
    block_start_flush_received, 
    block_receive_in_progress  
}block_recv_status_t;

typedef struct
{
    uint16 src_id;
    block_recv_status_t status;
}BLOCK_RX_T;

typedef struct
{
    uint16 dest_id; /* Data stream destination ID */
    uint8 data_sequence;/*接收顺序标志*/
    uint8 data_finish_flag;/*接收完成标志*/
}BLOCK_TX_T;

typedef struct
{
    BLOCK_RX_T rx;
    BLOCK_TX_T tx;
}APP_BLOCK_STATE_DATA_T;
/*=============================================================================*
 *  Private Data
 *============================================================================*/
/* String to give a brief description of the application */
/*static uint8 device_info[256];*/

/* Device info length */
static uint8 device_info_length;

/* Application data stream state */
static APP_STREAM_STATE_DATA_T app_stream_state;
static APP_BLOCK_STATE_DATA_T app_block_state;

/* Stream bytes sent tracker */
static uint16 tx_stream_offset = 0;

/* Stream send retry timer */
static timer_id stream_send_retry_tid = TIMER_INVALID;
static timer_id block_send_retry_tid = TIMER_INVALID;
/* Stream send retry counter */
static uint16 stream_send_retry_count = 0;

/* Current Rx Stream offset */
static uint16 rx_stream_offset = 0;

/* Rx Stream status flag */
static bool rx_stream_in_progress = FALSE;

/* Rx stream timeout tid */
static timer_id rx_stream_timeout_tid;

static uint8 Tx_Data_Buffer[40];
static uint8 Block_Send_Num = 0;
static uint8 Block_Send_Mod = 0;
static uint8 Tx_Data_Buffer_len = 0;
/*static APP_DATA_STREAM_CODE_T current_stream_code;*/

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static void streamSendRetryTimer(timer_id tid);
static void sendNextPacket(void);
static void resetRxStreamState(void);
static void resetTxStreamState(void);
static void rxStreamTimeoutHandler(timer_id tid);
static void handleCSRmeshDataStreamFlushInd(
                                          CSRMESH_DATA_STREAM_FLUSH_T *p_event);
static void handleCSRmeshDataBlockInd(uint16 src_id, 
                                            CSRMESH_DATA_BLOCK_SEND_T *p_event);
static void handleCSRmeshDataStreamDataInd(uint16 src_id, 
                                           CSRMESH_DATA_STREAM_SEND_T *p_event);
static void handleCSRmeshDataStreamSendCfm(
                                       CSRMESH_DATA_STREAM_RECEIVED_T *p_event);

static void endStream(void);
static void MeshRxdCheck_New(void);
static void blockSendRetryTimer(timer_id tid);
static void Handle01Data(void);
static void Handle02Data(void);
static void Handle03Data(void);
static void Handle04Data(void);
/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      streamSendRetryTimer
 *
 *  DESCRIPTION
 *      Timer handler to retry sending next packet
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/

static void streamSendRetryTimer(timer_id tid)
{
    CSRMESH_DATA_STREAM_SEND_T send_param;
    if( tid == stream_send_retry_tid )
    {
        stream_send_retry_tid = TIMER_INVALID;
        stream_send_retry_count++;
        if( stream_send_retry_count < MAX_SEND_RETRIES )
        {
            MemCopy(send_param.streamoctets, &BLE_TX_DATA[tx_stream_offset],
                                             app_stream_state.tx.last_data_len);
            send_param.streamoctets_len = app_stream_state.tx.last_data_len;
            send_param.streamsn = app_stream_state.tx.sn;
            
            #ifdef DEBUG_ENABLE
            uint8 chi = 0;
            DebugWriteString("\r\n");
            for(chi =0;chi < send_param.streamoctets_len;chi++)DebugWriteUint8(send_param.streamoctets[chi]);
            #endif                   

            /* Send the next packet */
            DataStreamSend(CSR_MESH_DEFAULT_NETID, 
                                      app_stream_state.tx.dest_id,
                                      AppGetCurrentTTL(), &send_param);

            stream_send_retry_tid =  TimerCreate(STREAM_SEND_RETRY_TIME, TRUE,
                                                          streamSendRetryTimer);
        }
        else
        {
            stream_send_retry_count = 0;
            endStream();
#ifdef ENABLE_WATCHDOG_MODEL
            WatchdogStart();
#endif /* ENABLE_WATCHDOG_MODEL */
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      rxStreamTimeoutHandler
 *
 *  DESCRIPTION
 *      Timer handler to handle rx stream timeout
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void rxStreamTimeoutHandler(timer_id tid)
{
    if( tid == rx_stream_timeout_tid )
    {
        /* Reset the stream */
        rx_stream_timeout_tid = TIMER_INVALID;
        rx_stream_in_progress = FALSE;
        resetRxStreamState();

#ifdef ENABLE_WATCHDOG_MODEL
        WatchdogStart();
#endif /* ENABLE_WATCHDOG_MODEL */
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      sendNextPacket
 *
 *  DESCRIPTION
 *      Forms a stream data packet with the current counter and sends it to
 *      the stream receiver
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void sendNextPacket(void)
{
    uint16 data_pending, len;
    CSRMESH_DATA_STREAM_SEND_T send_param;
    /* Stop retry timer */
    stream_send_retry_count = 0;
    TimerDelete(stream_send_retry_tid);
    stream_send_retry_tid = TIMER_INVALID;

    data_pending = device_info_length - tx_stream_offset;
    
    #ifdef DEBUG_ENABLE
     DebugWriteString("\r\n the data_pending is:");
     DebugWriteUint16(data_pending);
    #endif
     
    if( data_pending )
    {
        len = (data_pending > MAX_DATA_STREAM_PACKET_SIZE)? 
                                MAX_DATA_STREAM_PACKET_SIZE : data_pending;
        MemCopy(send_param.streamoctets, &BLE_TX_DATA[tx_stream_offset],len);
        send_param.streamoctets_len = len;
        send_param.streamsn = app_stream_state.tx.sn;
            
        /* Send the next packet */
        DataStreamSend(CSR_MESH_DEFAULT_NETID, app_stream_state.tx.dest_id,
                       AppGetCurrentTTL(), &send_param);
        
        #ifdef DEBUG_ENABLE
        uint8 chi = 0;
        	DebugWriteString("\r\n");
		for(chi =0;chi < len;chi++)DebugWriteUint8(send_param.streamoctets[chi]);
        #endif   
        app_stream_state.tx.last_data_len = len;

        stream_send_retry_tid = TimerCreate(STREAM_SEND_RETRY_TIME, TRUE,
                                                       streamSendRetryTimer);
    }
    else
    {
        /* Send flush to indicate end of stream */
        endStream();
#ifdef ENABLE_WATCHDOG_MODEL
        WatchdogStart();
#endif /* ENABLE_WATCHDOG_MODEL */
    }
}

/*----------------------------------------------------------------------------*
 * NAME 
 *     resetRxStreamState
 * 
 * DESCRIPTION
 *     Resets a stream being received
 *     This function resets a stream being received.\n
 *     The application must call this function to reset a stream in progress
 *     in case of a timeout. This will reset the receive status of the stream
 *     model so that it is ready to receive a new stream.
 *----------------------------------------------------------------------------*/
static void resetRxStreamState(void)
{
    /* Reset source ID */
    app_stream_state.rx.src_id = 0;
    app_stream_state.rx.status = stream_receive_idle;
    /*cdy add*/
    app_stream_state.rx.nesn = 0xffff;
}


static void resetTxStreamState(void)
{

    app_stream_state.tx.status = stream_send_idle;
    app_stream_state.tx.sn = 0;
    app_stream_state.tx.dest_id = 0;

}
/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamFlushInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_FLUSH message.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleCSRmeshDataStreamFlushInd(
                                        CSRMESH_DATA_STREAM_FLUSH_T *p_event)
{
    /*rx_stream_offset = 0;*/ /*mod by cdy*/

    if( rx_stream_in_progress == FALSE )
    {
#ifdef ENABLE_WATCHDOG_MODEL
        /* Stop Watchdog */
        WatchdogPause();
#endif /* ENABLE_WATCHDOG_MODEL */
        /* Start the stream timeout timer */
        TimerDelete(rx_stream_timeout_tid);
        rx_stream_timeout_tid = TimerCreate(RX_STREAM_TIMEOUT, TRUE,
                                                        rxStreamTimeoutHandler);
    }
    else
    {
        /* End of stream */
        rx_stream_in_progress = FALSE;
        TimerDelete(rx_stream_timeout_tid);
        rx_stream_timeout_tid = TIMER_INVALID;
#ifdef ENABLE_WATCHDOG_MODEL
        WatchdogStart();
#endif /* ENABLE_WATCHDOG_MODEL */
    }
}

    
/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataBlockInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_BLOCK_IND message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleCSRmeshDataBlockInd(uint16 src_id, 
                                            CSRMESH_DATA_BLOCK_SEND_T *p_event)
{
     uint8 block_data_buffer_flag;
     uint8 block_data_total_flag;
     uint8 block_data_buffer[10];
     uint8 i = 0;
     uint8 j = 0;
     uint8 block_data_buffer_len;

     block_data_buffer_len = p_event->datagramoctets_len;
     for(i =0;i < block_data_buffer_len;i++)
     {
          block_data_buffer[i] = p_event->datagramoctets[i];    
     }
     block_data_buffer_flag = block_data_buffer[2] & 0x0f;/*接收顺序标志*/
     block_data_total_flag = block_data_buffer[2] & 0xf0;/*一次数据包含几包*/
     if(block_data_buffer[0] == 0x7E || block_data_buffer[0] == 0xE7)
     {
       if(f_Block_Buffer_Empty == ON)
       {
          f_Block_Buffer_Empty = OFF;
          tm_1s.tRxMeshTimeOut2s.word = C_T_tRxMeshTimeOut2s;/*启动接收2秒超时计时*/
          Rx_MessageID = block_data_buffer[1];
          app_block_state.rx.src_id = src_id;
          switch(block_data_buffer_flag)
          {
               case 0x01:
                    BLE_RX_DATA[0] =  block_data_buffer[0];
                    BLE_RX_DATA[1] =  block_data_buffer[1];
                    for(j = 2;j < (block_data_buffer_len-1);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j+1];                        
                    }
                    f_rxmeshdata01 = ON;
                    /*该次发送只包含一包数据*/
                    if(block_data_total_flag == 0x10)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR;
                    }
               break;
               case 0x02:
                    f_rxmeshdata02 = ON;
                    for(j = 9;j < (block_data_buffer_len+6);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-6];                        
                    }
               break;
               case 0x03:
                    f_rxmeshdata03 = ON;
                    for(j = 16;j < (block_data_buffer_len+13);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-13];                        
                    }
               break;
               case 0x04:
                    f_rxmeshdata04 = ON;
                    for(j = 23;j < (block_data_buffer_len+20);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-20];                        
                    }
               break;
               default:
               break;
          }
     }
     else
     {
          if(app_block_state.rx.src_id == src_id && block_data_buffer[1] == Rx_MessageID)
          {
               switch(block_data_buffer_flag)
               {
               case 0x01:
                    f_rxmeshdata01 = ON;
                    BLE_RX_DATA[0] =  block_data_buffer[0];
                    BLE_RX_DATA[1] =  block_data_buffer[1];
                    for(j = 2;j < (block_data_buffer_len-1);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j+1];                        
                    }
                    if(block_data_total_flag == 0x20 && f_rxmeshdata02 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR;
                     }
                    else if(block_data_total_flag == 0x30 && f_rxmeshdata02 == ON && f_rxmeshdata03 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR; 
                         f_rxmeshdata03 = CLEAR;
                    }
               break;
               case 0x02:
                    f_rxmeshdata02 = ON;
                    for(j = 9;j < (block_data_buffer_len+6);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-6];                        
                    }
                    if(block_data_total_flag == 0x20 && f_rxmeshdata01 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR;
                     }
                    else if(block_data_total_flag == 0x30 && f_rxmeshdata01 == ON && f_rxmeshdata03 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR; 
                         f_rxmeshdata03 = CLEAR;
                    }
               break;
               case 0x03:
                    f_rxmeshdata03 = ON;
                    for(j = 16;j < (block_data_buffer_len+13);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-13];                        
                    }
                    if(block_data_total_flag == 0x30 && f_rxmeshdata01 == ON && f_rxmeshdata02 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR;
                         f_rxmeshdata03 = CLEAR;
                     }
               break;
               case 0x04:
                    f_rxmeshdata04 = ON;
                    for(j = 23;j < (block_data_buffer_len+20);j++)
                    {
                         BLE_RX_DATA[j] =  block_data_buffer[j-20];                        
                    }
                    if(block_data_total_flag == 0x40 && f_rxmeshdata01 == ON && f_rxmeshdata02 == ON && f_rxmeshdata03 == ON)
                    {
                         f_meshrxdataOK = ON;   
                         f_rxmeshdata01 = CLEAR; 
                         f_rxmeshdata02 = CLEAR;
                         f_rxmeshdata03 = CLEAR;
                         f_rxmeshdata04 = CLEAR;
                     }
               break;
               default:
               break;
               }                    
          }
     }
   }
   if(f_meshrxdataOK == ON)
   {
        f_meshrxdataOK = OFF;
        /*接收完成后，等待100ms才能再次进入重新接收*/
        tm_100ms.tmeshfinishdataWait100ms.word = C_T_tmeshfinishdataWait100ms;
        MeshRxdCheck_New();
        if(f_MeshRxdCheckOk == ON)
        {
             f_MeshRxdCheckOk = OFF;
             RX_MESH_ID = app_block_state.rx.src_id;
             if(BLE_RX_DATA[0] == 0x7E)UartTxDataType = 0xEE;
             else if(BLE_RX_DATA[0] == 0xE7)UartTxDataType = 0xAA;
        }
               
   }     
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamDataInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_DATA_IND message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleCSRmeshDataStreamDataInd(uint16 src_id, 
                                            CSRMESH_DATA_STREAM_SEND_T *p_event)
{
    /* Restart the stream timeout timer */
    TimerDelete(rx_stream_timeout_tid);
    rx_stream_timeout_tid = TimerCreate(RX_STREAM_TIMEOUT, TRUE,
                                                        rxStreamTimeoutHandler);

    /* Set stream_in_progress flag to TRUE */
    rx_stream_in_progress = TRUE;

    MemCopy(&BLE_RX_DATA[rx_stream_offset],&p_event->streamoctets[0], p_event->streamoctets_len);
    rx_stream_offset += p_event->streamoctets_len;    
       
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamSendCfm
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_SEND_CFM message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void handleCSRmeshDataStreamSendCfm(
                                    CSRMESH_DATA_STREAM_RECEIVED_T *p_event)
{
    
    tx_stream_offset += app_stream_state.tx.last_data_len;
    /* Send next block if it is not end of string */
    sendNextPacket();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startStream
 *
 *  DESCRIPTION
 *      Initialises the stream model to start sending a data stream. 
 *      This function sets the receiver device ID to which the data is to be
 *      sent using the StreamSendData
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

extern void startStream(uint16 dest_id)
{
    CSRMESH_DATA_STREAM_FLUSH_T flush_param;
    app_stream_state.tx.dest_id = dest_id;


    app_stream_state.tx.sn = 0;
    tx_stream_offset = 0;
    app_stream_state.tx.status = stream_start_flush_sent;
    app_stream_state.tx.last_data_len = 0;
    device_info_length = BLE_TX_DATA_LENGTH;

    flush_param.streamsn = app_stream_state.tx.sn;
    tm_1s.tMeshTimeOut2s.word = C_T_tMeshTimeOut2s; /*启动发送超时2s计时*/ 
    DataStreamFlush(CSR_MESH_DEFAULT_NETID, dest_id, 
                    AppGetCurrentTTL(), &flush_param);
}
static void Handle01Data(void)
{
     uint8 i = 0;
     uint8 len = 0;
     Tx_Data_Buffer[0] =  BLE_TX_DATA[0];
     Tx_Data_Buffer[1] =  BLE_TX_DATA[1];
     Tx_Data_Buffer[2] =  (Block_Send_Num << 4) | 0x01;
     if(Block_Send_Num <= 1)len = BLE_TX_DATA_LENGTH;
     else len = 9;
     for(i = 3;i <= len;i++)
     {
          Tx_Data_Buffer[i] = BLE_TX_DATA[i-1];   
     }
}
static void Handle02Data(void)
{
     uint8 i = 0;
     uint8 len = 0;
     Tx_Data_Buffer[10] =  BLE_TX_DATA[0];
     Tx_Data_Buffer[11] =  BLE_TX_DATA[1];
     Tx_Data_Buffer[12] =  (Block_Send_Num << 4) | 0x02;
     if(Block_Send_Num <= 2 && Block_Send_Mod != 0)len = 12 + Block_Send_Mod;
     else len = 19;
     for(i = 13;i <= len;i++)
     {
          Tx_Data_Buffer[i] = BLE_TX_DATA[i-4];   
     }
}
static void Handle03Data(void)
{
     uint8 i = 0;
     uint8 len = 0;
     Tx_Data_Buffer[20] =  BLE_TX_DATA[0];
     Tx_Data_Buffer[21] =  BLE_TX_DATA[1];
     Tx_Data_Buffer[22] =  (Block_Send_Num << 4) | 0x03;
     if(Block_Send_Num <= 3 && Block_Send_Mod != 0)len = 22 + Block_Send_Mod;
     else len = 29;
     for(i = 23;i <= len;i++)
     {
          Tx_Data_Buffer[i] = BLE_TX_DATA[i-7];   
     }
}
static void Handle04Data(void)
{
     uint8 i = 0;
     uint8 len = 0;
     Tx_Data_Buffer[30] =  BLE_TX_DATA[0];
     Tx_Data_Buffer[31] =  BLE_TX_DATA[1];
     Tx_Data_Buffer[32] =  (Block_Send_Num << 4) | 0x04;
     if(Block_Send_Num <= 4 && Block_Send_Mod != 0)len = 32 + Block_Send_Mod;
     else len = 39;
     for(i = 33;i <= len;i++)
     {
          Tx_Data_Buffer[i] = BLE_TX_DATA[i-10];   
     }          
}
static void Mesh_TxData_Handle(void)
{
     Block_Send_Num = ((BLE_TX_DATA_LENGTH - 2) / 7); 
     Block_Send_Mod = ((BLE_TX_DATA_LENGTH - 2) % 7);
     Tx_Data_Buffer_len = Block_Send_Num * 10 + Block_Send_Mod + 3;
     if(Block_Send_Mod != 0)Block_Send_Num = Block_Send_Num+1;
     switch(Block_Send_Num)
     {
          case 1:
               Handle01Data();
          break;
          case 2:
               Handle01Data();
               Handle02Data();
          break;
          case 3:
               Handle01Data();
               Handle02Data();
               Handle03Data();
          break;
          case 4:
               Handle01Data();
               Handle02Data();
               Handle03Data();
               Handle04Data();
          break;
          default:
          break;
     }
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      StartBlockSendData
 *
 *  DESCRIPTION
 *      Initialises the stream model to start sending a data stream. 
 *      This function sets the receiver device ID to which the data is to be
 *      sent using the BlockSendData
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void StartBlockSendData(uint16 dest_id)
{
     uint16 data_pending, len;
     CSRMESH_DATA_BLOCK_SEND_T send_param;
     app_stream_state.tx.dest_id = dest_id;   
     app_stream_state.tx.sn = 0;
     Mesh_TxData_Handle();
     /*Tx_Data_Buffer_len = StrLen((char *)Tx_Data_Buffer);*/
     device_info_length = Tx_Data_Buffer_len;
     tx_stream_offset = 0;   
     data_pending = device_info_length - tx_stream_offset;  
     TimerDelete(block_send_retry_tid);
     block_send_retry_tid = TIMER_INVALID;
     tm_1s.tMeshTimeOut2s.word = C_T_tMeshTimeOut2s; /*启动发送超时2s计时*/ 
     
     if( data_pending )
     {
          len = (data_pending > MAX_DATA_BLACK_PACKET_SIZE)? 
                                MAX_DATA_BLACK_PACKET_SIZE : data_pending;
          MemCopy(send_param.datagramoctets, &Tx_Data_Buffer[tx_stream_offset],len);
          send_param.datagramoctets_len = len;

          DataBlockSend(CSR_MESH_DEFAULT_NETID,app_stream_state.tx.dest_id,
                        AppGetCurrentTTL(),&send_param); 
          app_stream_state.tx.last_data_len = len;
          
          block_send_retry_tid = TimerCreate(BLOCK_SEND_RETRY_TIME, TRUE,
                                                       blockSendRetryTimer);
     }
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      blockSendRetryTimer
 *
 *  DESCRIPTION
 *      Timer handler to retry sending next packet
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void blockSendRetryTimer(timer_id tid)
{
    uint16 data_pending, len;
    CSRMESH_DATA_BLOCK_SEND_T send_param;
    tx_stream_offset += app_stream_state.tx.last_data_len; 
    app_stream_state.tx.sn += app_stream_state.tx.last_data_len;
    data_pending = device_info_length - tx_stream_offset;  
    if( tid == block_send_retry_tid )
    {
        block_send_retry_tid = TIMER_INVALID;
        if( data_pending )
        {
              len = (data_pending > MAX_DATA_BLACK_PACKET_SIZE)? 
                     MAX_DATA_BLACK_PACKET_SIZE : data_pending; 
              MemCopy(send_param.datagramoctets, &Tx_Data_Buffer[tx_stream_offset],len);
              send_param.datagramoctets_len = len;
              DataBlockSend(CSR_MESH_DEFAULT_NETID,app_stream_state.tx.dest_id,
                            AppGetCurrentTTL(),&send_param); 
              app_stream_state.tx.last_data_len = len;
              block_send_retry_tid = TimerCreate(BLOCK_SEND_RETRY_TIME, TRUE,
                                                       blockSendRetryTimer);
         }
        else
        {
               tm_100ms.tmfTxdataWait100ms.word = C_T_tmfTxdataWait100ms;
               TimerDelete(block_send_retry_tid);  
               block_send_retry_tid = TIMER_INVALID;
        }

    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      endStream
 *
 *  DESCRIPTION
 *      Sends flush to end stream and updates the stream transmit state
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void endStream(void)
{
    CSRMESH_DATA_STREAM_FLUSH_T flush_param;
    if(app_stream_state.tx.status == stream_send_in_progress)
    {
        app_stream_state.tx.status = stream_finish_flush_sent;
        app_stream_state.tx.last_data_len = 0;
    }
    /* Send flush to end stream */
    flush_param.streamsn = app_stream_state.tx.sn;
    DataStreamFlush(CSR_MESH_DEFAULT_NETID, app_stream_state.tx.dest_id, 
                                    AppGetCurrentTTL(), &flush_param);
}

static void MeshRxdCheck_New(void)
{
  uint8 tempY = 1;
  MeshNowBuffer = 0;
  while(tempY < (BLE_RX_DATA[2] + 1))
  {
    MeshNowBuffer ^= BLE_RX_DATA[tempY];
    tempY++;
  }
  MeshNowBuffer = MeshNowBuffer & 0x00ff;
  if(MeshNowBuffer == BLE_RX_DATA[BLE_RX_DATA[2] + 1] && (BLE_RX_DATA[0] == 0x7E || BLE_RX_DATA[0] == 0xE7))
  {
     f_MeshRxdCheckOk = 1;
   }
  else
  {
    f_MeshRxdCheckOk = 0;
  }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appDataServerHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Data Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult appDataServerHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                          CSRMESH_EVENT_DATA_T* p_event,
                                          CsrUint16 length, void **state_data)
{
    uint16 sn;
    uint16 send_ack_msg = FALSE;/*cdy add*/
    uint16 b_use_msg = FALSE;

    switch(event_code)
    {
        /* Stream flush indication */
        case CSRMESH_DATA_STREAM_FLUSH:
        {
            CSRMESH_DATA_STREAM_FLUSH_T *p_flush_msg = 
                (CSRMESH_DATA_STREAM_FLUSH_T *)p_event->data;

            sn = p_flush_msg->streamsn;
            #ifdef DEBUG_ENABLE
            DebugWriteString("\r\n the sn is:");
            DebugWriteUint16(p_flush_msg->streamsn);
            #endif
            /*  If the state is no-stream, update nesn and the sender ID */
            if( app_stream_state.rx.status == stream_receive_idle && sn == 0)
            {
                /* The stream end flush could have been re-transmitted. 
                 * Ack the message. Move to a new stream only if the sn or the 
                 * rx.src_id does not match
                 */                 
                send_ack_msg = TRUE;
                app_stream_state.rx.status = stream_start_flush_received;
                app_stream_state.rx.src_id = p_event->src_id;
                app_stream_state.rx.nesn = sn;
                b_use_msg = TRUE;
                rx_stream_offset = 0;
            }
            else if(app_stream_state.rx.status == stream_start_flush_received)
            {
                /* We have already received a flush to start a stream. 
                 * Respond to the message if it is from the same source. 
                 * Ignore otherwise. App is already notified no need to notify
                 * again
                 */
                if( app_stream_state.rx.src_id == p_event->src_id)
                {
                    send_ack_msg = TRUE;
                }
            }
            else if(app_stream_state.rx.status == stream_receive_in_progress
                    && p_event->src_id == app_stream_state.rx.src_id)
            {
                /* Data stream already in progress. Check if the sn is equal to
                 * nesn and the sender is rx.src_id. Otherwise ignore flush
                 */                
                if(sn == app_stream_state.rx.nesn )
                {
                    /* End of stream */
                    send_ack_msg = TRUE;
                    b_use_msg = TRUE;
                    app_stream_state.rx.status = stream_receive_idle;
                    app_stream_state.rx.nesn = 0xffff; /*add by cdy*/
                    MeshRxdCheck_New();
                    if(f_MeshRxdCheckOk == ON)
                    {
                         f_MeshRxdCheckOk = OFF;
                         RX_MESH_ID = app_stream_state.rx.src_id;
                         if(BLE_RX_DATA[0] == 0x7E)UartTxDataType = 0xEE;
                         else if(BLE_RX_DATA[0] == 0xE7)UartTxDataType = 0xAA;
                         /*f_Mesh_Uart_Tx = ON;置串口发送标志*/
                    }
                    
                    #ifdef DEBUG_ENABLE
                    uint8 temp = 0;
                    DebugWriteString("\r\n");
				DebugWriteString("the data is:");
				for(temp = 0;temp < (BLE_RX_DATA[2]+3);temp++)
				{	   
					DebugWriteUint8(BLE_RX_DATA[temp]);				   
				}
                    #endif
                }
                else/* if(sn == 0)*/ 
                {
                    send_ack_msg = TRUE;
                    rx_stream_in_progress = FALSE;
                    resetRxStreamState();
                }
            }

            *state_data = NULL;
            if(send_ack_msg == TRUE)
            {
                /* Acknowledge the sender */
                *state_data = &app_stream_state.rx.nesn;
            }
            
            if( b_use_msg == TRUE)
            {
                /* Message is useful app */
                handleCSRmeshDataStreamFlushInd(p_flush_msg);
            }
        }
        break;

        case CSRMESH_DATA_STREAM_SEND:
        {
            CSRMESH_DATA_STREAM_SEND_T *p_stream_msg =
                                (CSRMESH_DATA_STREAM_SEND_T *)p_event->data;

            /* Accept only if a data stream is received for an on-going 
             * stream from rx.src_id
             */
            if(p_event->src_id == app_stream_state.rx.src_id )
            {
                app_stream_state.rx.status = stream_receive_in_progress;
                sn = p_stream_msg->streamsn;

                if( sn == app_stream_state.rx.nesn )
                {
                    /* Update nesn */
                    app_stream_state.rx.nesn += p_stream_msg->streamoctets_len;

                    /* Process data */
                    handleCSRmeshDataStreamDataInd(p_event->src_id,
                                                   p_stream_msg);
                }

                /* Acknowledge the sender */
                *state_data = &app_stream_state.rx.nesn;
            }
            else
            {
                /* A stream transfer is ongoing with another device. Or 
                 * a Flush to start a stream was not received.
                 * Ignore the data stream send.
                 */
                *state_data = NULL;
            }
        }
        break;
        /* Received a single block of data */
        case CSRMESH_DATA_BLOCK_SEND:
        {
            CSRMESH_DATA_BLOCK_SEND_T *p_block_send =
                (CSRMESH_DATA_BLOCK_SEND_T *)p_event->data;
            handleCSRmeshDataBlockInd(p_event->src_id, p_block_send);
        }
        break;

        default:
        break;
    }
    return CSR_MESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appDataClientHandler
 *
 *  DESCRIPTION
 *      Application function to handle CSRmesh Data Model messages
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static CSRmeshResult appDataClientHandler(CSRMESH_MODEL_EVENT_T event_code, 
                                          CSRMESH_EVENT_DATA_T* p_event,
                                          CsrUint16 length,
                                          void **state_data)
{
    uint16 nesn;
    #ifdef DEBUG_ENABLE
    DebugWriteString("\r\n the tx.status is:");
    DebugWriteUint16(app_stream_state.tx.status); 
    #endif
    switch(event_code)
    {
        /* Received a repsonse to a flush or a data_stream */
        case CSRMESH_DATA_STREAM_RECEIVED:
        {
            CSRMESH_DATA_STREAM_RECEIVED_T *p_data_rcvd =
                      (CSRMESH_DATA_STREAM_RECEIVED_T *)p_event->data;
            /* Handle the message only if it is addressed to self device ID */
            /* Get the nesn from ack */
            nesn = p_data_rcvd->streamnesn;

            #ifdef DEBUG_ENABLE
            DebugWriteString("\r\n the nesn is:");
            DebugWriteUint16(nesn); 
            DebugWriteString("\r\n the tx.sn is:");
            DebugWriteUint16(app_stream_state.tx.sn); 
            #endif
            
            if(app_stream_state.tx.status == stream_start_flush_sent &&
               nesn == 0)
            {
                /* Received the acknowledgement for the stream flush
                 * sent to start streaming
                 */
                app_stream_state.tx.status = stream_send_in_progress;

                /* If there is any stream packet pending, send it */
                handleCSRmeshDataStreamSendCfm(p_data_rcvd);
            }
            else if(app_stream_state.tx.status == stream_start_flush_sent && nesn != 0)
            {
                /* Received the acknowledgement for the stream flush sent
                 * to finish the stream
                 */
                //startStream(0x8001);
            }
            else if(app_stream_state.tx.status == stream_finish_flush_sent &&
               nesn == 0xffff)
            {
                /* Received the acknowledgement for the stream flush sent
                 * to finish the stream
                 */
                resetTxStreamState();
                f_SendMSGOK = ON; /* 置发送完成标志*/
                /*nesn = 0; *//*add by cdy*/
            }
            else if(app_stream_state.tx.status == stream_finish_flush_sent &&
               nesn != 0xffff)
            {
                resetTxStreamState();
                f_SendMSGNG = ON; /*置发送错误标志*/
            }
            /* nesn must be tx.sn + tx.last_data_len */
            else if(app_stream_state.tx.status == stream_send_in_progress &&
                    nesn == app_stream_state.tx.sn +
                                  app_stream_state.tx.last_data_len)
            {
                app_stream_state.tx.sn += app_stream_state.tx.last_data_len;
                /* If there is any stream packet pending, send it */
                handleCSRmeshDataStreamSendCfm(p_data_rcvd);
            }
        }
        break;

        default:
        break;
    }
    return CSR_MESH_RESULT_SUCCESS;
}



/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppDataStreamInit
 *
 *  DESCRIPTION
 *      This function initializes the stream Model.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void DataModelHandlerInit(CsrUint8 nw_id,
                                 uint16 model_groups[],
                                 CsrUint16 num_groups)
{
    /* Register both both data client and server as we support both send
     * and receive stream
     */
    /* Server */
    DataModelInit(nw_id, model_groups, num_groups, appDataServerHandler);
    /* Client */
    DataModelClientInit(appDataClientHandler);

    /* Reset timers */
    stream_send_retry_tid = TIMER_INVALID;
    rx_stream_timeout_tid = TIMER_INVALID;

    /* Reset the device info */
    device_info_length = sizeof(BLE_RX_DATA);/*cdy add*/

    /* Initialise stream state */
    app_stream_state.rx.src_id = 0;
    app_stream_state.rx.status = stream_receive_idle;
    /*cdy add*/
    app_stream_state.rx.nesn = 0xffff;
    
    app_stream_state.tx.dest_id = 0;
    app_stream_state.tx.status = stream_send_idle;
    app_stream_state.tx.last_data_len = 0;
    /*cdy add*/
    app_stream_state.tx.sn = 0;
    
    /*MemCopy(&device_info[2], DEVICE_INFO_STRING, sizeof(DEVICE_INFO_STRING));*/
}
#endif /* ENABLE_DATA_MODEL */
