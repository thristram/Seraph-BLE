/******************************************************************************
 *  Copyright (c) 2016 donse young
 *
 *****************************************************************************/
/*============================================================================*
 *  Local Header File
 *============================================================================*/


#include <uart.h>
#include <sleep.h>
#include <data_client.h>
#include <panic.h>
#include <mem.h>
#include "data_model_handler.h"
#include "label.h"
#include "macros.h"

#include "define.h"
#include "byte_queue.h"
#include "app_debug.h"
#include "app_mesh_handler.h"
/*#include "debug_interface.h"*/  
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
/*#define HIGH_BAUD_RATE                   (0x0014) 4800*/
#define HIGH_BAUD_RATE                   (0x00eb) /* 57600*/
/*#define HIGH_BAUD_RATE                   (0x01d9)  115200*/
#define TIME30S (30*SECOND)
static uint16 uartRxDataCallback(void   *p_rx_buffer,
                                 uint16  length,
                                 uint16 *p_req_data_length);

static void uartTxDataCallback(void);
static void WifiRxdCheck_New(void);
static void sendPendingData(void);
static void WifiTxDataClear(void);
static void WifiTxGetBcc(void);
static void WifiRxdDataDo_New(void);
void SendDataToUart(void);
extern void AppGetState(void);

void WifiRxDataClear(void);
void processdata(void);
/*extern void startStream(uint16 dest_id);*/
static void WifiTxDataEEEE(void);
static void WifiTxDataDD(void);
static void CLEAR_BLE_RX_DATA(void);
static void WifiRxDataEEAA(void);
static void WifiRxDataEEEE(void);
static void WifiTxDataEEAA(void);
static void CLEAR_BLE_TX_DATA(void);

/* Create 256-byte receive buffer for UART data */
UART_DECLARE_BUFFER(rx_buffer, UART_BUF_SIZE_BYTES_64);

/* Create 256-byte transmit buffer for UART data */
UART_DECLARE_BUFFER(tx_buffer, UART_BUF_SIZE_BYTES_64);
/*#define SERIAL_RX_DATA_LENGTH           (20)*/
/*uint8 uart_rx_buffer[30];*/
/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
static void sendPendingData(void)
{
    uint16 length = BQGetDataSize(RECV_QUEUE_ID);
    uint8  data[40]; 
    uint16 size_val;
     /* Loop until the byte queue is empty */ 
    while (BQGetDataSize(RECV_QUEUE_ID) > 0)
    {     
        size_val = length > SERIAL_RX_DATA_LENGTH ?
                   SERIAL_RX_DATA_LENGTH : length;
         /* Read the next byte in the queue */
        if (BQPeekBytes(data,size_val,RECV_QUEUE_ID) > 0)
        {
            bool ok_to_commit = UartWrite( data,size_val);
            if(!ok_to_commit)
            {
                /* exit on failure */
                break;
            }
            else
                 BQPopBytes(data, size_val,RECV_QUEUE_ID);
        }
    }
    /*发送完成后，等待2ms*/
    tm_1ms.tWifiTwoByteSentTime.word = C_T_WifiTwoByteSentTime;
    /*启动接收160ms计时*/
    
}

void SendDataToUart(void)
{

    uint16 size_val = 0;
    uint8 txdata[40];
    uint8 i =0;
    if(WifiTxData[1].byte == 0xEE || WifiTxData[1].byte == 0xAA)
         size_val = WifiTxData[5].byte+3;
    else if(WifiTxData[0].byte == 0xDD && WifiTxData[1].byte == 0xDD)
         size_val = WifiTxData[3].byte+3;
    
    /*启动串口发送4ms计时*/
    tm_4ms.twifitstxcyc.word = C_T_wifitstxcyc;
    while(i < 40)
    {
         txdata[i] = WifiTxData[i].byte;
         i++;
    }
         /* First copy all the bytes received into the byte queue */
    BQSafeQueueBytes(txdata, size_val,RECV_QUEUE_ID);
    sendPendingData();

}

