/******************************************************************************
 *  Copyright (c) 2016 by donse young
 *
 *  FILE
 *      label.h
 *
 *  DESCRIPTION
 *      This file is variable definition
 *
 *****************************************************************************/

#ifndef __LABEL_H__
#define __LABEL_H__
#ifdef NO_DEFINEN 
#define extern  
#endif
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <sys_events.h>
#include <timer.h>
#include "typedef.h"
/*============================================================================*
 *  variable INIT
 *============================================================================*/

extern void timer_RBINT(void);
extern void time_main(void);
extern void InitUart(void);
extern void processuartdata(void);
extern void processMeshData(void);
extern void srf_init(void);
/*extern void endStream(void);*/
extern void init_stream_state(void);
extern void Check_Flush_nesn(void);
extern void PioSetMode_Bottun(void);
extern void HandlePIOChangedEvent(pio_changed_data *pio_data);
extern void handleExtraLongButtonPress(timer_id tid);
extern void Reset_BLE_Module(void);
extern void StartBlockSendData(uint16 dest_id);
extern uint8 timer250us;
extern uint8 MTimer;
extern uint8 m4msCount;
extern uint8 m100msCount;
extern uint8 m1sCount;
extern uint8 CommState;
extern uint8 CommRxState;
extern uint8 UartTxDataType;
extern uint8 WifiNowBuffer;
extern uint8 MeshNowBuffer;

/*
extern uint8 WifiRxDataOrder; 
extern uint8 House_Length;  
extern uint8 House_Width;  
extern uint8 House_Height;
extern uint8 Indoor_CoolNoise;
extern uint8 Indoor_HeatNoise;
extern uint8 SetSleepStats;
extern uint8 SetAirChange;
extern uint8 FDegreeAdjust;
extern uint8 SetASwingStates;
extern uint8 SetCSwingStates;
extern uint8 TempDispType;
extern uint8 SetWhistFan;
extern uint8 rxdtat_lenth;
extern uint8 Transmit_Strength;
extern uint8 Air_Quality; 
extern uint8 Air_PM2o5;
extern uint8 SetTempForFreq;
extern uint8 SetTimerForFreq;
extern uint8 Com_Version;
extern uint8 Com_Speed;
extern uint8 InDoor_Abi_Code; 
extern uint8 Power_Type;
extern uint8 Air_Cond_Type;
extern uint8 FanMotoType;  
extern uint8 FanMotoStep; 
extern uint8 Jump_Number;
extern uint16 self_dev_id;
extern uint8 BLE_StreamSN;
extern uint16 f_500MSWAIT;
extern uint8 f_SendFlush3times;
*/
extern uint8 SetMode;
extern uint8 SetFan; 
extern uint8 SetTemp;
extern uint8 TimerHour; 
extern uint8 TimerMin;  
extern bool g_trigger_write_callback;
extern uint8 WifiDataCount;
extern uint32 Device_Id_Num;  
extern uint8 User_Area; 
extern uint8 ModWorkMode;   /*无线模块工作模式*/
extern uint8 Signal_Strength;
extern uint8 Check_Data_Type;
extern uint16 InPm2p5Value;
extern uint16 Sieve_Time; /*滤网剩余时间*/
extern uint16 OutPm2p5Value;
extern uint8 BLE_RX_DATA_LENGTH;
extern uint8 BLE_TX_DATA_LENGTH;
extern uint8 WifiTxDataEEEECount;
extern uint8 WifiTxDataEEAACount;
extern uint8 WifiTxData82Count;
extern uint8 BLE_RX_DATA[40];
extern uint8 BLE_TX_DATA[40];
extern uint16 RX_MESH_ID;
extern uint16 TX_MESH_ID;
extern uint16 Local_MESH_ID;
extern uint8 Light_Intensity;/*光亮度*/
extern uint8 Tx_MessageID;
extern uint8 Rx_MessageID;
extern uint8 Command;
extern uint8 Gateway_MsgID;/*网关的message ID*/
extern uint8 Con_Mobile_Num;/*网关连接手机数量*/
extern uint8 Mesh_status;/*MESH 连接状态*/
extern uint8 Signal_Intensity;/*信号强度*/

/*
extern uint8 BLE_TX_DATA[50];
*/
extern volatile union FLAGS flag1;
#define f_Time1ms             flag1.bit.bit0
#define f_Time4ms             flag1.bit.bit1
#define f_Time10ms            flag1.bit.bit2
#define f_Time20ms            flag1.bit.bit3

