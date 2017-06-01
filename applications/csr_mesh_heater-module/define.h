/*! \file macros.h
 *
 *  \brief Commonly used macros
 *
 *  This file should be #included by most C or C++ source files, whether
 *  it is being included in the firmware or host-side code.
 *
 * Copyright (c) 2009 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
*/

#ifndef DEFINE_H
#define DEFINE_H

/*#define BUTTON_PIO                  (0)
#define BUTTON_PIO_MASK             (PIO_BIT_MASK(BUTTON_PIO))
#define EXTRA_LONG_BUTTON_PRESS_TIMER \
                                    (4*SECOND)

*/
#define C_EIGHTHEATTEMP        (8*2+0x40)
/*250us时基*/
#define TIME250US (250)
/*1ms时基*/

#define SERIAL_RX_DATA_LENGTH           (30)
#define C_T_OnTimeWait3s                                     (0x4000+3)
#define C_T_WifiSendData                                     (0x4000+3) 
#define C_T_WifiTwoByteSentTime                              (0x4000+2) 
#define C_T_wifitsrxcyc                                      (0x4000+40)  /*发完数据后，等待300ms,如接收不到数据，重新发送*/
#define C_T_wifitstxcyc                                      (0x4000+1)
#define C_T_Wifi2mscyc                                       (0x4000+4)
#define C_T_tWifitsfrmwt                                     (0x4000+5)
#define C_T_tHeartPack5s                                    (0x4000+30)
#define C_T_tUartWait10ms                                    (0x4000+20)
#define C_T_tSendWait800ms                                   (0x4000+8)
#define C_T_tPanic2Min                                       (0x4000+60)
#define C_T_tBleReset3Min                                    (0x4000+180)
#define C_T_tAdvUUID3Min                                     (0x4000+180) 
#define C_T_tMeshTimeOut2s                                   (0x4000+2) 
#define C_T_tmeshfinishdataWait100ms                         (0x4000+1) 
#define C_T_tRxMeshTimeOut2s                                 (0x4000+2) 
#define C_T_tmfTxdataWait100ms                               (0x4000+1) 



#define NORMAL               (1) 
#define AUTOM                (2)  
#define SLEEP                (3) 

#define cTxdPrepare          (0)
#define cTxd                 (1)  
#define cTxdEnd              (2) 

#define cRxdPrepare          (0)
#define cRxdWait             (1)
#define cRxd                 (2)   
#define cRxdEnd              (3)


#define Mesh_ununited         (0) /*MESH未连接*/
#define Mesh_connecting       (1)/*MESH 正在连接*/
#define Mesh_connected        (2)/*MESH 正在连接*/

#define WifiRxData32         (1) 
#define WifiRxData44         (2)    
#define WifiRxData31         (3)   
#define WifiRxData33         (4)   
#define WifiRxData34         (5)
#define WifiRxData35         (6)

 

#define SWINGSTOP              (0)   
#define SWING15                (1)   
#define SWING1                 (2)   
#define SWING2                 (3)   
#define SWING3                 (4)  
#define SWING4                 (5)  
#define SWING5                 (6)   
#define SWING35                (7)   
#define SWING25                (8)   
#define SWING24                (9)   
#define SWING14                (10)   
#define SWING13                (11)   
#define SWING135               (12)

#define LLFAN                (1)   
#define LFAN                 (2)   
#define LMFAN                (3)   
#define MFAN                 (4)   
#define HMFAN                (5)   
#define HFAN                 (6)   
#define SFAN                 (7)   
#define HHFAN                (8)   
#define HHHFAN               (9) 
#define HHHHFAN              (10) 

#endif