static uint16 uartRxDataCallback(void   *p_rx_buffer,
                                 uint16  length,
                                 uint16 *p_additional_req_data_length)
{  

        if ( length > 0 && tm_1ms.tUartWait10ms.fov == ON)
        {
        /* First copy all the bytes received into the byte queue */
        BQForceQueueBytes((const uint8 *)p_rx_buffer, length,SEND_QUEUE_ID);

        }
       
    /* Send any pending data waiting to be sent */
        while (BQGetDataSize(SEND_QUEUE_ID) > 0)
        {
            uint8 byte = '\0';        
        /* Read the next byte in the queue */
            if (BQPeekBytes(&byte, 1,SEND_QUEUE_ID) > 0)
            {
               WifiRxData[WifiDataCount].byte = byte;
               if(WifiDataCount <= 5)WifiDataCount++;
               else 
               {
                    WifiDataCount++;
                    if(WifiDataCount >= (WifiRxData[5].byte+3))
                    {
                         WifiDataCount = CLEAR;
                         f_rxdataOK = ON;
                         BLE_TX_DATA_LENGTH = WifiRxData[5].byte+3;
                         tm_1ms.tUartWait10ms.word = C_T_tUartWait10ms;
                    }
                }     
                    
               /*接收完一个字节后，启动1ms计时*/
               tm_1ms.tWifitsfrmwt.word = C_T_tWifitsfrmwt;
               BQCommitLastPeek(SEND_QUEUE_ID);
            }
            else
            {
               break;
            }
         }
    
    /* Inform the UART driver that we'd like to receive another byte when it
     * becomes available
     */
       *p_additional_req_data_length = (uint16)1;    

    /* Return the number of bytes that have been processed */
    return length;
}

static void uartTxDataCallback(void)
{
     sendPendingData();
}

void InitUart(void)
{
    
     UartInit(uartRxDataCallback,
              uartTxDataCallback,
              rx_buffer, UART_BUF_SIZE_BYTES_64,
              tx_buffer, UART_BUF_SIZE_BYTES_64,
              uart_data_unpacked);  
     /*SleepWakeOnUartRX(TRUE);*/
/* Configure the UART for high baud rate */
     UartConfig(HIGH_BAUD_RATE,0);
/* Enable the UART back */
     UartEnable(TRUE);
/* Read from UART */
     UartRead(1,0);
     /*串口测试使用
     const uint8 message[] = "\r\nType something: ";
     BQForceQueueBytes(message, sizeof(message)/sizeof(uint8));
     sendPendingData();
     */
/* Disable deep sleep, so characters dont go missing  */
     /*SleepModeChange(sleep_mode_never);*/
     /*串口初始化成功后，等待3秒后开始收发数据*/
     tm_1s.tOnTimeWait3s.word = C_T_OnTimeWait3s;
     tm_100ms.tWifiSendData.word = C_T_WifiSendData;  
}

static void WifiTxDataClear(void)
{
  unsigned char tempY = 0;
  while(tempY < 40)
  {
    WifiTxData[tempY].byte = 0;
    tempY++;
   }
}
void WifiRxDataClear(void)
{
  unsigned char tempY = 0;
  while(tempY < 40)
  {
    WifiRxData[tempY].byte = 0;
    tempY++;
   }
}

static void WifiTxGetBcc(void)
{
  unsigned char tempY = 2;
  uint8 size_val = 0;
  WifiNowBuffer = 0;
  if(WifiTxData[1].byte == 0xEE || WifiTxData[1].byte == 0xAA)
         size_val = WifiTxData[5].byte+2;
  else if(WifiTxData[0].byte == 0xDD && WifiTxData[1].byte == 0xDD)
         size_val = WifiTxData[3].byte+2;
  
  while(tempY < size_val)
  {
    WifiNowBuffer ^= WifiTxData[tempY].byte;
    tempY++;
  }
  WifiTxData[size_val].byte = WifiNowBuffer;
}