extern volatile union FLAGS flag2;
#define f_Set_Light          flag2.bit.bit0 
#define f_Ble_Reset          flag2.bit.bit1 
#define f_RxEEData           flag2.bit.bit2
#define f_Bar_Code_En        flag2.bit.bit3 
#define f_Device_En          flag2.bit.bit4 
#define f_ProdId_En          flag2.bit.bit5 
#define f_RxEAData           flag2.bit.bit6 
#define f_SetOnOff           flag2.bit.bit7  /*it is use*/

extern volatile union FLAGS flag3;
#define f_rxmeshdata01            flag3.bit.bit0  
#define f_rxmeshdata02            flag3.bit.bit1  
#define f_rxmeshdata03            flag3.bit.bit2  
#define f_rxmeshdata04            flag3.bit.bit3  
/*
extern volatile union FLAGS flag4;
#define f_SwingAct               flag4.bit.bit0 
#define f_RunEightHeat           flag4.bit.bit1 
#define f_Set20hr                flag4.bit.bit2 
#define f_Set10hr                flag4.bit.bit3 
#define f_Setdot5hr              flag4.bit.bit4 
#define f_SysDryBlow             flag4.bit.bit5 
#define f_SysEHeat               flag4.bit.bit6
#define f_SysHealth              flag4.bit.bit7
*/
extern volatile union FLAGS flag5;
#define f_SetLight               flag5.bit.bit0 /*it is use*/
#define f_Mesh_Uart_Tx           flag5.bit.bit1
#define f_BleOnOff               flag5.bit.bit3 
#define f_BleReset               flag5.bit.bit4 /*it is use*/ 
#define f_Mesh_Tx_Ready          flag5.bit.bit5
#define f_Mesh_First_Send        flag5.bit.bit6
#define f_Block_Buffer_Empty     flag5.bit.bit7

extern volatile union FLAGS flag6;
/*#define f_EnergySave             flag6.bit.bit0 
#define f_CoAnnunciator          flag6.bit.bit1 
#define f_GeaFeal                flag6.bit.bit2 
#define f_SysSE                  flag6.bit.bit3 
#define f_DefModeV9_1            flag6.bit.bit4 
#define f_WifiIrOk               flag6.bit.bit5 */
#define f_meshrxdataOK           flag6.bit.bit6
#define f_rxdataOK               flag6.bit.bit7

extern volatile union FLAGS flag7;
#define f_WifiRxdCheckOk         flag7.bit.bit0
#define f_Uart_TxRxError         flag7.bit.bit1
#define f_MeshRxdCheckOk         flag7.bit.bit2
/*#define f_Repeat_Send            flag7.bit.bit2
#define f_GetIndoorinf           flag7.bit.bit3
#define f_ChangeAir              flag7.bit.bit4
#define f_CoolHeat               flag7.bit.bit5
#define f_FreType                flag7.bit.bit6
#define f_Pow_Supply             flag7.bit.bit7*/
/*
extern volatile union FLAGS flag8;
#define f_Pow_Freq               flag8.bit.bit0
#define f_Room_temp              flag8.bit.bit1
#define f_Tube_temp              flag8.bit.bit2
#define f_HasMemory              flag8.bit.bit3
#define f_HasEleDust             flag8.bit.bit4
#define f_HasEHeat               flag8.bit.bit5
#define f_HasHealth              flag8.bit.bit6
#define f_HasAir                 flag8.bit.bit7
*/

extern volatile union FLAGS flag9;
/*#define f_HasSE                  flag9.bit.bit0
#define f_FanMotoAct             flag9.bit.bit1
#define f_RecFlushOn             flag9.bit.bit2
#define f_RecFlushOK             flag9.bit.bit3*/
#define f_SendMSGNG              flag9.bit.bit4
#define f_SendMSGOK              flag9.bit.bit5

extern volatile union FLAGS flag10;
#define f_PM2p5                  flag10.bit.bit0
#define f_Associate              flag10.bit.bit1
#define f_HomeWifiState          flag10.bit.bit2
#define f_ConCloud               flag10.bit.bit3
#define f_UartComErr             flag10.bit.bit4
#define f_BleSetTime             flag10.bit.bit5

extern volatile union FLAGS flag11;
#define f_Rx83Data                  flag11.bit.bit0
#define f_Rx94Data                  flag11.bit.bit1
#define f_Rx95Data                  flag11.bit.bit2
#define f_Rx82Data                  flag11.bit.bit3
#define f_Rx84Data                  flag11.bit.bit4
#define f_Rx92Data                  flag11.bit.bit5
#define f_Rx93Data                  flag11.bit.bit6
#define f_Rx91Data                  flag11.bit.bit7

