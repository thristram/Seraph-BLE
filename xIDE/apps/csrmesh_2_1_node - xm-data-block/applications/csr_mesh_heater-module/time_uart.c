/******************************************************************************
 *  Copyright (c) 2016 donse young
 *
 *****************************************************************************/
/*============================================================================*
 *  Local Header File
 *============================================================================*/
#include "label.h"
#include "macros.h"
#include "app_debug.h"
#include "iot_hw.h"
#include <pio.h>
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
uint8 *const chp_t1msec_adr[]=
{
  (uint8*)&tm_1ms.tWifitsfrmwt.word,
  (uint8*)&tm_1ms.tBle10msFlushWait.word,
  (uint8*)&tm_1ms.tUartWait10ms.word, 
  (uint8*)&tm_1ms.tWifiTwoByteSentTime.word,
  (uint8*)&tm_1ms.tWifi2mscyc.word,
  (uint8*)&tm_1ms.tWifi5mscyc.word,
  
};
#define TM1MSEC (sizeof(chp_t1msec_adr)/sizeof(chp_t1msec_adr[0]))

uint16 *const chp_t4msec_adr[]=
{
  (uint16*)&tm_4ms.tWifiTwoByte.word,
  (uint16*)&tm_4ms.twifitsrxcyc.word,
  (uint16*)&tm_4ms.twifitstxcyc.word,
};
#define TM4MSEC (sizeof(chp_t4msec_adr)/sizeof(chp_t4msec_adr[0]))

uint8 *const chp_t10msec_adr[]=
{
  (uint8*)&tm_10ms.tWifiSendData1.word,
  
};
#define TM10MSEC (sizeof(chp_t10msec_adr)/sizeof(chp_t10msec_adr[0]))

uint8 *const chp_t100msec_adr[]=
{
  (uint8*)&tm_100ms.tWifiSendData.word,
  (uint8*)&tm_100ms.tSendEEWait800ms.word,
  (uint8*)&tm_100ms.tSendEAWait800ms.word,
  (uint8*)&tm_100ms.tSend82Wait800ms.word, 
  (uint8*)&tm_100ms.tmeshfinishdataWait100ms.word, 
  (uint8*)&tm_100ms.tmfTxdataWait100ms.word,  
  (uint8*)&tm_100ms.tWriteFlashDelay.word,
  (uint8*)&tm_100ms.tRstWait500ms.word,
  
};
#define TM100MSEC (sizeof(chp_t100msec_adr)/sizeof(chp_t100msec_adr[0]))
  
uint8 *const chp_t1sec_adr[]=
{
  (uint8*)&tm_1s.tOnTimeWait3s.word,
  (uint8*)&tm_1s.tHeartPack5s.word,  
  (uint8*)&tm_1s.tPanic2Min.word,  
  (uint8*)&tm_1s.tBleReset3Min.word,
  (uint8*)&tm_1s.tAdvUUID3Min.word,
  (uint8*)&tm_1s.tMeshTimeOut2s.word,
  (uint8*)&tm_1s.tRxMeshTimeOut2s.word,
  (uint8*)&tm_1s.tPoweronWait3s.word,
  
};
#define TM1SEC (sizeof(chp_t1sec_adr)/sizeof(chp_t1sec_adr[0]))