static void WifiRxdCheck_New(void)
{
  uint8 tempY = 2;
  WifiNowBuffer = 0;
  while(tempY < (WifiRxData[5].byte + 2))
  {
    /*WifiNowBuffer = WifiNowBuffer + WifiRxData[tempY].byte;*/
    WifiNowBuffer ^=  WifiRxData[tempY].byte;   
    tempY++;
  }
  WifiNowBuffer = (WifiNowBuffer & 0x00ff);
  if(WifiNowBuffer == WifiRxData[WifiRxData[5].byte + 2].byte
    && ((WifiRxData[0].byte == 0xEE && WifiRxData[1].byte == 0xEE)
        || (WifiRxData[0].byte == 0xEE && WifiRxData[1].byte == 0xAA)))
  {
     f_WifiRxdCheckOk = 1;
   }
  else
  {
    f_WifiRxdCheckOk = 0;
  }
}
/*
static void BLE_RX_GCC(void)
{
     unsigned char tempY = 1;
     WifiNowBuffer = 0;
     while(tempY < (BLE_RX_DATA[2] + 1))
     {
          WifiNowBuffer = WifiNowBuffer + BLE_RX_DATA[tempY];
          tempY++;
     }
     BLE_RX_DATA[BLE_RX_DATA[2]+1] = WifiNowBuffer;
}
*/
static void BLE_TX_GCC(void)
{
     unsigned char tempY = 1;
     WifiNowBuffer = 0;
     while(tempY < (BLE_TX_DATA[2] + 1))
     {
          WifiNowBuffer ^= BLE_TX_DATA[tempY];
          tempY++;
     }
     BLE_TX_DATA[BLE_TX_DATA[2]+1] = WifiNowBuffer;     
}

static void WifiRxDataEEEE(void)
{
     uint8 i = 0;
     BLE_TX_DATA[0] = 0x7E;
     BLE_TX_DATA[1] = WifiRxData[2].byte;
     /*Tx_MessageID = WifiRxData[2].byte;*/
     TX_MESH_ID = 0x0000;
     TX_MESH_ID = (((uint16)WifiRxData[3].byte << 8)|(uint16)WifiRxData[4].byte);
     BLE_TX_DATA[2] = WifiRxData[5].byte - 2;
     for(i = 3;i <(BLE_TX_DATA[2] +1);i++)
     {
          BLE_TX_DATA[i] = WifiRxData[i+3].byte;          
     }
     /*UartTxDataType = 0xEA;  *//*接收到网关返回来的信息后，再置该值 */
     BLE_TX_GCC();
     BLE_TX_DATA_LENGTH = BLE_TX_DATA[2] + 2;
     /*BLE_TX_DATA_LENGTH = StrLen((char *)BLE_TX_DATA);*/
     f_Mesh_Tx_Ready = ON;  
}

static void WifiRxdDataDo_New(void)
{         
     if(WifiRxData[0].byte == 0xEE)
     {
          if(WifiRxData[1].byte == 0xEE)WifiRxDataEEEE();
          if(WifiRxData[1].byte == 0xAA)WifiRxDataEEAA();
     }
}
static void WifiRxDataEEAA(void)
{
     uint8 temp;
     CLEAR_BLE_TX_DATA();
     BLE_TX_DATA[0] = 0xE7;
     BLE_TX_DATA[1] = WifiRxData[2].byte; /*message ID*/
     BLE_TX_DATA[2] = WifiRxData[5].byte - 2;/*没有mesh id,因此减2*/
     BLE_TX_DATA[3] = WifiRxData[6].byte;/*command*/
     BLE_TX_DATA[4] = WifiRxData[7].byte;/*command type*/
     for(temp = 5;temp < (BLE_TX_DATA[2]+1);temp++)
     {
          BLE_TX_DATA[temp] =  WifiRxData[temp+3].byte;              
     }
     BLE_TX_GCC();
     BLE_TX_DATA_LENGTH = BLE_TX_DATA[2] + 2;
     /*BLE_TX_DATA_LENGTH = StrLen((char *)BLE_TX_DATA);*/
     TX_MESH_ID = CLEAR;
     TX_MESH_ID = (((uint16)WifiRxData[3].byte << 8)|(uint16)WifiRxData[4].byte);
     f_RxEEData = ON;
     tm_100ms.tSendEEWait800ms.word = CLEAR;
     WifiTxDataEEEECount = CLEAR;
     f_Mesh_Tx_Ready = ON; /*置mesh发送标志，准备发送数据*/
}