extern volatile union FLAGS flag12;
#define f_Tx83Data                  flag12.bit.bit0
#define f_Tx94Data                  flag12.bit.bit1
#define f_Tx95Data                  flag12.bit.bit2
#define f_Tx82Data                  flag12.bit.bit3
#define f_Tx84Data                  flag12.bit.bit4
#define f_Tx92Data                  flag12.bit.bit5
#define f_Tx93Data                  flag12.bit.bit6
#define f_Tx91Data                  flag12.bit.bit7

extern volatile union FLAGS flag13;
#define f_H_Pre_E1                  flag13.bit.bit0
#define f_F_Motor_E2                flag13.bit.bit1
#define f_Tem_Pro_E3                flag13.bit.bit2
#define f_Hum_Pro_E4                flag13.bit.bit3
#define f_BJT_Tem_E5                flag13.bit.bit4
#define f_Bord_Pm2p5_E6             flag13.bit.bit5
#define f_Comm_Err_E7               flag13.bit.bit6
#define f_IC_Err_E8                 flag13.bit.bit7

extern volatile union FLAGS flag14;
#define f_V_FeBak_D1                flag14.bit.bit0
#define f_H_V_Mod_D2                flag14.bit.bit1
#define f_In_Pm2p_D3                flag14.bit.bit2
#define f_Out_Pm2p_D4               flag14.bit.bit3
#define f_Repeat84Pack              flag14.bit.bit4
#define f_TX91BleData               flag14.bit.bit5
/*
extern volatile union FLAGS WifiRxIr_u;
#define WifiRxIr                   WifiRxIr_u.byte
#define f_WifiRxIrF0En             WifiRxIr_u.bit.bit0
#define f_WifiRxIrF1En             WifiRxIr_u.bit.bit1
#define f_WifiRxIrF2En             WifiRxIr_u.bit.bit2
#define f_WifiRxIrF3En             WifiRxIr_u.bit.bit3
#define f_WifiRxIrF4En             WifiRxIr_u.bit.bit4
#define f_WifiRxIrOutEn            WifiRxIr_u.bit.bit5
#define f_WifiRxIrFmEn             WifiRxIr_u.bit.bit6
#define f_WifiRxIrEn               WifiRxIr_u.bit.bit7
*/
/*============================================================================*
 *  TIME INIT
 *============================================================================*/
typedef struct {
  UTIME16 tWifitsfrmwt;
  UTIME16 tBle10msFlushWait;
  UTIME16 tUartWait10ms;
  UTIME16 tWifiTwoByteSentTime;
  UTIME16 tWifi2mscyc;
}_tm_1ms;
extern _tm_1ms tm_1ms;

typedef struct {
  UTIME16 tWifiTwoByte;
  UTIME16 twifitsrxcyc;   
  UTIME16 tWifi20mscyc;
  UTIME16 twifitstxcyc; 
}_tm_4ms;
extern _tm_4ms tm_4ms;

typedef struct {
  UTIME16 tWatchDog20ms;
  UTIME16 tWifiFiveByte1;
  UTIME16 tWifiFiveByte2;
}_tm_10ms;
extern _tm_10ms tm_10ms;

typedef struct {
  UTIME16 tWifiSendData;
  UTIME16 tSend91Wait800ms;
  UTIME16 tSend82Wait800ms;
  UTIME16 tSendEEWait800ms;
  UTIME16 tSendEAWait800ms;
  UTIME16 tmeshfinishdataWait100ms;
  UTIME16 tmfTxdataWait100ms;
}_tm_100ms;
extern _tm_100ms tm_100ms;

typedef struct {
  UTIME16 tOnTimeWait3s;
  UTIME16 tHeartPack30s;
  UTIME16 tSenHbData25s;
  UTIME16 tGetIndoorInf2s;
  UTIME16 tPanic2Min;
  UTIME16 tBleReset3Min;
  UTIME16 tAdvUUID3Min;
  UTIME16 tHeartPack5s;
  UTIME16 tMeshTimeOut2s;
  UTIME16 tRxMeshTimeOut2s;
}_tm_1s;
extern _tm_1s tm_1s;

typedef struct
{

    /* Timer for button press */
    timer_id                    button_press_tid;

}APP_HW_DATA_T;
extern APP_HW_DATA_T            g_app_hw_data;

extern volatile union FLAGS WifiTxData[40];
extern volatile union FLAGS WifiRxData[40];
#endif /* __SECURITY_TAG_H__ */
