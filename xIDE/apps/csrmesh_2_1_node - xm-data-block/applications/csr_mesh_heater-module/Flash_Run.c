/******************************************************************************
 *  Copyright (c) 2016 donse young
 *
 *****************************************************************************/
/*============================================================================*
 *  Local Header File
 *============================================================================*/



#include <mem.h>
#include <nvm.h>
#include "label.h"
#include "macros.h"
#include "define.h"
#include "app_debug.h"
#include "nvm_access.h"
#include "main_app.h"
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static char FlashBuf[NVM_BLOCK_SZ];

//#define debug

void flash_run(void)
{
     MemSet(FlashBuf, NULL, sizeof(FlashBuf));
     if(f_Ble_Reset == ON && tm_100ms.tRstWait500ms.fov == ON)
     {
          tm_100ms.tRstWait500ms.word = CLEAR;
          MemSet(FlashBuf, NULL, sizeof(FlashBuf));
          Nvm_Write((uint16 *)FlashBuf,sizeof(FlashBuf),OffSetAddr);
          Reset_BLE_Module();
     }
     if(FlashWrite == ON && tm_100ms.tWriteFlashDelay.fov == ON && tm_1s.tPoweronWait3s.fov == ON)
     {
          FlashWrite = OFF;
          tm_100ms.tWriteFlashDelay.word = CLEAR;
          FlashBuf[0] = (Local_MESH_ID >> 8) & 0x00ff;
          FlashBuf[1] = Local_MESH_ID & 0x00ff;
          FlashBuf[2] = (Gateway_MsgID >> 8) & 0x00ff;
          FlashBuf[3] = Gateway_MsgID & 0x00ff; 
          FlashBuf[4] = Mesh_status;
          FlashBuf[5] = ModWorkMode;
          Nvm_Write((uint16 *)FlashBuf,sizeof(FlashBuf),NVM_BLE_STATUS_MEMORY_WORDS);
     }
     
     else if(FlashRead == ON && tm_1s.tPoweronWait3s.fov == ON)
     {
          FlashRead = OFF;
          MemSet(FlashBuf, NULL, sizeof(FlashBuf));
          Nvm_Read((uint16 *)FlashBuf,sizeof(FlashBuf),NVM_BLE_STATUS_MEMORY_WORDS);
          Local_MESH_ID = (uint16)FlashBuf[0] << 8 | (uint16)FlashBuf[1];
          Gateway_MsgID = (uint16)FlashBuf[2] << 8 | (uint16)FlashBuf[3];
          Mesh_status = FlashBuf[4];
          ModWorkMode = FlashBuf[5];
        
          
     }
     
     
}