static void CLEAR_BLE_TX_DATA(void)
{
     uint8 i = 0;
     while(i < sizeof(BLE_TX_DATA))
     {
         BLE_TX_DATA[i] = 0;
         i++;
     }
}
static void CLEAR_BLE_RX_DATA(void)
{
     uint8 i = 0;
     while(i < sizeof(BLE_RX_DATA))
     {
         BLE_RX_DATA[i] = 0;
         i++;
     }
}

static void WifiTxDataDD(void)
{
     WifiTxData[0].byte = 0xDD;
     WifiTxData[1].byte = 0xDD;
     WifiTxData[2].byte = 0x00;
     WifiTxData[3].byte = 0x0A; /*byte 2,byte3 为帖长*/
     WifiTxData[4].byte = 0x01; /*指令command*/
     WifiTxData[5].byte = Signal_Intensity;/*信号强度*/
     WifiTxData[6].byte = Mesh_status;/*mesh连接状态*/
     WifiTxData[7].byte = Con_Mobile_Num;/*连接手机的数量*/
     WifiTxData[8].byte = (Local_MESH_ID >> 8) & 0xff;/*mesh id高位*/
     WifiTxData[9].byte = Local_MESH_ID & 0xff;/*mesh id低位*/
     WifiTxData[10].byte = (Gateway_MsgID >> 8) & 0xff;/*网关 mesh id高位*/
     WifiTxData[11].byte = Gateway_MsgID & 0xff;/*网关 mesh id低位*/
     WifiTxGetBcc(); 
     
}
static void WifiTxDataEEAA(void)
{
     uint8 i = 0;
     WifiTxData[0].byte = 0xEE;
     WifiTxData[1].byte = 0xAA; 
     WifiTxData[2].byte = BLE_RX_DATA[1]; /*随机序列号*/
     WifiTxData[4].byte = RX_MESH_ID & 0x00FF;
     WifiTxData[3].byte = (RX_MESH_ID >> 8) & 0x00FF;/*MESH_ID*/
     RX_MESH_ID = CLEAR; /*清RX_MESH_ID缓冲区*/
     WifiTxData[5].byte = BLE_RX_DATA[2] + 2;/*帖长，由于增加了MESH_ID，因此为BLE_RX_DATA[2]+2*/
     for(i = 6;i< (WifiTxData[5].byte + 2);i ++)
     {
         WifiTxData[i].byte = BLE_RX_DATA[i-3];               
     }
     CLEAR_BLE_RX_DATA();
     WifiTxGetBcc();
}
static void WifiTxDataEEEE(void)
{
     uint8 i = 0;
     WifiTxData[0].byte = 0xEE;
     WifiTxData[1].byte = 0xEE;
     WifiTxData[2].byte = BLE_RX_DATA[1]; /*随机序列号*/
     WifiTxData[4].byte = RX_MESH_ID & 0x00FF;
     WifiTxData[3].byte = (RX_MESH_ID >> 8) & 0x00FF;/*MESH_ID*/
     RX_MESH_ID = CLEAR; /*清RX_MESH_ID缓冲区*/
     WifiTxData[5].byte = BLE_RX_DATA[2] + 2;/*帖长，由于增加了MESH_ID，因此为BLE_RX_DATA[2]+2*/
     for(i = 6;i< (WifiTxData[5].byte + 2);i ++)
     {
         WifiTxData[i].byte = BLE_RX_DATA[i-3];               
     }
     CLEAR_BLE_RX_DATA();
     WifiTxGetBcc();
     f_RxEEData = CLEAR; /*接收标志位清0*/
     tm_100ms.tSendEEWait800ms.word = C_T_tSendWait800ms;/*启动800ms接收时间定时 */
     WifiTxDataEEEECount++;/*20次接收标志+1*/
}