void timer_RBINT(void)
{
  uint8 wuc_loop;
  m4msCount++;
  if(m4msCount >= 4)
  {
    f_Time4ms = ON;
    m4msCount = CLEAR;
  }
  
  for(wuc_loop =0;wuc_loop < TM1MSEC; ++wuc_loop)
  {
    if((*(chp_t1msec_adr[wuc_loop]) & TIME16RQ) != 0)
    {

      if((*chp_t1msec_adr[wuc_loop] == TIME16RQ) || (*chp_t1msec_adr[wuc_loop] == TIME16OVRQ))
      {
        *(chp_t1msec_adr[wuc_loop]) = TIME16OV;
      }
      else
      {
        --*(chp_t1msec_adr[wuc_loop]);
        if((*chp_t1msec_adr[wuc_loop] == TIME16RQ) || (*chp_t1msec_adr[wuc_loop] == TIME16OVRQ))
        {
          *(chp_t1msec_adr[wuc_loop]) = TIME16OV;
        }
      }
    }
  } 
  
   if(f_Time4ms == ON)
  {

    f_Time4ms = OFF;
    
    for(wuc_loop =0;wuc_loop < TM4MSEC; ++wuc_loop)
    {
   
      if((*(chp_t4msec_adr[wuc_loop]) & TIME16RQ) != 0)
      {
        
        if((*chp_t4msec_adr[wuc_loop] == TIME16RQ) || (*chp_t4msec_adr[wuc_loop] == TIME16OVRQ))
        {
          *(chp_t4msec_adr[wuc_loop]) = TIME16OV;
        }
        
        else
        {
          --*(chp_t4msec_adr[wuc_loop]);
          if((*chp_t4msec_adr[wuc_loop] == TIME16RQ) || (*chp_t4msec_adr[wuc_loop] == TIME16OVRQ))
          {
            *(chp_t4msec_adr[wuc_loop]) = TIME16OV;
          }
        
        }
        
      }
      
    }
   
  } 
}
void timer_10ms_RBINT(void)
{
  uint8 wuc_loop;

  for(wuc_loop =0;wuc_loop < TM10MSEC; ++wuc_loop)
  {
    if((*(chp_t10msec_adr[wuc_loop]) & TIME16RQ) != 0)
    {

      if((*chp_t10msec_adr[wuc_loop] == TIME16RQ) || (*chp_t10msec_adr[wuc_loop] == TIME16OVRQ))
      {
        *(chp_t10msec_adr[wuc_loop]) = TIME16OV;
      }
      else
      {
        --*(chp_t10msec_adr[wuc_loop]);
        if((*chp_t10msec_adr[wuc_loop] == TIME16RQ) || (*chp_t10msec_adr[wuc_loop] == TIME16OVRQ))
        {
          *(chp_t10msec_adr[wuc_loop]) = TIME16OV;
        }
      }
    }
  } 
}

void timer_100ms_RBINT(void)
{
  uint8 wuc_loop;

      for(wuc_loop =0;wuc_loop < TM100MSEC; ++wuc_loop)
      {
        if((*(chp_t100msec_adr[wuc_loop]) & TIME16RQ) != 0)
        {
          if((*chp_t100msec_adr[wuc_loop] == TIME16RQ) || (*chp_t100msec_adr[wuc_loop] == TIME16OVRQ))
          {
            *(chp_t100msec_adr[wuc_loop]) = TIME16OV;
          }
          else
          {
            --*(chp_t100msec_adr[wuc_loop]);
            if((*chp_t100msec_adr[wuc_loop] == TIME16RQ) || (*chp_t100msec_adr[wuc_loop] == TIME16OVRQ))
            {
              *(chp_t100msec_adr[wuc_loop]) = TIME16OV;
            }
          }
        }
      }
}
void timer_1s_RBINT(void)
{
  uint8 wuc_loop;

      for(wuc_loop =0;wuc_loop < TM1SEC; ++wuc_loop)
      {
        if((*(chp_t1sec_adr[wuc_loop]) & TIME16RQ) != 0)
        {
          if((*chp_t1sec_adr[wuc_loop] == TIME16RQ) || (*chp_t1sec_adr[wuc_loop] == TIME16OVRQ))
          {
            *(chp_t1sec_adr[wuc_loop]) = TIME16OV;
          }
          else
          {
            --*(chp_t1sec_adr[wuc_loop]);
            if((*chp_t1sec_adr[wuc_loop] == TIME16RQ) || (*chp_t1sec_adr[wuc_loop] == TIME16OVRQ))
            {

              *(chp_t1sec_adr[wuc_loop]) = TIME16OV;
            }
          }
        }
      } 
}