void processuartdata(void)
{
	/*
     if(tm_100ms.tSendEEWait800ms.fov == ON)
     {
         if(WifiTxDataEEEECount < 20)UartTxDataType = 0xEE;
         else
         {
              f_UartComErr = ON;
              WifiTxDataEEEECount = CLEAR;
              tm_100ms.tSendEEWait800ms.word = CLEAR;
         }
     }*/
     
     if(tm_1s.tOnTimeWait3s.fov == ON)
     {
          if(tm_100ms.tWifiSendData.fov == ON)
          {
               CommState = cTxdPrepare;
               CommRxState = cRxdPrepare;
               tm_100ms.tWifiSendData.word = CLEAR;
          }
          else
          {
               switch(CommState)
               {
                  case cTxdPrepare:
                  WifiTxDataClear(); 
                  if(UartTxDataType == 0xEE)
                  {
                           WifiTxDataEEEE();/*发送EE数据包*/   
                           UartTxDataType = CLEAR;        
                           CommState = cTxd;                         
                  }
                  else if(UartTxDataType == 0xAA)
                  {
                           WifiTxDataEEAA();/*发送AA数据包*/    
                           UartTxDataType = CLEAR;        
                           CommState = cTxd;                         
                  }
                  else if(UartTxDataType == 0xDD)
                  {
                           WifiTxDataDD();/*发送DD数据包*/ 
                           UartTxDataType = CLEAR;        
                           CommState = cTxd;                        
                  }
                  else if(tm_1s.tHeartPack5s.fov == ON)
                  {
                           
                           tm_1s.tHeartPack5s.word = C_T_tHeartPack5s;   
                          WifiTxDataDD();/*发送DD数据包*/ 
                           CommState = cTxd;                      
                  }                      
                  break;
                  case cTxd:
                    SendDataToUart();                   
                    CommState = cTxdEnd;
                  break;
                  case cTxdEnd:
                    if(tm_1ms.tWifiTwoByteSentTime.fov == ON || tm_4ms.twifitstxcyc.fov == ON)
                    {
                         CommState = cTxdPrepare;
                    }
                  break;
                  default:
                  break;
               } 
               
          switch(CommRxState)
          {
               case cRxdPrepare:
                    WifiRxDataClear();
                    WifiDataCount = CLEAR;
                    tm_1ms.tWifitsfrmwt.word = CLEAR;
                    /*tm_4ms.twifitsrxcyc.word = C_T_wifitsrxcyc;*/
                    CommRxState = cRxdWait;
               break;
               case cRxdWait:
                    if(f_rxdataOK == ON)
                    {
                         f_rxdataOK = OFF;
                         CommRxState = cRxd;
                    }                   
                    else if((tm_1ms.tWifitsfrmwt.fov == ON)/* ||(tm_4ms.twifitsrxcyc.fov == ON)*/)
                    {
                         CommRxState = cRxdPrepare;
                         /*f_Uart_TxRxError = ON;*/
                    }         
               break;
               case cRxd:
                    WifiRxdCheck_New();
                    if(f_WifiRxdCheckOk)
                    {
                         f_WifiRxdCheckOk = OFF;
                         tm_1ms.tWifi2mscyc.word = C_T_Wifi2mscyc;
                         WifiRxdDataDo_New();
                         CommRxState = cRxdEnd;
                    }
                    else
                    {
                         f_Uart_TxRxError = ON;
                         CommRxState = cRxdPrepare;
                    }
               break;
               case cRxdEnd:
                    if(tm_1ms.tWifi2mscyc.fov == ON)
                    {
                         CommRxState = cRxdPrepare;
                    }          
               break;  
               default:
               break;
          }     
               
          }
     }   
     
}












